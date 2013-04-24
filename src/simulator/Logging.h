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

#ifndef LOGGING_H
#define	LOGGING_H

#include <iostream>
#include <map>
#include <sys/stat.h>
#include <sys/types.h>
//#include <time.h>
#include <fstream>
#include <sstream>
#include <boost/filesystem.hpp>

#include "Config.h"
#include "../utility/Utility.h"
#include "../gridmodel/GridModel.h"
#include "../gridmodel/Household.h"
#include "../gridmodel/Vehicle.h"
#include "../utility/DateTime.h"
#include "../charging/ChargingBaseClass.h"
#include "../spotprice/SpotPrice.h"

class Logging {
public:
    Logging();
    void initialise(Config* config, GridModel gridmodel);
    virtual ~Logging();
    
    void createDir();
    std::string getDir();
    void update(DateTime currtime, GridModel gridmodel, ChargingBaseClass *charger, SpotPrice spotPrice);

private:
    std::string directory;
    std::string file_parameters;
    std::string file_demandHH;
    std::string file_demandEV;
    std::string file_demandTotal;
    std::string file_locationEV;
    std::string file_spotPrice;
    std::string file_phaseV;
    std::string file_phaseI;
    std::string file_eolV;
    std::string file_powerFactor;
    std::string file_householdV;
    std::string file_batterySOC;
    std::string file_probchargeEV;
};

#endif	/* LOGGING_H */

