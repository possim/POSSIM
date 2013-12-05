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

#ifndef HOUSEHOLDDEMAND_H
#define	HOUSEHOLDDEMAND_H

#include <iostream>
#include <fstream>
#include <exception>
#include <stdlib.h>
#include <vector>
#include <map>

#include "../simulator/Config.h"
#include "../utility/DateTime.h"
#include "../utility/Utility.h"
#include "../utility/Power.h"


/** A full household demand profile for a 24-hour period.  Stores all 
  * time - load pairs for this profile, plus a unique name for it.  */
struct HouseholdDemandProfile {
    std::string name;
    std::map<DateTime, S_Load> demand;
};

#include "Household.h"

/** Generates 24-hour demand profiles for households */
class HouseholdDemandModel {
private:
    
    /** Local copy of sim interval length */
    int simInterval;
    
    /** Determines whether to use:
      * (i)   generic profiles ("generic") 
      * (ii)  randomly assigned profiles ("random") 
      * (iii) phase-specific profiles, ("phasespecific"), or
      * (iv)  house-specific profiles ("housespecific") */
    std::string modelType;
    
    /** Path to demand data directory */
    std::string demandDataDir;
    
    /** Map of profile names to full demand profiles */
    std::map<std::string, HouseholdDemandProfile> demandProfiles;
    
    /** Path to file indicating phase allocation (which houses are on which phase) */
    std::string houseProfileAllocFile;
    
    /** Mapping of house names to demand profile names*/
    std::map<std::string,std::string> allocationMap;
    
    /** Add randomness to demand */
    double* randomDistribution;
    
public:
    /** Constructor */
    HouseholdDemandModel(Config* config);
    
    /** Destructor */
    virtual ~HouseholdDemandModel();
    
    /** Assign a full 24-hour demand profile to each household */
    void assignProfiles(std::map<std::string, Household*> &households);
    
private:
    /** Choose a random profile */
    HouseholdDemandProfile getRandomProfile();
    
    /** Input all profiles in the specified demanddatadir directory */
    void inputAllProfiles();
    
    /** Input a specific profile from file */
    HouseholdDemandProfile inputProfileFromFile(std::string filename);
    
    /** Input profile allocation from file (matching houses to specific profiles) */
    void inputProfileAllocationFromFile();
};

#endif	/* HOUSEHOLDDEMAND_H */

