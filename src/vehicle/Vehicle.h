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

// No need to include helper struct in documentation
/** \cond HIDDEN SYMBOLS */
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
    bool isWeekday;
    std::list<travelPair_t> travelPairs;
};
/** \endcond */


/** Represents a household in the grid.  Includes ID (national meter identifier),
  * demand profile, and vehicle battery.*/
class Vehicle {
   
public:

    /** ID (equivalent to NMI, national meter identifier) - linked to household
      * where this vehicle connects */
    int NMI;                    
    
    /** Name of vehicle */
    std::string name;
    
    /** Name of component in load flow model this house is represented by */
    std::string componentName;
    
    /** Name of house this vehicle is connected to */
    std::string parentHousehold;
    
    /** The vehicle battery. */
    Battery battery;

    /** Vehicle location: Home / Away / Other? */
    VehicleLocation location;

    /** Travel profile of this vehicle.  Renewed / regenerated every 24 hours. */
    vehicleRecord_t travelProfile;
    
    /** True if vehicle is connected to charger. */
    bool        isConnected;
    
    /** Datetime at which vehicle connected to charger. */
    DateTime    timeConnected;
    
    /** True if vehicle is charging. */
    bool        isCharging;
    
    /** Vehicle's charge rate (in kW), as assigned by charging algorithm. */
    double      chargeRate;
    
   
    /** Active power demand */
    double activePower;
    
    /** Inductive power demand */
    double inductivePower;
    
    /** Capacitive power demand */
    double capacitivePower;
    
    /** If vehicle arrived home at last time step, this variable stores
      * distance driven (for battery SOC update) */
    double distanceDriven;
    
    /** Temporary variable, to be removed soon. */
    double L, N, P;
    
    /** Logging, debug purposes: check if vehicle state changed since last step. */
    bool switchon;
    
public:
    /** Constructor */
    Vehicle(Config* config, int nmi, std::string newName, std::string newParentHH);
    
    /** Destructor */
    virtual ~Vehicle();
    
    /** Set start of charging */
    void setChargeStart(DateTime datetime);
    
    /** Set vehicle's power demand */
    void setPowerDemand(double active, double inductive, double capacitive);

    /** Get name of this vehicle. */
    std::string getName();
    
    /** Get vehicle ID (NMI, tied to house this vehicle is connected to).*/
    int getNMI();
    
    /** Get ID of travel profile this vehicle is currently using. */
    std::string getProfileRef();
    
    /** Set vehicle power factor. */
    double getPowerFactor();
    
    /** Get vehicle state of charge. */
    double getSOC();
    
    /** Get change in state of charge since last simulation interval. */
    double getSOCchange();
    
    /** Assign a new travel profile to this vehicle. */
    void setTravelProfile(vehicleRecord_t vr);
    
    /** Recharge vehicle battery. */
    void rechargeBattery();
    
    /** Discharge vehicle battery. */
    void dischargeBattery();
};

#endif	/* VEHICLE_H */

