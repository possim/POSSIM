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

#ifndef LOADFLOWINTERFACE_H
#define	LOADFLOWINTERFACE_H


#include <stdio.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>

#include "../gridmodel/Vehicle.h"
#include "../gridmodel/Household.h"

class LoadFlowInterface {

public:
    virtual void loadModel(std::string model) = 0;
    virtual void runSim() = 0;
    virtual double getVar(std::string var) = 0;
    virtual void setVar(std::string component, double value, std::string var) = 0;
    virtual int getNumHouses() = 0;
    virtual std::vector <std::string> getHouseNames() = 0;
    virtual void addVehicle(Vehicle vehicle) = 0;
    virtual void setDemand(std::string component, double a, double i, double c) = 0;
    virtual void printModel(std::string targetDir) = 0;
    virtual void generateReport(std::string dir, int month, bool isWeekday, int simInterval) = 0;
    virtual void getOutputs(double phaseV[12], double phaseI[12], double eolV[12], std::map<int, Household> &households) = 0;
};

#endif	/* LOADFLOWINTERFACE_H */

