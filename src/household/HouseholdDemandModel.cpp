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


#include "HouseholdDemandModel.h"

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/file.hpp>

HouseholdDemandModel::HouseholdDemandModel(Config* config) {
    std::cout << "Loading household demand model ..." << std::endl;
    
    // Save several global variables locally
    simInterval = config->getInt("simulationinterval");
    modelType = config->getConfigVar("demandmodel");
    demandDataDir = config->getConfigVar("demanddatadir");
    houseProfileAllocFile = config->getConfigVar("housedemandalloc");
    randomDistribution = config->getRandomParams("demandrandom_int");
}

HouseholdDemandModel::~HouseholdDemandModel() {
}

HouseholdDemandProfile HouseholdDemandModel::inputProfileFromFile(std::string filename) {
    // Open data file     
    boost::iostreams::stream<boost::iostreams::file_source> infile(filename.c_str());
    if(!infile){
       std::cout << "Cannot open household demand data input file " << filename;
       exit (1);
    }

    HouseholdDemandProfile newProfile;
    
    try {
        std::string line;
        std::vector<std::string> tokens;
        DateTime currDatetime;
        S_Load currLoad;
        std::string::size_type start = filename.find_last_of("/\\")+1;
        std::string::size_type end   = filename.find_last_of('.');
        newProfile.name = filename.substr(start, end-start); 
        

        while(std::getline(infile,line)) {
            utility::tokenize(line,tokens,",\r");
            // First entry can be either in datetime format (look for periods)
            if(tokens.at(0).find_first_of('.') != std::string::npos)
                currDatetime.set(tokens.at(0));
            // or in minutes of day format (when there are no periods)
            else
                currDatetime.set(utility::string2int(tokens.at(0)));

            currLoad.P = utility::string2double(tokens.at(1));
            currLoad.Q = utility::string2double(tokens.at(2));
            //std::cout << currLoad.P << " " << currLoad.Q << std::endl;
            
            newProfile.demand[currDatetime.totalMinutes()] = currLoad;
        }

        infile.close();
    }
    catch(std::exception &e) {
        std::cout << "\nWARNING: Could not read demand profile info from file " << filename << "!" << std::endl;
    }
    return newProfile;
}

void HouseholdDemandModel::inputProfileAllocationFromFile() {
    // Open data file     
    std::ifstream infile(houseProfileAllocFile.c_str());
    if(!infile){
       std::cout << "Cannot open household allocation input file " << houseProfileAllocFile;
       exit (1);
    }

    std::string line;
    std::vector<std::string> tokens;
    allocationMap.clear();
    
    while(getline(infile,line)) {
       utility::tokenize(line,tokens,",");
       allocationMap[tokens.at(0)] = tokens.at(1);
    }
    
    infile.close();
}

void HouseholdDemandModel::inputAllProfiles() {
    std::vector<std::string> fileNames;
    HouseholdDemandProfile currProfile;
    int numDemandProfiles = 0;
    
    // Regardless of model type, input all profiles in given directory
    fileNames = utility::getAllFileNames(demandDataDir);
    for(int i=0; i<fileNames.size(); i++) {
        std::string::size_type pos1 = fileNames.at(i).find_last_of("/\\");
        // Ignore files starting with a period (e.g. .DS_Store)
        if(fileNames.at(i).at(pos1+1) != '.') {
            currProfile = inputProfileFromFile(fileNames.at(i));
            demandProfiles[currProfile.name] = currProfile;
            numDemandProfiles++;
        }
    }
    
    std::cout << " - found " << numDemandProfiles << " demand profile files" << std::endl;
}

HouseholdDemandProfile HouseholdDemandModel::getRandomProfile() {
    int index = utility::randomUniform(0, demandProfiles.size()-1);
    int i=0;
    for(std::map<std::string,HouseholdDemandProfile>::iterator it=demandProfiles.begin(); it!=demandProfiles.end(); ++it) {
        if(i == index)
            return(it->second);
        i++;
    }    
    // Should not reach this point.
    HouseholdDemandProfile nullProfile;
    return nullProfile;
}


void HouseholdDemandModel::assignProfiles(std::map<std::string, Household*> &households) {
    std::cout << " - Assigning demand profiles to houses ...";

    if(modelType == "generic")
        for(std::map<std::string,Household*>::iterator it=households.begin(); it!=households.end(); ++it)
            it->second->demandProfile = inputProfileFromFile(demandDataDir);

    else if(modelType == "random") {
        inputAllProfiles();
        for(std::map<std::string,Household*>::iterator it=households.begin(); it!=households.end(); ++it)
            it->second->demandProfile = getRandomProfile();
    }

    else if(modelType == "phasespecific") {
        inputAllProfiles();
        for(std::map<std::string,Household*>::iterator it=households.begin(); it!=households.end(); ++it) {
            Phase phase = it->second->phase;
            if(phase == A)      it->second->demandProfile = demandProfiles["phaseA"];
            else if(phase == B) it->second->demandProfile = demandProfiles["phaseB"];
            else                it->second->demandProfile = demandProfiles["phaseC"];
        }
    }

    else if(modelType == "housespecific") {
        inputAllProfiles();
        inputProfileAllocationFromFile();
        for(std::map<std::string,Household*>::iterator it=households.begin(); it!=households.end(); ++it) {
            std::string profileName = allocationMap[it->second->name];
            it->second->demandProfile = demandProfiles[profileName];
        }
    }

    std::cout << " OK" << std::endl;
}
