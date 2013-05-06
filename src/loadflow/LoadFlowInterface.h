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

/** Base class for interaction with third party load flow / network modelling
  * software.  One goal of POSSIM is to allow a variety of load flow packages
  * to be plugged in and out as desired.  Each load flow package is accessed
  * via a "LoadFlowInterface" specific to that package.  Each such interface
  * must inherit this base class. */
class LoadFlowInterface {

public:
    /** Load network model. */
    virtual void loadModel(std::string model) = 0;
    
    /** Run a load flow simulation. */
    virtual void runSim() = 0;
    
    /** Get value of variable having this name. */
    virtual double getVar(std::string var) = 0;
    
    /** Set value of variable having this name. */
    virtual void setVar(std::string component, double value, std::string var) = 0;
    
    /** Get number of houses in the model. */
    virtual int getNumHouses() = 0;
    
    /** Get names of houses in the model. */
    virtual std::vector <std::string> getHouseNames() = 0;
    
    /** Add a vehicle to the model. */
    virtual void addVehicle(Vehicle vehicle) = 0;
    
    /** Set demand of the given component. */
    virtual void setDemand(std::string component, double a, double i, double c) = 0;
    
    /** Print the model (to pdf in this run's logging directory, ideally). */
    virtual void printModel(std::string targetDir) = 0;
    
    /** Generate a report. */
    virtual void generateReport(std::string dir, int month, bool isWeekday, int simInterval) = 0;
    
    /** Get load flow simulator's outputs, after load flow conducted. */
    virtual void getOutputs(double phaseV[12], double phaseI[12], double eolV[12], std::map<int, Household> &households) = 0;
};

#endif	/* LOADFLOWINTERFACE_H */

