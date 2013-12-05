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

void Household::setModelType(std::string mt) {
    modelType = mt;
}

S_Load Household::generateDemandAtTimeOfDay(DateTime datetime) {
    // This function is used by "generic" demand model -- i.e. date not important
    
    
    S_Load demandNow;
    
    // Find the closest demand entry in demandProfile.  This may be
    // exact, but in certain situations may be slightly later (e.g. if demand data is
    // in 30-min intervals, but we are running sim in 5-min intervals)
    for(std::map<DateTime, S_Load>::iterator iter=demandProfile.demand.begin(); iter!=demandProfile.demand.end(); ++iter) {
        if(datetime.timeEquals(iter->first) || datetime.isEarlierInDayThan(iter->first)) {
            demandNow = iter->second;
            break;
        }
    }
    
    // TODO:  Ensure this function is safer!
    
    // Load for closest time to now has been found, update local values
    activePower = demandNow.P;
    if(demandNow.Q < 0) {
        inductivePower = 0.000001;
        capacitivePower = demandNow.Q;
    }
    else {
        inductivePower = demandNow.Q;
        capacitivePower = 0.000001;
    }
   
    
    return demandNow;
}

S_Load Household::generateDemandAtExactTime(DateTime datetime) {
    S_Load demandNow;
    
    // Find the closest demand entry in demandProfile.  This may be
    // exact, but in certain situations may be slightly later (e.g. if demand data is
    // in 30-min intervals, but we are running sim in 5-min intervals)
    for(std::map<DateTime, S_Load>::iterator iter=demandProfile.demand.begin(); iter!=demandProfile.demand.end(); ++iter) {
        if(datetime.equals(iter->first) || datetime.isEarlierThan(iter->first)) {
            demandNow = iter->second;
            break;
        }
    }
    
    // TODO:  Ensure this function is safer!
    
    // Load for closest time to now has been found, update local values
    activePower = demandNow.P;
    if(demandNow.Q < 0) {
        inductivePower = 0.000001;
        capacitivePower = demandNow.Q;
    }
    else {
        inductivePower = demandNow.Q;
        capacitivePower = 0.000001;
    }
   
    
    return demandNow;
}

S_Load Household::generateDemandAt(DateTime datetime) {
    if(modelType == "generic")
        return generateDemandAtTimeOfDay(datetime);
    else
        return generateDemandAtExactTime(datetime);
}

// Return power factor
double Household::getPowerFactor(DateTime datetime) {
    return atan((inductivePower+capacitivePower)/activePower);
}

