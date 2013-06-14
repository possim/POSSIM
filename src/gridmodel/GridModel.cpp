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




GridModel::GridModel() {
}

GridModel::~GridModel() {
}

void GridModel::initialise(Config* config, LoadFlowInterface* lf) {
    // Save some global parameters locally for convenience
    baseVoltage = config->getDouble("basevoltage");
    averagePowerFactor = config->getDouble("powerfactor");
    evPenetration = config->getInt("evpenetration");
    minVoltage = config->getInt("minvoltage");
    sumHouseholdLoads = 0;
    
    // set handle to loadflow
    loadflow = lf;
    
    // Create transformer
    transformer = new DistributionTransformer();
    
    // Extract structure, parameters, numHouses, etc from model file
    loadflow->extractModel(root, transformer, poles, lineSegments, households);
    
    // Households should now be mapped to their name (string), also create
    // map to NMI for convenience (sometimes houses need to be searched by NMI)
    buildHouseholdNMImap();
    
    // Determine total impedance from transformer to every house
    calculateHouseholdZ(root, 0, 0);
    
    // Update transformer capacity if config is different from value
    // extracted from model
    if(transformer->capacity != config->getDouble("txcapacity")) {
        std::cout << " - transformer capacity was " << transformer->capacity
                  << ", resetting to " << config->getDouble("txcapacity") << std::endl;
        loadflow->setTxCapacity(transformer->name, config->getDouble("txcapacity"));
        transformer->capacity = config->getDouble("txcapacity");
    }
}

void GridModel::loadGridModel() {
    Household* newHouse;
    std::vector <std::string> houseNames;
    std::string name;
    int nmi;          
    double demandProfile;
    

    //loadflow->parseModel(root, poles, lineSegments, households);
    
    // get house names
    houseNames = loadflow->getHouseNames();
    std::cout << " - MATLAB model contains " << houseNames.size() << " houses";

    // create each house (find name, parent transformer)
    for(size_t i=0; i<houseNames.size(); i++) {
        
        // Find name, nmi
        try {
            name = houseNames.at(i);
            nmi = i+1;
            //std::cout << nmi << " " << name << std::endl;
        }
        catch(const std::out_of_range& e) {
            std::cout << "Error: household has invalid component name syntax (name)" << std::endl;
            name = "House x";
            nmi = 999;
        }                 
        
        // Add house
        newHouse = new Household();
        newHouse->NMI = nmi;
        newHouse->name = name;
        newHouse->baseVoltage = baseVoltage;
        households.insert(std::pair<std::string, Household*>(name,newHouse));
        //std::cout << "   - House " << nmi << " has demand profile " << demandProfile << std::endl;
    }
    
    std::cout << ", added OK" << std::endl;
}

void GridModel::addVehicles(Config* config) {
    std::ifstream infile;
    std::string line;
    std::string houseName, vehicleName;
    int nmi, numEVs;
    Vehicle *newVehicle;
    std::vector<int> indexVector;
    bool generateRandom = false;
    Household *currHousehold;
    
    // To be safe, ensure all household hasCar booleans are set to false until
    // vehicles are added
    for(std::map<std::string,Household*>::iterator it=households.begin(); it!=households.end(); ++it)
        it->second->hasCar = false;
    
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
        currHousehold = findHousehold(nmi);
        //std::cout << "Want to add vehicle to: " << currHousehold->name << std::endl;
        currHousehold->hasCar = true;
        vehicleName = currHousehold->name;
        vehicleName.append("_EV");

        newVehicle = new Vehicle(config, nmi, vehicleName, currHousehold->name);
        newVehicle->componentName = currHousehold->componentName.substr(0, currHousehold->componentName.find_last_of('/')+1);
        newVehicle->componentName.append("Vehicle");
        //std::cout << "Vehicle " << newVehicle->name << " will have component name " << newVehicle->componentName << std::endl;
        vehicles[vehicleName] = newVehicle;
        loadflow->addVehicle(*newVehicle);
    } 
    
    buildVehicleNMImap();
    
    std::cout<<" OK"<<std::endl;
}

