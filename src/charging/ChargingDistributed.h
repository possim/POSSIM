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

#ifndef CHARGINGDISTRIBUTED_H
#define	CHARGINGDISTRIBUTED_H

#include <stdlib.h>
#include <algorithm>
#include <iostream>
#include <map>
#include <cmath>
#include <iomanip>

#include "ChargingBaseClass.h"
#include "../simulator/Config.h"
#include "../gridmodel/GridModel.h"
#include "../utility/Utility.h"

/** Uses a distributed charging algorithm in which each vehicle (or rather, 
  * each EV charger) makes individual decisions on whether to schedule the
  * vehicle load or not, based on only local voltage measurements and the 
  * vehicle's state of charge.  This algorithm is described in more detail in:
  * J. de Hoog, D. A. Thomas, V. Muenzel, D. C. Jayasuriya, T. Alpcan, M. Brazil, and I. Mareels,
  * "Electric Vehicle Charging and Grid Constraints: Comparing Distributed and Centralized Approaches",
  * In Proceedings of the IEEE Power and Energy Society General Meeting. Vancouver, Canada, July 2013. */
class ChargingDistributed : public ChargingBaseClass  {
    
private:
    /** Maximum possible charging rate */
    double maxChargeRate;
    
    /** Minimum allowed voltage */
    int minVoltage;
    
    /** Local copy of simulation interval length */
    int simInterval;
    
    /** We don't necessarily want to change the charge rate at each interval, but we do
      * want to have a probability of the vehicle starting to charge over a 
      * given future interval.  This is that interval time, in minutes. */
    int periodOfLoadSchedule;
    
public:
    /** Constructor */
    ChargingDistributed(Config* config, GridModel &gridmodel, DateTime datetime);

    /** Destructor */
    ~ChargingDistributed();
    

    void setAllChargeRates(DateTime datetime, GridModel &gridModel);
    
    void setOneChargeRate(DateTime datetime, GridModel &gridModel, int vehicleID);

};

#endif	/* CHARGING_DISTRIBUTED_H */

