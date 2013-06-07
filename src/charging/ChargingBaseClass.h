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

#ifndef CHARGINGBASECLASS_H
#define	CHARGINGBASECLASS_H

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>

#include "../gridmodel/GridModel.h"
#include "../simulator/Config.h"


/** Base class for EV charging algorithms.  Any electric vehicle charging
  * algorithm in the directory /src/charging may be chosen via the simulator
  * configuration.  To allow charging algorithm to exist as objects within the
  * code, polymorphism is used.  This is the base class that any charging
  * algorithm must inherit. */
class ChargingBaseClass {

public:
    /** Constructor */
    ChargingBaseClass(Config* config, GridModel gridModel);
    
    /** Destructor */
    ~ChargingBaseClass();
    
    /** Set charge rates of all vehicles in model at current date and time. */
    virtual void setChargeRates(DateTime datetime, GridModel &gridModel) = 0;
};


#endif	/* CHARGINGBASECLASS_H */

