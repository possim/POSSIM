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

#ifndef BATTERY_H
#define	BATTERY_H

#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>

#include "../simulator/Config.h"
#include "../utility/Utility.h"



/** Implements a battery model that takes into account the number of individual
  * cells in the vehicle battery, and provides a black box model for each.  The
  * black box model itself accounts for internal resistance and dynamic voltage
  * SOC profile. Battery discharge, however, is modelled in a much simpler way:
  * For a given vehicle trip, that trip's percentage of total vehicle range is
  * assumed to map linearly to change in SOC.  This could certainly be improved
  * in the future, although battery discharge does depend on difficult factors
  * such as driving style. */
class Battery {
private:
    /** Path to file containing battery specification. */
    std::string batteryFile;
    
    /** Local copy of simulation interval size (required to update SOC). */
    int    simInterval;
    
    /** Maximum range that can be driven with this battery.  (e.g. 160km for Nissan Leaf)*/
    double range;
    
    /** Discount factor taking into account losses in charge process as well as
      * losses due to peripheral demand (lights, heating, cooling, etc.) */
    double chargingEfficiency;
    
    /** Number of parallel cells per block */
    int    numParallelCells;
    
    /** Number of blocks in series (each block can contain more than one cell in parallel)*/
    int    numSeriesBlocks;
    
    /** Maximum cell capacity */
    double C_Cell_Max;          
    
    /** Maximum battery capacity (all cells) */
    double C_Total_Max;         
    
    /** Safe upper capacity limit, full battery */
    double C_SafeUpper;         
    
    /** Safe lower capacity limit, full battery */
    double C_SafeLower;         
    
    /** Individual cell voltage */
    double V_Cell;              
    
    /** Individual cell internal resistance */
    double R_Cell;              
    
    /** Upper voltage limit, cell */
    double V_Max;               
    
    /** SOC-Voltage profile */
    std::map<double,double> SOC_V_Pairs;        

public:
    
    /** Capacity of whole battery, Ah */
    double capacity;            
    
    /** SOC of whole battery, % */
    double SOC;                 
    
    /** SOC of whole battery at last interval */
    double SOC_last;            
    
    
public:
    /** Constructor */
    Battery(Config* config);
    
    /** Destructor */
    virtual ~Battery();

    /** Recharge battery for one simulation interval at given charge rate. */
    void recharge(double chargeRate);
    
    /** Discharge battery given distance driven. */
    void discharge(double distance);
    
    /** Display battery details. */
    void displayCharacteristics();

private:
    /** Upload key battery specification to populate model. */
    void uploadCharacteristics(std::string filename);
    
    /** Find open circuit voltage given current SOC. */
    double find_V_OpenCircuit();
    
    /** Calculate current through each cell, given power being drawn and SOC-dependent voltage. */
    double calculate_I_Cell(double powerCell, double presentVoltage);

};

#endif	/* BATTERY_H */

