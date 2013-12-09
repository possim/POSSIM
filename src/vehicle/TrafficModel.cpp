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

#include "TrafficModel.h"


TrafficModel::TrafficModel(Config* config) {
    std::cout << "Loading traffic model data ...";
    std::cout.flush();
    
    std::string inputFile = config->getString("trafficdata");
        
    // Open data file     
    std::ifstream infile(inputFile.c_str());
    if(!infile){
       std::cout << "Cannot open vehicle profile input file " << inputFile;
       exit (1);
    }

    std::string line;
    std::vector<std::string> tokens;
    vehicleRecord_t vehicleRecord;
    travelPair_t travelPair;
    int numWeekend = 0, numWeekday = 0;
    
    // Get correct record
    while(getline(infile, line)) {
        tokens.clear();
        utility::tokenize(line, tokens, ",");

        vehicleRecord.name = tokens[0];
        vehicleRecord.travelPairs.clear();
        for(size_t i=2; i<tokens.size(); i+=2) {
            travelPair.time.setTime(utility::string2int(tokens[i]));
            travelPair.distance = utility::string2double(tokens[i+1]);
            vehicleRecord.travelPairs.push_back(travelPair);
        }
        
        if(tokens[1] == "Saturday" || tokens[1] == "Sunday") {
            vehicleRecord.isWeekday = false;
            numWeekend++;
        }
        else {
            vehicleRecord.isWeekday = true;
            numWeekday++;
        }
        
        travelRecords.push_back(vehicleRecord);
    }
    
    infile.close();
    
    std::cout << std::endl << " - found " << travelRecords.size() 
              << " vehicle records (" 
              << numWeekday << " weekday, " 
              << numWeekend << " weekend) ... "
              << std::endl;

    home2away = 0;
    away2home = 0;
    
    srand((unsigned int)(time(NULL)));
    
    // Store model type and path to file
    modelType = config->getString("trafficmodel");
    file_travelProfiles = config->getString("trafficmodelfile");
    std::cout << " - using " << modelType << " traffic model ..." << std::endl;
    
    // Little check on modeltype
    if(modelType != "random" && modelType != "ordered" && modelType != "vehiclespecific") {
        std::cout << " - WARNING: modelType (" << modelType << ") does not correspond to a supported model. "
                  << "Using random travel profile assignment instead." << std::endl;
        modelType = "random";
    }

    // If using ordered list of vehicle profiles, read them in
    if(modelType == "ordered") {
        std::cout << " - reading in ordered travel profile list ..." << std::endl;
        // Open data file     
        std::ifstream infile(file_travelProfiles.c_str());
        if(!infile){
            std::cout << "Cannot open ordered vehicle profile input file " << file_travelProfiles << "!";
            exit (1);
        }

        // File should have simple list of preferred vehicle profiles, store in list
        while(std::getline(infile,line)) {
            tokens.clear();
            utility::tokenize(line, tokens, "\r\n");
            vehicleRecord.name = tokens[0];
            travelRecordList.push_back(vehicleRecord.name);
        }

        infile.close();
    }
    
    std::cout << " - Traffic model loaded OK" << std::endl;
    std::cout.flush();
}


TrafficModel::~TrafficModel() {
}

// Assign a random vehicle record to each vehicle
void TrafficModel::assignVehicleRecords(DateTime datetime, std::map<std::string,Vehicle*> &vehicles) {
    std::cout << " - A new day: loading new travel profiles ...";
    std::cout.flush();
    

    // If using ordered list of vehicle profiles
    if(modelType == "ordered") {
        int counter = 0;
        std::string thisRecordName;

        for(std::map<std::string,Vehicle*>::iterator it = vehicles.begin(); it != vehicles.end(); ++it) {
            thisRecordName = travelRecordList.at(counter);

            for(std::vector<vehicleRecord_t>::iterator it2 = travelRecords.begin(); it2 != travelRecords.end(); ++it2)
                if(it2->name == thisRecordName) {
                    it->second->travelProfile = *it2;
                    break;
                }

            counter++;

            // Cycle through ordered list if smaller than num vehicles
            if(counter >= travelRecordList.size()) counter = 0;
        }

    }
        
    // In all other cases, pick random record
    else {
        int randomIndex;
        
        for(std::map<std::string,Vehicle*>::iterator it = vehicles.begin(); it != vehicles.end(); ++it) {

            // Generate random number until either weekend or weekday (as desired) is found
            // (WARNING:  could loop endlessly, should fix in future)
            randomIndex = rand()%travelRecords.size();
            while(travelRecords.at(randomIndex).isWeekday != datetime.isWeekday())
                randomIndex = rand()%travelRecords.size();

            it->second->travelProfile = travelRecords.at(randomIndex); 
        }
    }
    
    
    std::cout << " OK" << std::endl;
    std::cout.flush();
}

