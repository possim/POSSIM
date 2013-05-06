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

#include "ChargingTOU.h"

ChargingTOU::ChargingTOU(Config* config, GridModel gridModel) :
ChargingBaseClass(config, gridModel){
    maxChargeRate = config->getDouble("maxchargerate");
    chargeStart = 23;
    chargeEnd = 6;
}

ChargingTOU::~ChargingTOU() {
}


void ChargingTOU::setChargeRates(DateTime datetime, GridModel &gridModel) {
    // Set chargeRates
    for(std::map<int,Vehicle>::iterator it = gridModel.vehicles.begin(); it != gridModel.vehicles.end(); ++it)
        if(it->second.getSOC() < 98  &&  it->second.isConnected && (datetime.hour >= chargeStart || datetime.hour <= chargeEnd)) {
            it->second.chargeRate = maxChargeRate;
            it->second.isCharging = true;
        }
        else {
            it->second.chargeRate = 0;
            it->second.isCharging = false;
        }
}
