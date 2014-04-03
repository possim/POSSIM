/* 
Software License Agreement (BSD License)

Copyright (c) 2013, Julian de Hoog <julian@dehoog.ca>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

 * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above
   copyright notice, this list of conditions and the following
   disclaimer in the documentation and/or other materials provided
   with the distribution.
 * The name of the author may not be used to endorse or promote 
   products derived from this software without specific prior 
   written permission from the author.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE. 
*/

#include "Simulator.h"

Simulator::Simulator(Config* configIn):
        householdDemandModel(configIn),
        trafficModel(configIn),
        spotPrice(configIn)
{
    std::cout << "Loading Simulator ..." << std::endl;
    
    // Store local pointer to config object
    config = configIn;

    // Set some global parameters locally for convenience
    startTime.set(config->getConfigVar("starttime"));
    currTime = startTime;
    finishTime.set(config->getConfigVar("finishtime"));
    showDebug = config->getBool("showdebug");

    // Create interface to load flow simulator
    switch(config->getLoadFlowSim()) {
        case 0:         loadflow = new TestingInterface(config);
                        break;
        case 1:         loadflow = new MatlabInterface(config);
                        break;
        default:        loadflow = new MatlabInterface(config);
                        break;
    }
    
    // Load model, set initial values
    gridModel.initialise(config, loadflow);
    
    // Assign demand profiles to houses
    householdDemandModel.assignProfiles(gridModel.households);
    
    // Add vehicles to gridmodel
    gridModel.addVehicles(config);
    
    // Assign first vehicle profiles
    trafficModel.initialise(startTime, gridModel.vehicles);

    // Determine what kind of charging update method to use
    chargeRateUpdate = config->getConfigVar("chargerateupdate");
    
    // If batch, update all vehicles at the same time
    if(chargeRateUpdate == "batch")
        chargeRateGroupSize = gridModel.vehicles.size();
    
    // If individual, update vehicles one at a time
    else if(chargeRateUpdate == "individual")
        chargeRateGroupSize = 1;
    
    // Otherwise grouped
    else
        chargeRateGroupSize = config->getInt("chargerategroupsize");
    
    // Determine whether to update in ordered or random way
    chargeRateOrder = config->getConfigVar("chargerateorder");
    
    // Create array of vehicle IDs, for determining order of charge updates
    for(std::map<std::string,Vehicle*>::iterator it = gridModel.vehicles.begin(); it != gridModel.vehicles.end(); ++it)
        vehicleIDs.push_back(it->second->NMI);
    
    // Initialise log
    log.initialise(config, gridModel);
    gridModel.setLogDir(log.getDir());

    // Create charging algorithm
    switch(config->getChargingAlg()) {
        case 0:         charger = new ChargingUncontrolled(config, gridModel);
                        break;
        case 1:         charger = new ChargingEqualShares(config, gridModel);
                        break;
        case 2:         charger = new ChargingDistributed(config, gridModel, startTime);
                        break;
        case 4:         charger = new ChargingTOU(config, gridModel);
                        break;
/*        case 5:         charger = new ChargingOptimal1(config, gridModel, loadflow, log.getDir());
                        break;
        case 6:         charger = new ChargingOptimal2(config, gridModel, loadflow, log.getDir(), &spotPrice);
                        break;
        case 7:         charger = new ChargingOptimal3(config, gridModel, loadflow, log.getDir(), &spotPrice);
                        break;
        case 8:         charger = new ChargingDiscrete(config, gridModel, startTime);
                        break;
        case 10:        charger = new ChargingWplug(config, gridModel, startTime);
                        break;
        case 11:        charger = new ChargingWplug2(config, gridModel, startTime);
                        break;*/
        default:        charger = new ChargingUncontrolled(config, gridModel);
                        break;
    }
    
    // Optional: print load flow model to log directory
    //loadflow->printModel(log.getDir());
    
    std::cout << " - Simulator loaded OK" << std::endl << std::endl;
}

Simulator::~Simulator() {
}

