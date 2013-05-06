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
#include "../loadflow/LoadFlowInterface.h"
#include "../loadflow/MatlabInterface.h"
#include "../utility/Utility.h"
#include "HouseholdDemand.h"
#include "Household.h"
#include "Vehicle.h"

/** The full grid model.  This class contains all houses, vehicles, and any
  * further components as required, as well as the network structure, network
  * specs, etc.  It interacts with the load flow interface, sending inputs to 
  * it and reading outputs from it.*/
class GridModel {
private:
    
    /** Local copy of pointer to load flow interface. */
    LoadFlowInterface   *loadflow;
    
    /** Percentage of houses having an EV. */
    int                 evPenetration;
    
    /** Average power factor of household loads (local copy of config variable). */
    double              powerFactor;
    
    /** Minimum voltage allowed by distribution code (local copy of config variable). */
    int                 minVoltage;
    
    /** Nominal capacity of distribution transformer, kVA. */
    double              distTransCapacity;
    
    /** Total household loads at current point in time. */
    double              sumHouseholdLoads;
    

public:

    /** Map of Households to their ID numbers. */
    std::map<int, Household>    households;
    
    /** Map of Vehicles to the ID numbers of houses they are associated with. */
    std::map<int, Vehicle>      vehicles;

    /** Voltage RMS, Magnitude, Shift, for each phase and neutral, as measured at Tx. */
    double phaseV[12];
    
    /** Current RMS, Magnitude, Shift, for each phase and neutral, as measured at Tx. */
    double phaseI[12];
    
    /** Voltage RMS, Magnitude, Shift, for each phase and neutral, as measured at end-of-line. */
    double eolV[12];
    
    
public:
    /** Constructor */
    GridModel();
    
    /** Destructor */
    virtual ~GridModel();
    
    /** Initialise, load grid model, add vehicles and other components as required. */
    void initialise(Config* config, LoadFlowInterface* loadflow);
    
    /** Assumes traffic model and charging algorithm have been run, charges
      * or discharges vehicles' batteries as required. */
    void updateVehicleBatteries();
    
    /** Using households' demand profiles, assigns a demand to each house for
      * next interval. */
    void generateLoads(DateTime currTime, HouseholdDemand householdDemand);

    /** Display summary of all individual household and vehicle loads. */
    void displayFullSummary();
    
    /** Display summary of vehicle charging / discharging. */
    void displayVehicleSummary();
    
    /** Display summary of aggregated household and vehicle loads. */
    void displayLoadSummary();
    
    /** Run load flow applying household loads only (and no vehicle loads) at
      * 4:00 am. Required for e.g. distributed charging algorithm.
      * This function should ideally be deprecated, to do. */
    void runValleyLoadFlow(DateTime datetime, HouseholdDemand householdDemand);  
    
    /** Run full load flow using specified load flow interface. */
    void runLoadFlow(DateTime currTime);
    
    /** Return available distribution transformer capacity after household
      * loads are accounted for (required for EqualShare charging algorithm) */
    double getAvailableCapacity();
    

private:
    /** Load the full grid model from loadflow interface / file. */
    void loadGridModel(LoadFlowInterface* loadflow);
    
    /** Add vehicles to the grid model as required. */
    void addVehicles(Config* config);
    
};

#endif	/* GRIDMODEL_H */