void GridModel::updateVehicleBatteries() {
    for(std::map<std::string,Vehicle*>::iterator it = vehicles.begin(); it != vehicles.end(); ++it) {
        if(it->second->distanceDriven > 0)
            it->second->dischargeBattery();
        else if(it->second->isConnected && it->second->isCharging)
            it->second->rechargeBattery();
    }
}

void GridModel::generateLoads(DateTime currTime, HouseholdDemandModel householdDemand) {
    std::cout << " - Generating household loads ...";
    double activePower, reactivePower;
    sumHouseholdLoads = 0;

    for(std::map<std::string,Household*>::iterator it = households.begin(); it != households.end(); ++it) {
        activePower = it->second->demandProfileFactor * it->second->getDemandAt(currTime).P;
        reactivePower = it->second->demandProfileFactor * it->second->getDemandAt(currTime).Q;

        sumHouseholdLoads += activePower;
    }
    
    for(std::map<std::string,Vehicle*>::iterator it = vehicles.begin(); it != vehicles.end(); ++it) 
        it->second->setPowerDemand(it->second->chargeRate, 0, 0); 
    
    std::cout << " OK" << std::endl;
}

void GridModel::displayVehicleSummary() {
    std::cout << "Vehicle Summary" << std::endl;
    std::cout << "  Name   Status      Con Cha dSOC  ChRa    SOC" << std::endl
              << "  -----------------------------------------------------------" << std::endl;
    for(std::map<std::string,Vehicle*>::iterator it = vehicles.begin(); it != vehicles.end(); ++it) {

        // Vehicle NMI (3 chars)
        std::cout << "  " << std::setw(6) << std::right << it->second->name << " ";
 
        // Vehicle status - home, returned, away (11 char)
        if(it->second->location != Home)
            std::cout << "Away        ";

        else if(it->second->distanceDriven > 0)
            std::cout << "Drove " << std::setw(3) << std::right << it->second->distanceDriven << "km ";

        else 
            std::cout << "Home        ";
        
        // Vehicle connected
        if(it->second->isConnected)
            std::cout << "Yes ";
        else
            std::cout << "No  ";

        // Vehicle charging
        if(it->second->isCharging)
            std::cout << "Yes ";
        else
            std::cout << "No  ";
        
        // Last SOC inc
        std::cout << std::setw(5) << std::right << std::setprecision(2) << it->second->getSOCchange() << " ";

        // Charge rate
        std::cout << std::setw(4) << std::right << std::setprecision(0) << it->second->chargeRate << " ";
     
        // L
        //std::cout << std::setw(4) << std::right << std::setprecision(2) << it->second->L << " ";
        // N
        //std::cout << std::setw(4) << std::right << std::setprecision(2) << it->second->N << " ";
        // P
        //std::cout << std::setw(4) << std::right << std::setprecision(2) << it->second->P << " ";
        
        // Switch on?
        if(it->second->switchon)
            std::cout << "* ";
        else
            std::cout << "  ";
        
        // SOC
        std::cout << std::setw(5) << std::right << std::setprecision(1) << it->second->getSOC() << std::endl;
    }
}

void GridModel::displayFullSummary(DateTime currTime) {
    std::cout << "Household demand:" << std::endl;
    for(std::map<std::string,Household*>::iterator it = households.begin(); it != households.end(); ++it)
        std::cout<< "   House " << std::setw(2) << std::right << it->second->NMI << ": " 
                 << std::setw(7) << std::right << std::setiosflags(std::ios::fixed) << std::setprecision(2) 
                 << it->second->demandProfile.demand[currTime.totalMinutes()].P << " W" << std::endl;

    std::cout << "Vehicle demand:" << std::endl;
    for(std::map<std::string,Vehicle*>::iterator it = vehicles.begin(); it != vehicles.end(); ++it)
        std::cout<< " Vehicle " << std::setw(2) << std::right << it->second->NMI << ": " 
                 << std::setw(7) << std::right << std::setiosflags(std::ios::fixed) << std::setprecision(2) 
                 << it->second->activePower << " W" << std::endl;
}

