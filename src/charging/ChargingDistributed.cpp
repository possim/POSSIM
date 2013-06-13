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

#include "ChargingDistributed.h"

ChargingDistributed::ChargingDistributed(Config* config, GridModel gridModel) :
    ChargingBaseClass(config, gridModel){
    maxChargeRate = config->getDouble("maxchargerate");
}

ChargingDistributed::~ChargingDistributed() {
}

ChargingDistributed::ChargingDistributed(Config* config, GridModel &gridModel, DateTime datetime, HouseholdDemandModel hhDemand) :
        ChargingBaseClass(config, gridModel) 
{
    minVoltage = config->getInt("minvoltage");
    simInterval = config->getInt("simulationinterval");
    periodOfLoadSchedule = config->getInt("loadscheduleperiod");
    
    // To properly initialise, run a "valley load flow".  In other words, choose
    // a valley load time (say 4:30 am), assign typical household loads, and store
    // voltages at each house.  In other words, calibrate the system.  We now 
    // know that each house can safely schedule additional load when its voltage
    // is at or near this "valley" level.
    gridModel.runValleyLoadFlow(datetime, hhDemand);

    std::cout << " - Distributed charging algorithm init complete!" << std::endl;   
}


/* For more detail on how this algorithm works, see:
 * J. de Hoog, D. A. Thomas, V. Muenzel, D. C. Jayasuriya, T. Alpcan, M. Brazil, and I. Mareels,
 * "Electric Vehicle Charging and Grid Constraints: Comparing Distributed and Centralized Approaches",
 * In Proceedings of the IEEE Power and Energy Society General Meeting. Vancouver, Canada, July 2013. */
void ChargingDistributed::setChargeRates(DateTime datetime, GridModel &gridModel) {
    double L, N, P, P_applied;
    std::string currName;
    double currV, valleyV, currSOC;
    
    for(std::map<std::string,Vehicle*>::iterator it = gridModel.vehicles.begin(); it != gridModel.vehicles.end(); ++it) {
        currName = it->second->getName();
        currSOC = it->second->getSOC();
        it->second->switchon = false;
        currV = gridModel.households.at(currName)->V_RMS;
        valleyV = gridModel.households.at(currName)->V_valley;

        // Calculate L (voltage probability)
        if(currV > valleyV)
            L = 1;
        else if(currV < minVoltage)
            L = -1;
        else
            L = (currV - minVoltage) / (valleyV - minVoltage);
        it->second->L = L;
        
        // Calculate N (SOC probability)
        if(currSOC < 20)
            N = 1;
        else
            N = (100 - currSOC) / 80;
        it->second->N = N;
        
        P = std::max(0.0, ((L + N) / 2));
        it->second->P = P;
        
        P_applied = 1 - pow((1-P),(double(simInterval)/double(periodOfLoadSchedule)));
        std::cout << "Probability of charging " << it->second->getNMI() << ": " << P << " (" << P_applied << ")" << std::endl;
        
        
        // Check if this vehicle is connected, if not disregard.
        if(!it->second->isConnected) {
            it->second->isCharging = false;
            it->second->chargeRate = 0;
            continue;
        }
        
        // Check if this vehicle is fully charged, if so disregard.
        if(currSOC > 98) {
            it->second->isCharging = false;
            it->second->chargeRate = 0;
            continue;
        }
        
        // Check if this vehicle is already charging
        if(it->second->isCharging) {
            continue;
        }

        // If we reach this point, vehicle is connected, needs charge, but is not yet charging

        //std::cout << std::setw(3) << std::right << std::setiosflags(std::ios::fixed) << currV << " " 
         //         << std::setw(3) << std::right << std::setiosflags(std::ios::fixed) << valleyV << " " 
         //         << std::setw(3) << std::right << std::setiosflags(std::ios::fixed) << currSOC << " " 
         //         << std::setw(3) << std::right << std::setiosflags(std::ios::fixed) << L << " + " 
         //         << std::setw(3) << std::right << std::setiosflags(std::ios::fixed) << N << " = " 
         //         << std::setw(3) << std::right << std::setiosflags(std::ios::fixed) << P;
        
        if(utility::randomUniform(0,1) < P_applied) {
            it->second->chargeRate = maxChargeRate;
            it->second->isCharging = true;
            it->second->switchon = true;
            //std::cout << " Yes, started charging " << it->second.isCharging;
        }
        else {
            it->second->chargeRate = 0;  
            it->second->isCharging = false;
            //std::cout << " No, not yet " << it->second.isCharging;
        }
        //std::cout << std::endl;
    }
}
