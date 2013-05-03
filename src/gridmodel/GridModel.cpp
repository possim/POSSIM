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

#include "GridModel.h"

/* Determines what the inductivePower has to be to achieve given powerFactor for given activePower */
double getQfromPF(double activePower, double powerFactor) {
    if(activePower == 0) return 0;
    return tan(acos(powerFactor))*activePower;
}


GridModel::GridModel() {
}

GridModel::~GridModel() {
}

void GridModel::initialise(Config* config, LoadFlowInterface* loadflow) {
    distTransCapacity = config->getInt("disttranscapacity");
    powerFactor = config->getDouble("powerfactor");
    evPenetration = config->getInt("evpenetration");
    minVoltage = config->getInt("minvoltage");
    availableCapacity = 0;
    
    loadGridModel(loadflow);
    addVehicles(config);
}

void GridModel::loadGridModel(LoadFlowInterface* lf) {
    Household* newHouse;
    std::vector <std::string> houseNames;
    std::vector<std::string> tokens;
    std::string name;
    int nmi;          
    double demandProfile;
    
    // set handle to loadflow
    loadflow = lf;
    
    // get house names
    houseNames = loadflow->getHouseNames();
    std::cout << " - MATLAB model contains " << houseNames.size() << " houses" << std::endl;

    // create each house (find name, parent transformer)
    for(size_t i=0; i<houseNames.size(); i++) {
        
        // Find name, nmi
        try {
            name = houseNames.at(i);
            nmi = i+1;
            std::cout << nmi << " " << name << std::endl;
        }
        catch(const std::out_of_range& e) {
            std::cout << "Error: household has invalid component name syntax (name)" << std::endl;
            name = "House x";
            nmi = 999;
        }                 
        
        // Assign this house a demand profile (according to normal distribution)
        // demandProfile = utility::randomNormal(1, 0.25, 0.5, 1.5);
        demandProfile = 1;
        
        // Add house
        newHouse = new Household(nmi, name, demandProfile);
        households.insert(std::pair<int, Household>(nmi,*newHouse));
        //std::cout << "   - House " << nmi << " has demand profile " << demandProfile << std::endl;
    }
}

void GridModel::addVehicles(Config* config) {
    std::ifstream infile;
    std::string line;
    std::string houseName, vehicleName;
    int nmi, numEVs;
    Vehicle *newVehicle;
    std::vector<int> indexVector;
    bool generateRandom = false;
    
    // Check if vehicle file exists
    std::string inputFile = config->getString("vehicleassignment");
    
    if(strcmp(inputFile.c_str(), "random") == 0)
        generateRandom = true;
    else {
        infile.open(inputFile.c_str());
        if(!infile){
           std::cout << "Cannot open vehicle profile input file " << inputFile << "; will generate random vehicle locations instead." << std::endl;
           generateRandom = true;
        }
    }
    
    if(generateRandom) {
        srand((unsigned int)(time(NULL)));

        // Calculate number of EVs
        numEVs = std::min(households.size()*evPenetration/100, households.size());
        std::cout << " - Randomly adding " << numEVs << " vehicles (" << evPenetration << "%) ...";
        std::cout.flush();

        // Randomly pick from households to associate EVs
        for(size_t i=1; i<=households.size(); i++) indexVector.push_back(i);
        utility::random_unique(indexVector.begin(), indexVector.end(), numEVs);
    }
    
    else { // take vehicle locations from file
        numEVs=0;
        while(getline(infile, line)) {
            numEVs++;
            indexVector.push_back(utility::string2int(line));
        }
        evPenetration = 100*numEVs/households.size();
        config->setConfigVar("evpenetration", utility::int2string(evPenetration));
        std::cout << " - Added " << numEVs << " vehicles (" << evPenetration << "%) ...";
    }
    
    // Create vector of EVs and add to grid model
    for(int i=0; i<numEVs; i++) {
        nmi = indexVector[i];
        // WARNING SHOULD HAVE BETTER ERROR CHECKING HERE
        houseName = households.at(nmi).getName();
        households.at(nmi).hasCar = true;

        int lastindex = houseName.find_last_of("/"); 
        vehicleName = houseName.substr(0, lastindex).append("/Vehicle");

        newVehicle = new Vehicle(config, nmi, vehicleName);
        vehicles.insert(std::pair<int,Vehicle>(nmi,*newVehicle));
        loadflow->addVehicle(*newVehicle);
    } 
    
    std::cout<<"......................... OK"<<std::endl;
}

void GridModel::updateVehicleBatteries() {
    for(std::map<int,Vehicle>::iterator it = vehicles.begin(); it != vehicles.end(); ++it) {
        if(it->second.distanceDriven > 0)
            it->second.dischargeBattery();
        else if(it->second.isConnected)
            it->second.rechargeBattery();
    }
}