// Assign vehicle records, and set each vehicle to its initial state.  If a
// simulation starts at a non-midnight time, this means that earlier entries
// need to be cycled through until correct state at correct time is found.
void TrafficModel::initialise(DateTime datetime, std::map<std::string,Vehicle*> &vehicles) {
    travelPair_t nextTravelPair;
    bool endOfTravelPairs;
    
    assignVehicleRecords(datetime, vehicles);
    
    for(std::map<std::string,Vehicle*>::iterator it = vehicles.begin(); it != vehicles.end(); ++it) {
        it->second->distanceDriven = 0;
        
        // Skip if there is no transition
        if(it->second->travelProfile.travelPairs.size() == 0)
            continue;
        
        nextTravelPair = it->second->travelProfile.travelPairs.front();
        
        endOfTravelPairs = false;
        
        // If vehicle has made transition(s)
        while(datetime.isLaterInDayThan(nextTravelPair.time) && !endOfTravelPairs) {
            it->second->travelProfile.travelPairs.pop_front();
            if(it->second->location == Home) {
                it->second->location = Away;
                it->second->isConnected = false;
            }
            else {
                it->second->location = Home;
                it->second->isConnected = true;
                it->second->setChargeStart(datetime);
            }
            
            if(it->second->travelProfile.travelPairs.size() > 0)
                nextTravelPair = it->second->travelProfile.travelPairs.front();
            else
                endOfTravelPairs = true;
        }
    }
}

// For all vehicles, check if there has been a change to their status over the
// past interval.  If yes, update.   At midnight, generate new profiles.
void TrafficModel::update(DateTime datetime, std::map<std::string,Vehicle*> &vehicles) {
    home2away = 0;
    away2home = 0;
    travelPair_t nextTravelPair;
    
    std::cout << " - Updating traffic model ...\n";
    
    // At midnight, generate new set of random profiles
    if(datetime.isMidnight())
        assignVehicleRecords(datetime, vehicles);
    
    
    for(std::map<std::string,Vehicle*>::iterator it = vehicles.begin(); it != vehicles.end(); ++it) {
        it->second->distanceDriven = 0;
        
        // Skip if there is no remaining transition
        if(it->second->travelProfile.travelPairs.size() == 0)
            continue;
        
        nextTravelPair = it->second->travelProfile.travelPairs.front();
        
        // If vehicle has made transition(s)
        if(datetime.isLaterInDayThan(nextTravelPair.time)) {
            it->second->travelProfile.travelPairs.pop_front();
            if(it->second->location == Home) {
                home2away++;
                it->second->location = Away;
                it->second->isConnected = false;
                it->second->isCharging = false;
            }
            else {
                away2home++;
                it->second->location = Home;
                it->second->distanceDriven = nextTravelPair.distance;
                it->second->setChargeStart(datetime);
                it->second->isConnected = true;
                
                // Optional:  Prevent vehicles from plugging in if they are only going to be home for a short time
                /*if(it->second->travelProfile.travelPairs.size() > 0 &&
                (it->second->travelProfile.travelPairs.front().time.diffInMinutes(datetime) < 120)){
                    it->second->isConnected = false;
                    it->second->isCharging = false;
                }*/

            }
        }
    }
    //std::cout << " OK" << std::endl;
}

void TrafficModel::displaySummary(std::map<std::string,Vehicle*> vehicles) {
    int numHome=0, numConnected=0;
    
    for(std::map<std::string,Vehicle*>::iterator it = vehicles.begin(); it != vehicles.end(); ++it) {
        if(it->second->location == Home) {
            numHome++;
            if(it->second->isConnected)
                numConnected++;
        }
    }
    
    std::cout << "Traffic Model Summary" << std::endl
              << "  Total vehicles: " << std::setw(4) << std::right << std::setiosflags(std::ios::fixed) << vehicles.size() << std::endl
              << "  Departed      : " << std::setw(4) << std::right << std::setiosflags(std::ios::fixed) << home2away << std::endl
              << "  Arrived       : " << std::setw(4) << std::right << std::setiosflags(std::ios::fixed) << away2home << std::endl
              << "  Now at home   : " << std::setw(4) << std::right << std::setiosflags(std::ios::fixed) << numHome << std::endl
              << "  Now connected : " << std::setw(4) << std::right << std::setiosflags(std::ios::fixed) << numConnected << std::endl;
}

