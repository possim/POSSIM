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
#include "../vehicle/TrafficModel.h"
#include "../spotprice/SpotPrice.h"
#include "../charging/ChargingUncontrolled.h"
#include "../charging/ChargingEqualShares.h"
#include "../charging/ChargingDistributed.h"
#include "../charging/ChargingTOU.h"
//#include "../charging/ChargingOptimal1.h"
//#include "../charging/ChargingOptimal2.h"
//#include "../charging/ChargingOptimal3.h"
//#include "../charging/ChargingDiscrete.h"
//#include "../charging/ChargingWplug.h"
//#include "../charging/ChargingWplug2.h"

/** The outer Simulator loop.
 * This class defines the main outer loop of the entire simulation.
 * It loads all components, starts the simulation, interrupts or delays the 
 * simulation as required, and ends the simulation, ensuring that all output is
 * logged.  If you want to understand how POSSIM works, start here.
 */
class Simulator {
private:
   
    /** The interface to whatever load flow software is being used.  This
     *  is a derived object and could point to any of a number of interfaces
     *  (matlab, digsilent, opendss, etc.). */
    LoadFlowInterface *loadflow;
    
    /** The interface to whatever charging algorithm is being used to charge
      * electric vehicles.  This is a derived object and could point to any
      * number of charging algorithms.*/
    ChargingBaseClass *charger;

    /** The model of the grid.  This contains all houses, vehicles, and 
      * special network components of any other sort. */
    GridModel gridModel;
    
    /** This will be moved to Household soon. */
    HouseholdDemandModel householdDemandModel;
    
    /** This will be moved to Vehicle soon */
    TrafficModel trafficModel;
    
    /** Maintains the changing electricity spot price using specifed data files */
    SpotPrice spotPrice;
    
    /** Takes care of all logging */
    Logging log;
    
    /** Time at which simulation is to start (inclusive) */
    DateTime startTime;
    
    /** Time of current simulation cycle */
    DateTime currTime;
    
    /** Time at which simulation is to finish (inclusive) */
    DateTime finishTime;
    
    /** Boolean specifying whether debug info should be displayed */
    bool showDebug;
    
    /** Type of update algorithm to use when applying charging rate (batch/grouped/individual) */
    std::string chargeRateUpdate;
    
    /** Number of vehicles to group together if using grouped charging algorithm updates */
    int chargeRateGroupSize;
    
    /** Order in which to apply updates (ordered/random) */
    std::string chargeRateOrder;
    
    /** Array of all vehicle IDs, to determine order of charging */
    std::vector<int> vehicleIDs;
    
    /** Local pointer to config object */
    Config *config;

    /** A simple timing update to give the user an indication of how much
      * longer the simulation will run for. */
    void timingUpdate(boost::posix_time::time_duration lastCycleLength);
    
public:
    /** Constructor.  
      * Accepts config object to set configuration parameters.*/
    Simulator(Config* config);
    
    /** Destructor*/
    virtual ~Simulator();
    
    /** Start the simulation! */
    void run();
    
};

#endif	/* SIMULATOR_H */

