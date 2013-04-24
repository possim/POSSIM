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

#ifndef VEHICLE_H
#define	VEHICLE_H



#include <iostream>
#include <list>

#include "../simulator/Config.h"
#include "../battery/Battery.h"
#include "../utility/DateTime.h"


enum VehicleLocation {
    Away = 0,
    Home = 1
};
    
struct travelPair_t {
    DateTime time;
    double distance;
};

struct vehicleRecord_t {
    std::string name;
    std::list<travelPair_t> travelPairs;
};

class Vehicle {
   
private:

    int NMI;                    // National Meter Identifier.  Must be linked 
                                // to household with same NMI
    std::string componentRef;   // Name of representative entity in load flow model
    Battery battery;            // Maintains battery model (level of charge)

    
public:
    VehicleLocation location;

    vehicleRecord_t travelProfile; // Travel profile for 24-hour period

    bool        isConnected;      // Whether connected to charger or not
    DateTime    timeConnected;    // Time at which last connected to charger
    
    bool        isCharging;       // Whether vehicle is charging
    double      chargeRate;       // Rate at which vehicle should charge
    
    double activePower;           // current demand of this vehicle
    double inductivePower;
    double capacitivePower;
    
    double distanceDriven;       // Used by traffic model to set amount driven in last sim step
    
    double L, N, P;             // Temp for dist charging testing
    bool switchon;
    
public:
    Vehicle(Config* config, int nmi, std::string name);
    virtual ~Vehicle();
    
    void setChargeStart(DateTime datetime);
    void setPowerDemand(double active, double inductive, double capacitive);

    std::string getName();
    int getNMI();
    std::string getProfileRef();
    double getPowerFactor();
    
    double getSOC();
    double getSOCchange();
    
    void setTravelProfile(vehicleRecord_t vr);
    
    void rechargeBattery();
    void dischargeBattery();
};

#endif	/* VEHICLE_H */

