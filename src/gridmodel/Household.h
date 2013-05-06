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

/** Represents a household in the grid.  Includes ID (national meter identifier),
  * demand profile, and local voltage measurements.*/
class Household {

private:
    /** ID (equivalent to NMI, national meter identifier)*/
    int NMI;                    
    
    /** Name of equivalent component of this household in loadflow model */
    std::string componentRef;
    
    /** Demand profile */
    double demandProfile;
    
    /** 4-tuple to add random effects on household demand */
    double demandRandomness[4];
    
public:
    /** True if there is an EV associated with this house. */
    bool hasCar;
    
    /** Voltage, current interval, RMS */
    double V_RMS;
    
    /** Voltage, current interval, magnitude */
    double V_Mag;
    
    /** Voltage, current interval, phase */
    double V_Pha;
    
    /** Power demand, active */
    double activePower;
    
    /** Power demand, inductive */
    double inductivePower;
    
    /** Power demand, capacitive */
    double capacitivePower;
    
    /** Voltage at this house during valley load (for e.g. distributed charging alg) */
    double V_valley;
    
public:
    /** Constructor */
    Household(int nmi, std::string name, double dP);
    
    /** Destructor */
    virtual ~Household();
    
    /** Returns name of component this Household is tied to in loadflow */
    std::string getComponentRef();
    
    /** Returns NMI */
    int getNMI();
    
    /** Returns demand profile */
    double getDemandProfile();

    /** Sets power demand */
    void setPowerDemand(double active, double inductive, double capacitive);

    /** Gets power factor of this household */
    double getPowerFactor();

    
};

#endif	/* HOUSEHOLD_H */

