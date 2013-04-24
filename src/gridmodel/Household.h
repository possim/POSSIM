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


class Household {

private:
    int NMI;                    // national meter identifier
    std::string componentRef;   // name of component of this household in loadflow model
    double demandProfile;       // adds randomness according to normal distribution (1,0.2)
    
public:
    bool hasCar;                // EV associated with this house?
    double V_RMS;               // RMS Voltage
    double V_Mag;               // Voltage Magnitude
    double V_Pha;               // Voltage Phase
    double activePower;         // current demand of this household
    double inductivePower;
    double capacitivePower;
    
    double V_valley;            // Voltage at this house during valley load (for e.g. distributed charging alg)

public:
    Household(int nmi, std::string name, double dP);
    virtual ~Household();
    
    std::string getName();
    int getNMI();
    double getDemandProfile();

    void setPowerDemand(double active, double inductive, double capacitive);
    double getPowerFactor();

private:

};

#endif	/* HOUSEHOLD_H */