void GridModel::generateLoads(DateTime currTime, HouseholdDemand householdDemand) {
    double activePower, inductivePower;
    double randomFactor;
    double sumLoads = 0;

    for(std::map<int,Household>::iterator it = households.begin(); it != households.end(); ++it) {
        activePower = it->second.getDemandProfile() * householdDemand.getDemandAt(currTime, it->second.getNMI());
        //randomFactor = utility::randomNormal(0, 0.1, -0.3, 1-powerFactor);
        randomFactor = 0;
        
        inductivePower = getQfromPF(activePower, powerFactor + randomFactor);
        it->second.setPowerDemand(activePower, inductivePower, 0);
        sumLoads += activePower;
    }
    availableCapacity = distTransCapacity - sumLoads;

    for(std::map<int,Vehicle>::iterator it = vehicles.begin(); it != vehicles.end(); ++it) 
        it->second.setPowerDemand(it->second.chargeRate, 0, 0); 
}

void GridModel::displayVehicleSummary() {
    std::cout << "Vehicle Summary" << std::endl;
    std::cout << "  NMI Status      Con Cha dSOC  ChRa  L    N    P      SOC" << std::endl
              << "  ---------------------------------------------------------" << std::endl;
    for(std::map<int,Vehicle>::iterator it = vehicles.begin(); it != vehicles.end(); ++it) {

        // Vehicle NMI (3 chars)
        std::cout << "  " << std::setw(3) << std::right << it->second.getNMI() << " ";
 
        // Vehicle status - home, returned, away (11 char)
        if(it->second.location != Home)
            std::cout << "Away        ";

        else if(it->second.distanceDriven > 0)
            std::cout << "Drove " << std::setw(3) << std::right << it->second.distanceDriven << "km ";

        else 
            std::cout << "Home        ";
        
        // Vehicle connected
        if(it->second.isConnected)
            std::cout << "Yes ";
        else
            std::cout << "No  ";

        // Vehicle charging
        if(it->second.isCharging)
            std::cout << "Yes ";
        else
            std::cout << "No  ";
        
        // Last SOC inc
        std::cout << std::setw(5) << std::right << std::setprecision(2) << it->second.getSOCchange() << " ";

        // Charge rate
        std::cout << std::setw(4) << std::right << std::setprecision(0) << it->second.chargeRate << " ";
     
        // L
        std::cout << std::setw(4) << std::right << std::setprecision(2) << it->second.L << " ";
        // N
        std::cout << std::setw(4) << std::right << std::setprecision(2) << it->second.N << " ";
        // P
        std::cout << std::setw(4) << std::right << std::setprecision(2) << it->second.P << " ";
        
        // Switch on?
        if(it->second.switchon)
            std::cout << "* ";
        else
            std::cout << "  ";
        
        // SOC
        std::cout << std::setw(5) << std::right << std::setprecision(1) << it->second.getSOC() << std::endl;
    }
}

void GridModel::displayFullSummary() {
    std::cout << "Household demand:" << std::endl;
    for(std::map<int,Household>::iterator it = households.begin(); it != households.end(); ++it)
        std::cout<< "   House " << std::setw(2) << std::right << it->second.getNMI() << ": " 
                 << std::setw(7) << std::right << std::setiosflags(std::ios::fixed) << std::setprecision(2) << it->second.activePower << " W, PF: " 
                 << std::setw(4) << std::right << std::setiosflags(std::ios::fixed) << std::setprecision(2) << it->second.getPowerFactor() << std::endl;

    std::cout << "Vehicle demand:" << std::endl;
    for(std::map<int,Vehicle>::iterator it = vehicles.begin(); it != vehicles.end(); ++it)
        std::cout<< " Vehicle " << std::setw(2) << std::right << it->second.getNMI() << ": " 
                 << std::setw(7) << std::right << std::setiosflags(std::ios::fixed) << std::setprecision(2) << it->second.activePower << " W, PF: " 
                 << std::setw(4) << std::right << std::setiosflags(std::ios::fixed) << std::setprecision(2) << it->second.getPowerFactor() << std::endl;
}

