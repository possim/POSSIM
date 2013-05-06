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

#include "Battery.h"

// Upload battery specs, set initial values, randomness to initial SOC if
// specified by config
Battery::Battery(Config* config) {
    batteryFile = config->getString("batterydata");
    simInterval = config->getInt("simulationinterval");
    
    uploadCharacteristics(batteryFile);
    
    // Random initial capacity of battery
    double randCapacity = utility::randomNormal(config->getRandomParams("batteryrandom"));
    capacity = config->getDouble("batterystart") + randCapacity;
    if(capacity > C_SafeUpper)
        capacity = C_SafeUpper;
    SOC = (capacity - C_SafeLower) / (C_SafeUpper - C_SafeLower) * 100;
    SOC_last = SOC;
}

Battery::~Battery() {
}


void Battery::uploadCharacteristics(std::string filename) {  
   std::string line;
   std::vector<std::string> tokens;
   double thisSOC, thisV;
   
  
   // Open data file
   std::fstream infile(filename.c_str(), std::ios::in);
   if(!infile){
      std::cout << std::endl << "ERROR: Cannot open battery input file: " << filename << std::endl;
      std::cout.flush();
      exit (1);
   }
   
   // Input range
   getline(infile, line);
   utility::tokenize(line, tokens, " ");
   range = utility::string2double(tokens[1]);
   
   // Input charging efficiency
   tokens.clear();
   getline(infile, line);
   utility::tokenize(line, tokens, " ");
   chargingEfficiency = utility::string2double(tokens[1]);
   
   // Input number of cells in parallel, in each pack
   tokens.clear();
   getline(infile, line);
   utility::tokenize(line, tokens, " ");
   numParallelCells = utility::string2int(tokens[1]);
   
   // Input number of packs in series
   tokens.clear();
   getline(infile, line);
   utility::tokenize(line, tokens, " ");
   numSeriesBlocks = utility::string2int(tokens[1]);
   
   // Input total capacity
   tokens.clear();
   getline(infile, line);
   utility::tokenize(line, tokens, " ");
   C_Cell_Max = utility::string2double(tokens[1]);
   C_Total_Max = C_Cell_Max * numParallelCells;
   
   // Input upper limit, useable capacity
   tokens.clear();
   getline(infile, line);
   utility::tokenize(line, tokens, " ");
   C_SafeUpper = C_Total_Max * utility::string2double(tokens[1]);
   
   // Input lower limit, useable capacity
   tokens.clear();
   getline(infile, line);
   utility::tokenize(line, tokens, " ");
   C_SafeLower = C_Total_Max * utility::string2double(tokens[1]);
   
   // Input nominal voltage
   tokens.clear();
   getline(infile, line);
   utility::tokenize(line, tokens, " ");
   V_Cell = utility::string2double(tokens[1]);
   
   // Input internal resistance
   tokens.clear();
   getline(infile, line);
   utility::tokenize(line, tokens, " ");
   R_Cell = utility::string2double(tokens[1]);
   
   // Input maximum voltage
   tokens.clear();
   getline(infile, line);
   utility::tokenize(line, tokens, " ");
   V_Max = utility::string2double(tokens[1]);
   
   // skip line
   getline(infile, line); 
   
   // Input battery voltage profile
   while(getline(infile, line)) {
       tokens.clear();
       utility::tokenize(line, tokens, ",");
       thisSOC = utility::string2double(tokens[0]);
       thisV = utility::string2double(tokens[1]);
       SOC_V_Pairs.insert(std::pair<double, double>(thisSOC, thisV));
   }

   infile.close();
}

void Battery::displayCharacteristics() {
    std::cout << "Battery has:" << std::endl
              << "  range              : " << range << std::endl
              << "  charging efficiency: " << chargingEfficiency << std::endl
              << "  useable upper limit: " << C_SafeUpper << std::endl
              << "  useable lower limit: " << C_SafeLower << std::endl
              << "  cells in parallel  : " << numParallelCells << std::endl
              << "  blocks in series   : " << numSeriesBlocks << std::endl
              << "  total capacity     : " << C_Total_Max << std::endl
              << "  nominal voltage    : " << V_Cell << std::endl
              << "  internal resistance: " << R_Cell << std::endl
              << "  maximum voltage    : " << V_Max << std::endl
              << "  num of soc-v pairs : " << SOC_V_Pairs.size() << std::endl;
}

double Battery::find_V_OpenCircuit() {
    double adjustedSOC = (double)((int)(SOC*10))/10;  // get precision to 0.1
    
    return(SOC_V_Pairs.at(adjustedSOC));
}

double Battery::calculate_I_Cell(double P_Cell, double V_OpenCircuit) {
    //find solution to quadratic equation
    double a = R_Cell;
    double b = V_OpenCircuit;
    double c = -1*P_Cell;
    
    double currentCell = (-1*b + sqrt(pow(b,2) - 4*a*c)) / (2*a);
    
    return currentCell;
}

void Battery::recharge(double chargeRate) {
    if(SOC >= 100) {
        if(chargeRate > 0)
                std::cout << "ERROR: Trying to charge with rate " << chargeRate 
                          << " when battery is already at 100%!" << std::endl;
        SOC_last = 100;
        SOC = 100;
        return;
    }
    
    double P_Charge = chargeRate * chargingEfficiency;
    double P_Cell = P_Charge / double(numParallelCells * numSeriesBlocks);
    double V_OpenCircuit = find_V_OpenCircuit();
    
    double I_Cell = calculate_I_Cell(P_Cell, V_OpenCircuit);
    
    //std::cout << "  ChargeRate: " << chargeRate << ", "
    //          << "CellPower: " << P_Cell << ", "
    //          << "V_OpenCircuit: " << V_OpenCircuit << ", "
    //          << "I_Cell: " << I_Cell << std::endl;
    
    // Respect battery's upper voltage limit
    if(V_OpenCircuit + I_Cell*R_Cell > V_Max) 
        I_Cell = (V_Max - V_OpenCircuit) / R_Cell;

    double C_Increase = I_Cell * numParallelCells * (double)simInterval/60;
    capacity = capacity + C_Increase;
    
    double newSOC = (capacity - C_SafeLower) / (C_SafeUpper - C_SafeLower) * 100;
    if(newSOC > 100)
        newSOC = 100;
    
    //std::cout << "  Recharged at current of " << I_Cell << " per cell"
    //          << ", state of charge changed from " << SOC 
    //          << "% to " << newSOC << "%" << std::endl;
    
    SOC_last = SOC;
    SOC = newSOC;
}


void Battery::discharge(double distance) {
    double socDecrease = distance / range * 100;
    
    SOC_last = SOC;

    // Could use some better error checking here?
    if(socDecrease > SOC) {
        std::cout << "  WARNING: CAR WOULD HAVE RUN OUT OF POWER.  Setting SOC to 0." << std::endl;
        SOC = 0;
        capacity = 0;
    }
    else {
        SOC = SOC-socDecrease;
        capacity = C_SafeLower + SOC/100 * (C_SafeUpper - C_SafeLower);
    }
}