// Provide an update on how long the simulation is expected to take to the user
void Simulator::timingUpdate(boost::posix_time::time_duration lastCycleLength) {
    long total = finishTime.minus(startTime, config->getInt("simulationinterval"));
    long complete = currTime.minus(startTime, config->getInt("simulationinterval"));
    long togo = finishTime.minus(currTime, config->getInt("simulationinterval"));
    
    std::cout << "Simulation progress: " << std::endl
              << " - Start  : " << startTime.toString() << std::endl
              << " - Now at : " << currTime.toString() << std::endl
              << " - End    : " << finishTime.toString() << std::endl
              << " - " << std::setprecision(2) << double(complete)/double(total)*100 << "% complete, approximately " 
              << utility::timeDisplay(togo*(long)(lastCycleLength.total_milliseconds())) << " remaining." << std::endl;
}

// Run simulation
void Simulator::run() {
    // Several variables to keep track of how long cycle / full simulation took
    boost::posix_time::ptime timerRun, timerCycle;
    boost::posix_time::time_duration lastCycleLength;
    utility::startTimer(timerRun);
    utility::startTimer(timerCycle);
        
    std::cout << "Starting Simulation ... " << std::endl;
    
    // Outer simulation loop
    while(!currTime.isLaterThan(finishTime)) {
        std::cout << "-------------------------------------------" << std::endl;        
        currTime.display();
        std::cout << std::endl;

        // Update electricity spot price
        spotPrice.update(currTime);
        
        // Update traffic model
        trafficModel.update(currTime, gridModel.vehicles);
        
        // Update vehicles' battery SOC based on distance driven / charging
        gridModel.updateVehicleBatteries();

        // Update grid model - generate household loads & reset vehicle loads
        gridModel.generateAllHouseholdLoads(currTime);
        gridModel.resetVehicleLoads();

        // Depending on charge update model, apply charge rate updates and run load flow
        if(chargeRateOrder == "random")
            std::random_shuffle(vehicleIDs.begin(), vehicleIDs.end());
        
        // Determine EV charging rates
        if(chargeRateUpdate == "batch") {
            charger->setAllChargeRates(currTime, gridModel);
            gridModel.generateAllVehicleLoads();
            gridModel.runLoadFlow(currTime);
        }
        
        else if(chargeRateUpdate == "individual") {
            gridModel.runLoadFlow(currTime);
            for(int i=0; i<vehicleIDs.size(); i++) {
                charger->setOneChargeRate(currTime, gridModel, vehicleIDs.at(i));
                gridModel.generateOneVehicleLoad(vehicleIDs.at(i));
                gridModel.runLoadFlow(currTime);
            }
        }
        
        else {  // "grouped"
            gridModel.runLoadFlow(currTime);
            for(int i=0; i<vehicleIDs.size(); i+=chargeRateGroupSize) {
                for(int j=i; j<std::min((int)gridModel.vehicles.size(), (int)(i+chargeRateGroupSize)); j++) {
                    charger->setOneChargeRate(currTime, gridModel, vehicleIDs.at(i));
                    gridModel.generateOneVehicleLoad(vehicleIDs.at(i));
                }
                gridModel.runLoadFlow(currTime);
            }
        }

        // If desired, show some results
        if(showDebug) {
            spotPrice.display();
            trafficModel.displaySummary(gridModel.vehicles);
            gridModel.displayVehicleSummary();
            gridModel.displayLoadSummary(currTime);
        }

        // Provide quick update on timing to output
        timingUpdate(lastCycleLength);

        // Log data
        log.update(currTime, gridModel, charger, spotPrice);
        
        // Add optional user specified delay into cycle
        // while(utility::timediff(boost::posix_time::microsec_clock::local_time(), time_cycleStart) < config->getInt("intervaldelay"));
        
        // Estimate timing
        lastCycleLength = boost::posix_time::microsec_clock::local_time() - timerCycle;
        std::cout << "Cycle complete, took: " << utility::updateTimer(timerCycle) << std::endl;

        currTime.increment(config->getInt("simulationinterval"));
    }
    
    // Simulation complete, provide some output, generate report.
    std::cout << "-------------------------------------------" << std::endl
              << "Simulation complete, took " << utility::endTimer(timerRun) << std::endl;
    if(config->getBool("generatereport")) {
        std::cout << "Generating report ..." << std::endl; 
        loadflow->generateReport(log.getDir(), currTime.month, currTime.isWeekday(), config->getInt("simulationinterval"));
        std::cout << "Report written to " << log.getDir() << std::endl; 
    }
    std::cout << "-------------------------------------------" << std::endl; 
}