void GridModel::displayLoadSummary() {
    double hhSum=0, hhPF=0, hhMin=100000, hhMax=0;
    double evSum=0, evPF=0, evMin=100000, evMax=0;
    int evHome=0, evCharging=0;
    double currPower;
    

    for(std::map<int,Vehicle>::iterator it = vehicles.begin(); it != vehicles.end(); ++it) {
        if(it->second.location == Home) {
            evHome++;
            if(it->second.isCharging) 
                evCharging++;
            
            currPower = it->second.activePower;

            evPF += it->second.getPowerFactor();
            evSum += currPower;
            if(currPower < evMin)
                evMin = currPower;
            if(currPower > evMax)
                evMax = currPower;
        }
    }
    for(std::map<int,Household>::iterator it = households.begin(); it != households.end(); ++it) {
        currPower = it->second.activePower;
        
        hhPF += it->second.getPowerFactor();
        hhSum += currPower;
        if(currPower < hhMin)
            hhMin = currPower;
        if(currPower > hhMax)
            hhMax = currPower;
    }

    
    // if no EVs connected, no min found
    if(evMin == 100000)
        evMin = 0;
    
    std::cout << "Vehicle Loads" << std::endl
              << "  Total Loads   : " << std::setw(4) << std::right << std::setiosflags(std::ios::fixed) << evCharging << std::endl
              << "  Avg Load      : " << std::setw(7) << std::right << std::setiosflags(std::ios::fixed) << std::setprecision(2) << evSum/evCharging << " W"  << std::endl
              << "  Max           : " << std::setw(7) << std::right << std::setiosflags(std::ios::fixed) << std::setprecision(2) << evMax << " W"  << std::endl
              << "  Min           : " << std::setw(7) << std::right << std::setiosflags(std::ios::fixed) << std::setprecision(2) << evMin << " W"  << std::endl
              << "  Avg PF        : " << std::setw(7) << std::right << std::setiosflags(std::ios::fixed) << std::setprecision(2) << evPF/evCharging << std::endl;
    std::cout << "Household Loads" << std::endl
              << "  Total Loads   : " << std::setw(4) << std::right << std::setiosflags(std::ios::fixed) << households.size() << std::endl
              << "  Avg Load      : " << std::setw(7) << std::right << std::setiosflags(std::ios::fixed) << std::setprecision(2) << hhSum/households.size() << " W"  << std::endl
              << "  Max           : " << std::setw(7) << std::right << std::setiosflags(std::ios::fixed) << std::setprecision(2) << hhMax << " W"  << std::endl
              << "  Min           : " << std::setw(7) << std::right << std::setiosflags(std::ios::fixed) << std::setprecision(2) << hhMin << " W"  << std::endl
              << "  Avg PF        : " << std::setw(7) << std::right << std::setiosflags(std::ios::fixed) << std::setprecision(2) << hhPF/households.size() << std::endl;
}


void GridModel::runValleyLoadFlow(DateTime datetime, HouseholdDemand householdDemand) {
    boost::posix_time::ptime time_startLoadFlow(boost::posix_time::microsec_clock::local_time());
    std::cout << " - Running valley load flow analysis ... ";
    std::cout.flush();
    
    // Choose valley time
    DateTime valleytime = datetime;
    valleytime.hour = 4;
    valleytime.minute = 0;

    // Set all household loads to valley load, vehicle loads to zero
    double activePower, inductivePower;
    
    for(std::map<int,Household>::iterator it = households.begin(); it != households.end(); ++it) {
        activePower = householdDemand.getDemandAt(valleytime, it->second.getNMI()) * 1000;
        inductivePower = getQfromPF(activePower, powerFactor);
        loadflow->setDemand(it->second.getName(), activePower, inductivePower, 0);
    }
    for(std::map<int,Vehicle>::iterator it = vehicles.begin(); it != vehicles.end(); ++it)
        loadflow->setDemand(it->second.getName(), 0, 0, 0);
    
    loadflow->runSim();
    
    loadflow->getOutputs(phaseV, phaseI, eolV, households);
    for(std::map<int,Household>::iterator it = households.begin(); it != households.end(); ++it)
        it->second.V_valley = it->second.V_RMS;

    std::cout << "complete, took: " << utility::timeDisplay(utility::timeElapsed(time_startLoadFlow)) << std::endl;
}


void GridModel::runLoadFlow(DateTime currTime) {
    boost::posix_time::ptime time_startLoadFlow = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::time_duration length_loadFlow;

    std::cout << "Running load flow analysis ... ";
    std::cout.flush();

    for(std::map<int,Household>::iterator it = households.begin(); it != households.end(); ++it)
        loadflow->setDemand(it->second.getName(), it->second.activePower, it->second.inductivePower, it->second.capacitivePower);

    for(std::map<int,Vehicle>::iterator it = vehicles.begin(); it != vehicles.end(); ++it)
        loadflow->setDemand(it->second.getName(), it->second.activePower, it->second.inductivePower, it->second.capacitivePower);
    
    loadflow->runSim();
    
    loadflow->getOutputs(phaseV, phaseI, eolV, households);

    length_loadFlow = boost::posix_time::microsec_clock::local_time() - time_startLoadFlow;
    std::cout << "complete, took: " << utility::timeDisplay((long)(length_loadFlow.total_milliseconds())) << std::endl;
}


double GridModel::getAvailableCapacity() {
    return availableCapacity;
}