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

#ifndef HOUSEHOLD_H
#define	HOUSEHOLD_H

#include <iostream>
#include <vector>
#include <sstream>

#include "../utility/Utility.h"
#include "../utility/Power.h"
#include "../gridmodel/Feeder.h"
#include "HouseholdDemandModel.h"


/** Represents a household in the grid.  Includes ID (national meter identifier),
  * demand profile, and local voltage measurements.*/
class Household {
private:
    /** Stores demand model type (e.g. generic, housespecific, etc.) */
    std::string modelType;
    
public:
    /** ID (equivalent to NMI, national meter identifier)*/
    int NMI;                    
    
    /** Name of household */
    std::string name;
    
    /** Name of component in load flow model this house is represented by */
    std::string componentName;
    
    /** Demand profile: greater than 1 if this is a high-use house,  less
      * than 1 if it's a low-use house */
    double demandProfileFactor;
    
    /** 4-tuple to add normal distribution random effects on household 
      * demand each interval */
    double demandRandomness[4];
    
    /** Phase that this household is connected to. */
    Phase phase;
    
    /** Base voltage of network this household is in. */
    double baseVoltage;
    
    /** This household's full demand profile for a 24-hour period. */
    HouseholdDemandProfile demandProfile;
    
    /** Name of parent pole that this household is connected to. */
    std::string parentPoleName;
    
    /** True if there is an EV associated with this house. */
    bool hasCar;
    
    /** Voltage, current interval, RMS */
    double V_RMS;
    
    /** Voltage, current interval, magnitude */
    double V_Mag;
    
    /** Voltage, current interval, phase */
    double V_Pha;
    
    /** Voltage at this house during valley load (for e.g. distributed charging alg) */
    double V_valley;
    
    /** Phase unbalance at this house ( |V_-| / |V_+|), as a percentage */
    double V_unbalance;
    
    /** True if the Household has been assigned a parent pole (for tree building) */
    bool hasParent;
    
    /** The service line (from pole to house)*/
    lineModel serviceLine;
    
    /** The total impedance*/
    Impedance totalImpedanceToTX;
    
    /** Active power demand */
    double activePower;
    
    /** Inductive power demand */
    double inductivePower;
    
    /** Capacitive power demand */
    double capacitivePower;
    
    
public:
    /** Constructor */
    Household();

    /** Destructor */
    virtual ~Household();
    
    /** Set model type (generic, phase-specific, etc.)*/
    void setModelType(std::string mt);

    /** Returns demand at specified time of day.  If no value is available
      * for the specific time, then demand value for nearest available time is
      * returned. */
    S_Load generateDemandAt(DateTime datetime);
    
    /** Returns power factor at given date and time */
    double getPowerFactor(DateTime datetime);
    
private:
    // The following two functions are helpers, depending on what demand model type is being used
    S_Load generateDemandAtExactTime(DateTime datetime);
    S_Load generateDemandAtTimeOfDay(DateTime datetime);
};

#endif	/* HOUSEHOLD_H */

