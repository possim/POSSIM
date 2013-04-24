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

#ifndef SIMULATOR_H
#define	SIMULATOR_H

#include <boost/date_time/posix_time/posix_time.hpp>

#include "Config.h"
#include "Logging.h"
#include "../loadflow/MatlabInterface.h"
#include "../loadflow/TestingInterface.h"
#include "../utility/Utility.h"
#include "../utility/DateTime.h"
#include "../gridmodel/GridModel.h"
#include "../trafficmodel/TrafficModel.h"
#include "../spotprice/SpotPrice.h"
#include "../charging/ChargingUncontrolled.h"
#include "../charging/ChargingEqualShares.h"
#include "../charging/ChargingDistributed.h"
#include "../charging/ChargingTOU.h"


class Simulator {
private:
   
    LoadFlowInterface *loadflow;        // Interface to load flow software
    
    ChargingBaseClass *charger;         // EV charging algorithm

    Logging log;                        // For logging
    
    DateTime startTime;                 // Time at which simulation is to start (inclusive)
    DateTime currTime;                  // Time of current simulation cycle
    DateTime finishTime;                // Time at which simulation is to finish (inclusive)
    
    int simInterval;                    // Length of each interval in minutes
    int delay;                          // Optional delay for each simulation
    bool showDebug;                     // Optional show debug info
    bool generateReport;                // Optional generate graphs at end of run
    
    GridModel gridModel;                // Grid model
    HouseholdDemand householdDemand;    // Household demand model
    TrafficModel trafficModel;          // Traffic model
    SpotPrice spotPrice;                // Electricity spot price
    
    long lastIteration;   // How long last iteration took (for estimating time remaining)

    
    void timingUpdate();
    
public:
    Simulator(Config* config);
    virtual ~Simulator();
    
    void run();
    
};

#endif	/* SIMULATOR_H */

