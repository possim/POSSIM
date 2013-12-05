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
#include "../household/HouseholdDemandModel.h"
#include "../household/Household.h"
#include "../vehicle/Vehicle.h"
#include "DistributionTransformer.h"
#include "NetworkData.h"


/** The full grid model.  This class contains all houses, vehicles, and any
  * further components as required, as well as the network structure, network
  * specs, etc.  It interacts with the load flow interface, sending inputs to 
  * it and reading outputs from it.*/
class GridModel {
private:
    
    /** Local copy of pointer to load flow interface. */
    LoadFlowInterface *loadflow;
    
    /** Percentage of houses having an EV. */
    int evPenetration;
    
    /** Average power factor of household loads (local copy of config variable). */
    double averagePowerFactor;
    
    /** Total household loads at current point in time. */
    double sumHouseholdLoads;
    
    double sumLastVehicleLoads;
    
    /** Mapping of households to their NMI */
    std::map<int, Household*> householdNMImap;
    
     /** Mapping of vehicles to their NMI */
    std::map<int, Vehicle*> vehicleNMImap;
    
    /** Directory for logging data */
    std::string logDir;
    
    /** Directory for temporary data storage / file interaction with load flow software */
    std::string tempDir;
    

public:

    /** Base voltage */
    double baseVoltage;
    
    /** Minimum voltage allowed by distribution code (local copy of config variable). */
    int minVoltage;
    
    /** The pole that is the root of the distribution network tree. */
    FeederPole* root;
    
    /** The Distribution transformer, usually connected at the network root. */
    DistributionTransformer* transformer;
    
    /** All poles in the distribution network, mapped to their names. */
    std::map<std::string, FeederPole*> poles;
    
    /** All power line segments (backbone) in the distribution network, mapped to their names. */
    std::map<std::string, FeederLineSegment*> lineSegments;
    
    /** All Households in the network, mapped to their ID (=NMI) numbers. */
    std::map<std::string, Household*> households;
    
    /** All Vehicles in the network, mapped to the ID(=NMI) numbers of houses they are associated with. */
    std::map<std::string, Vehicle*> vehicles;

    /** All relevant network data */
    NetworkData networkData;
    
    
    
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
    
    /** Using households' demand profiles, generates demand for each house for
      * next interval. */
    void generateLoads(DateTime currTime);

    /** Display summary of all individual household and vehicle loads. */
    void displayFullSummary(DateTime currTime);
    
    /** Display summary of vehicle charging / discharging. */
    void displayVehicleSummary();
    
    /** Display summary of aggregated household and vehicle loads. */
    void displayLoadSummary(DateTime currTime);
    
    /** Run load flow applying household loads only (and no vehicle loads) at
      * 4:00 am. Required for e.g. distributed charging algorithm.
      * This function should ideally be deprecated, to do. */
    void runValleyLoadFlow(DateTime datetime);  
    
    /** Run full load flow using specified load flow interface. */
    void runLoadFlow(DateTime currTime);
    
    /** Return available distribution transformer capacity after household
      * loads are accounted for (required for EqualShare charging algorithm) */
    double getAvailableCapacity();
	
    double getTransformerCapacity();
	
    double getSumHouse();
	
    double getLastVehicleLoad();
    
    /** Determine maximum percentage deviation of any individual phase load from average phase load */
    double getDeviation(DateTime currTime);
    
    /** Find the household with the given NMI */
    Household* findHousehold(int NMI);
    
    /** Find the household with the given NMI */
    Vehicle* findVehicle(int NMI);
    
    /** Add vehicles to the grid model as required. */
    void addVehicles(Config* config);
    
    /** Set logging directory. */
    void setLogDir(std::string logDir);

private:
    /** Load the full grid model from loadflow interface / file. */
    void loadGridModel();
    
    /** Set for each household what the demand model (e.g. generic, phase-specific, etc.) is.*/
    void setHouseholdDemandModel(std::string model);
    
    /** Calculate the total impedance at each household (using DFS traversal of tree)*/
    void calculateHouseholdZ(FeederPole* pole, double r, double x);
    
    /** Following a load flow calculation, calculate voltage unbalance at each
      * pole in the network. */
    void calculateVoltageUnbalance(DateTime currTime);
    
    /** DFS traversal of network tree, recalculating current at each pole. */
    void calculatePoleCurrents(FeederPole* pole, Phasor I[], DateTime currTime);
    
    /** DFS traversal of network tree, recalculating voltages at each pole. */
    void calculatePoleVoltages(FeederPole* pole);
    
    /** Houses are initially mapped to their names.  It can be convenient to
      * have a mapping to their NMI as well.  This function creates that mapping. */
    void buildHouseholdNMImap();

    /** Vehicles are initially mapped to their names.  It can be convenient to
      * have a mapping to their NMI as well.  This function creates that mapping. */
    void buildVehicleNMImap();
};

#endif	/* GRIDMODEL_H */

