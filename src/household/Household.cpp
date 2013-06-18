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

#include "Household.h"

Household::Household() {
    baseVoltage = 230;
    NMI = 0;
    name = "Noname";
    componentName = "Noname";
    phase = A;
    demandProfileFactor = 1;
    V_RMS = 0;
    V_Mag = 0;
    V_Pha = 0;
    V_valley = 0;
    V_unbalance = 0;
    hasParent = false;
    serviceLine.resistance = 0;
    serviceLine.inductance = 0;
    serviceLine.capacitance = 0;
    serviceLine.length = 0;
}

Household::~Household() {
}


S_Load Household::getDemandAt(DateTime datetime) {
    // First check if there is value for this exact time (TO DO)
    
    S_Load loadAtClosestTime;
    int closestTime = 100000;
    
    // Sometimes, a load profile is given in say 30 min intervals, but a 
    // simulation is running at say 5 min intervals.  In that case, choose
    // demand values in demand profile closest to current time.
    
    // Iterate through full demand profile saving value at closest time
    for(std::map<int, S_Load>::iterator iter=demandProfile.demand.begin(); iter!=demandProfile.demand.end(); ++iter) {
        if(abs(datetime.totalMinutes() - iter->first) < closestTime) {
            closestTime = abs(datetime.totalMinutes() - iter->first);
            loadAtClosestTime = iter->second;
        }
    }
   
    
    return loadAtClosestTime;
}

double Household::getActivePower(DateTime datetime) {
    return(demandProfile.demand[datetime.totalMinutes()].P);
}

double Household::getInductivePower(DateTime datetime) {
    // negative Q means capacitive load, return 0 (well close; matlab doesn't like 0's)
    if(demandProfile.demand[datetime.totalMinutes()].Q < 0)
        return 0.000001;
    
    // positive Q means inductive
    return(demandProfile.demand[datetime.totalMinutes()].Q);
}

double Household::getCapacitivePower(DateTime datetime) {
    // positive Q means inductive load, return 0 (well close; matlab doesn't like 0's)
    if(demandProfile.demand[datetime.totalMinutes()].Q > 0)
        return 0.000001;
    
    // negative Q means capacitive
    return(demandProfile.demand[datetime.totalMinutes()].Q);
}

// Return power factor
double Household::getPowerFactor(DateTime datetime) {
    return atan(demandProfile.demand[datetime.totalMinutes()].Q/demandProfile.demand[datetime.totalMinutes()].P);
}