void GridModel::displayLoadSummary(DateTime currTime) {
    double hhSum=0, hhPF=0, hhMin=100000, hhMax=0;
    double evSum=0, evPF=0, evMin=100000, evMax=0;
    int evHome=0, evCharging=0;
    double currPower;
    

    for(std::map<std::string,Vehicle*>::iterator it = vehicles.begin(); it != vehicles.end(); ++it) {
        if(it->second->location == Home) {
            evHome++;
            if(it->second->isCharging) 
                evCharging++;
            
            currPower = it->second->activePower;

            evPF += it->second->getPowerFactor();
            evSum += currPower;
            if(currPower < evMin)
                evMin = currPower;
            if(currPower > evMax)
                evMax = currPower;
        }
    }
    for(std::map<std::string,Household*>::iterator it = households.begin(); it != households.end(); ++it) {
        currPower = it->second->demandProfile.demand[currTime.totalMinutes()].P;
        
        hhPF += it->second->getPowerFactor(currTime);
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


void GridModel::runValleyLoadFlow(DateTime datetime, HouseholdDemandModel householdDemand) {
    boost::posix_time::ptime timer;
    
    utility::startTimer(timer);
    std::cout << " - Running valley load flow analysis ... ";
    std::cout.flush();
    
    // Choose valley time
    DateTime valleytime = datetime;
    valleytime.hour = 4;
    valleytime.minute = 0;

    // Set all household loads to valley load, vehicle loads to zero
    for(std::map<std::string,Household*>::iterator it = households.begin(); it != households.end(); ++it)
        loadflow->setDemand(it->second->componentName, it->second->getActivePower(valleytime), it->second->getInductivePower(valleytime), it->second->getCapacitivePower(valleytime));

    for(std::map<std::string,Vehicle*>::iterator it = vehicles.begin(); it != vehicles.end(); ++it)
        loadflow->setDemand(it->second->componentName, 0, 0, 0);
    
    loadflow->runSim();
    
    loadflow->getOutputs(networkData.phaseV, networkData.phaseI, networkData.eolV, households);
    for(std::map<std::string,Household*>::iterator it = households.begin(); it != households.end(); ++it)
        it->second->V_valley = it->second->V_RMS;

    std::cout << "complete, took: " << utility::endTimer(timer) << std::endl;
}


void GridModel::runLoadFlow(DateTime currTime) {
    boost::posix_time::ptime timerFull, timer;
    
    utility::startTimer(timerFull);
    utility::startTimer(timer);
    
    std::cout << "Running load flow analysis ... " << std::endl;
    
    std::cout << " - setting household loads ...";
    std::ofstream outfile("tempHHloads.txt");
    for(std::map<std::string,Household*>::iterator it = households.begin(); it != households.end(); ++it)
        outfile << it->second->componentName << ", "
                << it->second->getActivePower(currTime)+0.001 << ", "
                << it->second->getInductivePower(currTime) << ", "
                << it->second->getCapacitivePower(currTime) << std::endl;
    outfile.close();
    loadflow->setDemand("tempHHloads.txt");
    std::cout << " OK (took " << utility::updateTimer(timer) << ")" << std::endl;
    
    std::cout << " - setting vehicle loads ...";
    outfile.open("tempVHloads.txt");
    for(std::map<std::string,Vehicle*>::iterator it = vehicles.begin(); it != vehicles.end(); ++it)
        outfile << it->second->componentName << ", "
                << it->second->activePower+0.001 << ", "
                << it->second->inductivePower << ", "
                << it->second->capacitivePower << std::endl;
    outfile.close();
    loadflow->setDemand("tempVHloads.txt");
    std::cout << " OK (took " << utility::updateTimer(timer) << ")" << std::endl;
    
    std::cout << " - running load flow calculation ...";
    std::cout.flush();
    loadflow->runSim();
    std::cout << " OK (took " << utility::updateTimer(timer) << ")" << std::endl;
    
    std::cout << " - getting output ...";
    loadflow->getOutputs(networkData.phaseV, networkData.phaseI, networkData.eolV, households);
    std::cout << " OK (took " << utility::updateTimer(timer) << ")" << std::endl;

    //std::cout << " - calculating voltage unbalance ..." << std::endl;
    //calculateVoltageUnbalance(currTime);
    //std::cout << " OK (took " << utility::updateTimer(timer) << ")" << std::endl;

    std::cout << "Load flow analysis complete, took: " << utility::endTimer(timerFull) << std::endl;
}


double GridModel::getAvailableCapacity() {
    return transformer->capacity - sumHouseholdLoads;
}

Household* GridModel::findHousehold(int NMI) {
    return householdNMImap[NMI];
}

Vehicle* GridModel::findVehicle(int NMI) {
    return vehicleNMImap[NMI];
}

// recalculate individual phase voltages at each pole
void GridModel::calculateVoltageUnbalance(DateTime currTime) {
    Phasor current[3];
    current[0].set(0,0);
    current[1].set(0,-120);
    current[2].set(0,120);
    
    calculatePoleCurrents(root, current, currTime);
    std::cout << "At root, currents are: " << std::endl;
    for(int i=0; i<3; i++)
        std::cout << " " << i+1 << ": " << current[i].toString() << "\n";
    
    calculatePoleVoltages(root);
    std::cout << "At root, voltages are: " << std::endl;
    for(int i=0; i<3; i++)
        std::cout << " " << i+1 << ": " << root->voltage[i].toString() << "\n";
    
}

// Calculate current at each pole
void GridModel::calculatePoleCurrents(FeederPole* pole, Phasor I[], DateTime currTime) {
    // Recursively iterate through to leaves
    if(pole->childLineSegments.size() > 0)
        for(std::vector<FeederLineSegment*>::iterator it=pole->childLineSegments.begin(); it!=pole->childLineSegments.end(); ++it)
            if((*it)->childPole != NULL)
                calculatePoleCurrents((*it)->childPole, I, currTime);
    
    // Add current from each house and vehicle
    Household* currHouse;
    int currPhase;
    Phasor currI;
    
    if(pole->households.size() > 0)
        for(std::vector<Household*>::iterator it=pole->households.begin(); it!=pole->households.end(); ++it) {

            currHouse = *it;
            currPhase = (int)(currHouse->phase);
            currI.setRC(currHouse->getDemandAt(currTime).P / baseVoltage, currHouse->getDemandAt(currTime).Q / baseVoltage);
            if(currHouse->hasCar)
                currI.addReal(findVehicle(currHouse->NMI)->chargeRate / baseVoltage);
            if(currPhase == 1)
                currI.addPhase(120);
            else if(currPhase == 2)
                currI.addPhase(240);

            I[currPhase] = I[currPhase].plus(currI);
        }
    
    for(int i=0; i<3; i++)
        pole->current[i] = I[i];
}

// Calculate current at each pole
void GridModel::calculatePoleVoltages(FeederPole* pole) {
    Phasor txVoltage[3];
    txVoltage[0].set(transformer->voltageOut*sqrt(2.0), 0);
    txVoltage[1].set(transformer->voltageOut*sqrt(2.0), -120);
    txVoltage[2].set(transformer->voltageOut*sqrt(2.0), 120);
    
    std::cout << pole->name << ":";
    for(int i=0; i<3; i++) {
        pole->voltage[i] = txVoltage[i].minus(pole->current[i].times(pole->totalImpedanceToTX));
        std::cout << "\n  " << i << "        I: " << pole->current[i].toString() 
                  << "\n           Z: " << pole->totalImpedanceToTX.toString()
                  << "\n           V: " << pole->voltage[i].toString() << std::endl;
    }
    
    Phasor a, a2;
    a.set(1,120);
    a2.set(1,240);
    
    Phasor vAB = pole->voltage[1].minus(pole->voltage[0]);
    Phasor vBC = pole->voltage[2].minus(pole->voltage[1]);
    Phasor vCA = pole->voltage[0].minus(pole->voltage[2]);
    
    Phasor vMinus = vAB.plus(vBC.times(a2).plus(vCA.times(a)));
    Phasor vPlus = vAB.plus(vBC.times(a).plus(vCA.times(a2)));
    
    std::cout << "\n------------------------"
              << "\n         Vab: " << vAB.toString()
              << "\n         Vbc: " << vBC.toString()
              << "\n         Vca: " << vCA.toString()
              << "\n       Vca*a: " << vCA.times(a).toString()
              << "\n      Vbc*a2: " << vBC.times(a2).toString()
              << "\nVbc*a2+Vca*a: " << (vBC.times(a2).plus(vCA.times(a))).toString()
              << "\n          V-: " << vMinus.toString()
              << "\n          V+: " << vPlus.toString()
              << "\n       V-/V+: " << vMinus.dividedBy(vPlus).toString()
              << "\n           %: " << vMinus.getAmplitude() / vPlus.getAmplitude() * 100
              << "\n________________________________________________________\n"
              << std::endl;
              

    for(std::vector<FeederLineSegment*>::iterator it=pole->childLineSegments.begin(); it!=pole->childLineSegments.end(); ++it)
        if((*it)->childPole != NULL)
            calculatePoleVoltages((*it)->childPole);
}

// Recursive DFS traversal of network tree, keeping track of impedance for each house
void GridModel::calculateHouseholdZ(FeederPole* pole, double r, double x) {
    pole->totalImpedanceToTX.resistance = r;
    pole->totalImpedanceToTX.reactance = x;
    
    // First, update all households connected to this pole (Note: ignore service line impedance)
    for(std::vector<Household*>::iterator it=pole->households.begin(); it!=pole->households.end(); ++it) {
        (*it)->totalImpedanceToTX.resistance = r + 2* (*it)->serviceLine.length * (*it)->serviceLine.resistance;
        (*it)->totalImpedanceToTX.reactance = x + 2* (*it)->serviceLine.length * ((*it)->serviceLine.inductance - (*it)->serviceLine.capacitance);
    }
    
    double tempR, tempX;
    // Second, for every child pole of this pole, do the same
    for(std::vector<FeederLineSegment*>::iterator it=pole->childLineSegments.begin(); it!=pole->childLineSegments.end(); ++it) {
        tempR = r + (*it)->line.length * (*it)->line.resistance;
        tempX = x + (*it)->line.length * ((*it)->line.inductance - (*it)->line.capacitance);
        if((*it)->childPole != NULL)
            calculateHouseholdZ((*it)->childPole, tempR, tempX);
    }
}

// Create mapping from households to NMIs (not names)
void GridModel::buildHouseholdNMImap() {
    householdNMImap.clear();
    for(std::map<std::string,Household*>::iterator it = households.begin(); it!=households.end(); ++it)
        householdNMImap[it->second->NMI] = it->second;
}

// Create mapping from vehicles to NMIs (not names)
void GridModel::buildVehicleNMImap() {
    vehicleNMImap.clear();
    for(std::map<std::string,Vehicle*>::iterator it = vehicles.begin(); it!=vehicles.end(); ++it)
        vehicleNMImap[it->second->NMI] = it->second;
}

