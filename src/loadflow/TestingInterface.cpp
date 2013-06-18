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



#include "TestingInterface.h"



/*   CONSTRUCTOR, DESTRUCTOR   */
    
TestingInterface::TestingInterface(Config* config) {
    std::cout << " - Using testing interface ... OK" << std::endl;
}

TestingInterface::~TestingInterface() {
}


void TestingInterface::loadModel(Config* config) {
}

void TestingInterface::extractModel(FeederPole* &root, 
                                    DistributionTransformer* &transformer,
                                    std::map<std::string,FeederPole*> &poles, 
                                    std::map<std::string,FeederLineSegment*> &lineSegments, 
                                    std::map<std::string,Household*> &households) {
}
    
void TestingInterface::runSim() {
}

double TestingInterface::getVar(std::string var) {
    return 2.0;
}

void TestingInterface::setVar(std::string component, double value, std::string var) {
}

void TestingInterface::setVar(std::string component, std::string value, std::string var){
}

void TestingInterface::setTxCapacity(std::string component, double value) {
}

int TestingInterface::getNumHouses() {
    return 12;
}

std::vector <std::string> TestingInterface::getHouseNames() {
    std::vector<std::string> houseNames;
    std::string curr;
    for(int i=0; i<12; i++) {
        curr = "test/HH_1_" + utility::int2string(i+1);
        houseNames.push_back(curr);
    }
    return houseNames;
}

void TestingInterface::addVehicle(Vehicle vehicle) {
}

void TestingInterface::setDemand(std::string component, double a, double i, double c) {
}

void TestingInterface::setDemand(std::string filename) {
}

void TestingInterface::printModel(std::string targetDir) {
}

void TestingInterface::generateReport(std::string dir, int month, bool isWeekday, int simInterval) {
    
}

void TestingInterface::getOutputs(std::string logDir,
                                NetworkData &networkData, 
                                std::map<std::string,Household*> &households, 
                                std::map<std::string,FeederLineSegment*> &lineSegments,
                                std::map<std::string,FeederPole*> &poles) {
    
}
