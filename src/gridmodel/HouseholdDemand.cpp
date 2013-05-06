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


#include "HouseholdDemand.h"


HouseholdDemand::HouseholdDemand(Config* config) {
    std::cout << "Loading household demand model... ";
    std::cout.flush();
    
    simInterval = config->getInt("simulationinterval");
    modelType = config->getConfigVar("demandmodel");
    demandDataFile = config->getConfigVar("demanddata");
    phaseAllocFile = config->getConfigVar("phaseallocation");
    randomnessVars = config->getRandomParams("demandrandom_int");
    
    // Use generic profile
    if(modelType == "genericprofile") {
        for(int month=1; month<=12; month++) {
            inputWeekdayData(month);
            inputWeekendData(month);
        }
    }
    
    else if(modelType == "readfromfile") {
        inputPhaseAllocation();

        std::ifstream infile(demandDataFile.c_str());
        if(!infile){
            std::cout << "Cannot open demand data input file: " << demandDataFile << std::endl;
            exit (1);
        }

        std::string line;
        std::vector<std::string> tokens;
        double V[3], I[3];
        Z_Triple zTriple;

        getline(infile,line);
        while(getline(infile,line)) {
            utility::tokenize(line,tokens,",");
            V[0] = utility::string2double(tokens.at(1));
            I[0] = utility::string2double(tokens.at(2));
            V[1] = utility::string2double(tokens.at(4));
            I[1] = utility::string2double(tokens.at(5));
            V[2] = utility::string2double(tokens.at(7));
            I[2] = utility::string2double(tokens.at(8));

            for(int i=0; i<=2; i++)
                zTriple.Z[i] = V[i]/I[i] - numHouses[i]/totalHouses;

            exactDemand.insert(std::pair<std::string, Z_Triple>(tokens.at(0), zTriple));
        }
    }

    else {
        std::cout << "ERROR!  Could not identify household demand model type!" << std::endl;
        exit(1);
    }

    std::cout << "OK" << std::endl;
}


HouseholdDemand::~HouseholdDemand() {
}


double HouseholdDemand::getDemandAt(DateTime datetime, int nmi) {
    double newDemand;
    
    if(modelType == "genericprofile") {
        double newDemand;

        if(datetime.isWeekday())
            newDemand = genericDemand[datetime.month-1][0].at(datetime.totalMinutes()/simInterval);
        else
            newDemand = genericDemand[datetime.month-1][1].at(datetime.totalMinutes()/simInterval);
    }
    
    else if(modelType == "readfromfile") {
        int phaseNum = housePhase.at(nmi) - 1;
        Z_Triple zTriple = exactDemand.at(datetime.formattedString());
        newDemand = (230 * 230/zTriple.Z[phaseNum]) / numHouses[phaseNum]; 
    }
    
    else {
        std::cout << "ERROR!  Could not identify household demand model type!" << std::endl;
        exit(1);
    }

    newDemand += utility::randomNormal(randomnessVars);

    if(newDemand < 0)
        newDemand = 0;

    return newDemand;    
}

void HouseholdDemand::inputData(std::string filename, int month, int weekday) {
    // Open data file     
    std::ifstream infile(filename.c_str());
    if(!infile){
       std::cout << "Cannot open household data input file " << filename;
       exit (1);
    }

    std::string line;
    std::vector<std::string> tokens;

    for(int i=0; i<(1440/simInterval); i++) {
       infile >> line;
       tokens.clear();
       utility::tokenize(line,tokens,",");
       genericDemand[month-1][weekday].push_back(atof(tokens.at(1).c_str()));
    }
    infile.close();
}

void HouseholdDemand::inputWeekdayData(int month) {
    // Open data file
    std::stringstream ss;
    ss << demandDataFile << utility::time2string(month) << "_weekday.csv";      
    inputData(ss.str(), month, 0);
}

void HouseholdDemand::inputWeekendData(int month) {
    // Open data file
    std::stringstream ss;
    ss << "data/demand/2011_" << utility::time2string(month) << "_weekend.csv";      
    inputData(ss.str(), month, 1);
}

void HouseholdDemand::inputPhaseAllocation() {
    // Open data file     
    std::ifstream infile(phaseAllocFile.c_str());
    if(!infile){
       std::cout << "Cannot open phase allocation input file " << phaseAllocFile;
       exit (1);
    }

    std::string line;
    std::vector<std::string> tokens;
    int houseNum, phaseNum;
    numHouses[0] = 0;
    numHouses[1] = 0;
    numHouses[2] = 0;

    while(getline(infile, line)) {
       utility::tokenize(line,tokens,",");
       houseNum = utility::string2int(tokens.at(0));
       phaseNum = utility::string2int(tokens.at(1));
       housePhase.insert(std::pair<int,int>(houseNum, phaseNum));
       numHouses[phaseNum-1]++;
       totalHouses++;
    }
    infile.close();
}