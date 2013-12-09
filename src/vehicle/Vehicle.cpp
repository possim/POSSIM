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

#include "Vehicle.h"


Vehicle::Vehicle(Config* config, int nmi, std::string newName, std::string parentHH) :
    battery(config) {
    NMI = nmi;
    name = newName;
    parentHousehold = parentHH;
    componentName = "";
    activePower = 0;
    inductivePower = 0;
    capacitivePower = 0;
    location = Home;
    isConnected = true;
    isCharging = false;
    distanceDriven = 0;
    L = 0;
    N = 0;
    P = 0;
    switchon = false;
    timeConnected.set(config->getConfigVar("starttime"));
}

Vehicle::~Vehicle() {
}

std::string Vehicle::getName() {
    return name;
}

int Vehicle::getNMI() {
    return NMI;
}

std::string Vehicle::getProfileRef() {
    return travelProfile.name;
}

double Vehicle::getPowerFactor() {
    if(activePower == 0) return 0;
    return activePower/sqrt(pow(activePower, 2) + pow(inductivePower-capacitivePower,2));
}

double Vehicle::getSOC() {
    return battery.SOC;
}

double Vehicle::getSOCchange() {
    return (battery.SOC - battery.SOC_last);
}

void Vehicle::setChargeStart(DateTime datetime) {
    timeConnected = datetime;
}

void Vehicle::setPowerDemand(double active, double inductive, double capacitive) {
    activePower = active;
    inductivePower = inductive;
    capacitivePower = capacitive;
}

void Vehicle::setTravelProfile(vehicleRecord_t vr) {
    travelProfile.name = vr.name;
    travelProfile.travelPairs.clear();
    travelPair_t currPair;
    for(std::list<travelPair_t>::iterator it = vr.travelPairs.begin(); it!=vr.travelPairs.end(); ++it) {
        currPair.distance = it->distance;
        currPair.time = it->time;
        travelProfile.travelPairs.push_back(currPair);
    }
}

void Vehicle::rechargeBattery() {
    battery.recharge(activePower);
}

void Vehicle::dischargeBattery() {
    battery.discharge(distanceDriven);
}

