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

#ifndef TRAFFICMODEL_H
#define	TRAFFICMODEL_H

#include <vector>
#include <set>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
//#include <time.h>
#include <iomanip>

#include "../simulator/Config.h"
#include "../gridmodel/Vehicle.h"
#include "../utility/DateTime.h"
#include "../utility/Utility.h"


class TrafficModel {
private:
    std::vector<vehicleRecord_t> weekdayRecords;  // vehicle records on weekdays
    std::vector<vehicleRecord_t> weekendRecords;  // vehicle records on weekends
    
    int home2away;      // keep track of number of transitions
    int away2home;      // keep track of number of transitions
    
public:
    TrafficModel(Config* config);
    virtual ~TrafficModel();
    
    void initialise(DateTime datetime, std::map<int,Vehicle> &vehicles);
    void update(DateTime datetime, std::map<int,Vehicle> &vehicles);
    void displaySummary(std::map<int,Vehicle> vehicles);
private:
    void inputData(std::string filename);
    void assignVehicleRecords(DateTime datetime, std::map<int,Vehicle> &vehicles);
    void getVehicleRecord(Vehicle &vehicle, int n);
};

#endif	/* TRAFFICMODEL_H */

