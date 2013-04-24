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

#ifndef GRIDMODEL_H
#define	GRIDMODEL_H

#include <stdlib.h>
#include <algorithm>
#include <iostream>
#include <map>
#include <cmath>
#include <iomanip>
#include <string.h>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "../simulator/Config.h"
#include "HouseholdDemand.h"
#include "Household.h"
#include "Vehicle.h"
#include "../loadflow/LoadFlowInterface.h"
#include "../loadflow/MatlabInterface.h"
#include "../utility/Utility.h"


class GridModel {
private:
    LoadFlowInterface   *loadflow;
    
    int                 evPenetration;  // % of houses having an EV
    double              powerFactor;    // Power factor of household loads
    int                 minVoltage;     // Min voltage allowed by distribution code
    
    double              distTransCapacity;      // Nominal capacity of DistTrans, kVA
    double              availableCapacity;      // Capacity of DistTrans after household loads
    

public:

    std::map<int, Household>    households;
    std::map<int, Vehicle>      vehicles;
    

    double phaseV[12];  // Voltage RMS, Magnitude, Shift, for each phase and neutral, as measured at Tx
    double phaseI[12];  // Current RMS, Magnitude, Shift, for each phase and neutral, as measured at Tx
    double eolV[12];    // Voltage RMS, Magnitude, Shift, for each phase and neutral, as measured at end-of-line
    
    
public:
    GridModel();
    virtual ~GridModel();
    
    void initialise(Config* config, LoadFlowInterface* loadflow);
    
    void updateVehicleBatteries();
    
    void generateLoads(DateTime currTime, HouseholdDemand householdDemand);

    void displayVehicles();
    void displayFullSummary();
    void displayVehicleSummary();
    void displayLoadSummary();
    
    void runValleyLoadFlow(DateTime datetime, HouseholdDemand householdDemand);  
                                                // To determine voltage losses when no loads present
                                                // (Required e.g. for calibration in distributed charging algorithm)
    void runLoadFlow(DateTime currTime);
    
    double getAvailableCapacity();
    

private:
    void loadGridModel(LoadFlowInterface* loadflow);
    
    void addVehicles(Config* config);
    
};

#endif	/* GRIDMODEL_H */

