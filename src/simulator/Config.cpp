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

#include "Config.h"


// Constructor:  open config file, read all values.
Config::Config() {
    // Input from default xml file
    inputXML("configs/defaultconfig.xml");
    
    // Ensure right values for particular variables
    correctValues();
}

// Destructor
Config::~Config() {
}

// Read in all config variables as specified in this xml file
void Config::inputXML(std::string filename) {
    int start, end;
    std::string line, name;
    configvar_t currvar;

    // Open config file     
    std::ifstream infile(filename.c_str());
    if(!infile){
       std::cout << "Cannot open config file: " << filename << "!" << std::endl;
       return;
    } 

    // Ignore first line
    getline(infile, line);

    // Loop through remaining lines
    // (Could really do with a better xml parser here!)
    while(getline(infile, line)) {
        if(!line.empty() && line.size()>2) {
            start = line.find_first_of("<")+1;
            end = line.find_first_of(" ", start);
            name = line.substr(start, end-start);
            
            start = line.find_first_of("\"", end) + 1;
            end = line.find_first_of("\"", start);
            currvar.flag = line.substr(start, end-start);
            
            start = line.find_first_of("\"", end+1) + 1;
            end = line.find_first_of("\"", start);
            currvar.value = line.substr(start, end-start);
            
            configVars[name] = currvar;
        }
    }
}

// Some simple input error checking.  Allows config to have e.g. any of the 
// following with same result:  MATLAB, Matlab, matlab, 1.
void Config::correctValues() {
    std::string value = getConfigVar("loadflowsimulator");
    if(value.compare("Testing") == 0 ||
       value.compare("testing") == 0 ||
       value.compare("0") == 0)
        setConfigVar("loadflowsimulator", "testing");
    else if(value.compare("Matlab") == 0 ||
            value.compare("MATLAB") == 0 ||
            value.compare("matlab") == 0 ||
            value.compare("1") == 0)
               setConfigVar("loadflowsimulator", "matlab");
    else if(value.compare("DIGSILENT") == 0 ||
            value.compare("DigSilent") == 0 ||
            value.compare("digsilent") == 0 ||
            value.compare("2") == 0)
               setConfigVar("loadflowsimulator", "digsilent");
    
    value = getConfigVar("chargingalgorithm");
    if(value.compare("Uncontrolled") == 0 ||
       value.compare("uncontrolled") == 0 ||
       value.compare("0") == 0)
        setConfigVar("chargingalgorithm", "uncontrolled");
    else if(value.compare("EqualShare") == 0 ||
            value.compare("equalshare") == 0 ||
            value.compare("1") == 0)
               setConfigVar("chargingalgorithm", "equalshare");
    else if(value.compare("Distributed") == 0 ||
            value.compare("distributed") == 0 ||
            value.compare("2") == 0)
               setConfigVar("chargingalgorithm", "distributed");
    else if(value.compare("TOU") == 0 ||
            value.compare("tou") == 0 ||
            value.compare("4") == 0)
               setConfigVar("chargingalgorithm", "TOU");
    else if(value.compare("OPTIMAL1") == 0 ||
            value.compare("Optimal1") == 0 ||
            value.compare("optimal1") == 0 ||
            value.compare("5") == 0)
               setConfigVar("chargingalgorithm", "optimal1");
    else if(value.compare("OPTIMAL2") == 0 ||
            value.compare("Optimal2") == 0 ||
            value.compare("optimal2") == 0 ||
            value.compare("6") == 0)
               setConfigVar("chargingalgorithm", "optimal2");
    else if(value.compare("OPTIMAL3") == 0 ||
            value.compare("Optimal3") == 0 ||
            value.compare("optimal3") == 0 ||
            value.compare("7") == 0)
               setConfigVar("chargingalgorithm", "optimal3");
    else if(value.compare("Discrete") == 0 ||
            value.compare("discrete") == 0 ||
            value.compare("8") == 0)
               setConfigVar("chargingalgorithm", "discrete");
    else if(value.compare("MPC") == 0 ||
            value.compare("mpc") == 0 ||
            value.compare("9") == 0)
               setConfigVar("chargingalgorithm", "mpc");
    else if(value.compare("Wplug") == 0 ||
            value.compare("WPlug") == 0 ||
            value.compare("wplug") == 0 ||
            value.compare("10") == 0)
               setConfigVar("chargingalgorithm", "wplug");
}

// Return string value of a given config variable.
std::string Config::getConfigVar(std::string name) {
    std::string val = configVars[name].value;
    if(val.length() == 0)
        std::cout << "\n\nWARNING: Did not find value for config variable " << name << "!\n" << std::endl;
    return val;
}

// If config variable is boolean, get bool value directly
bool Config::getBool(std::string name) {
    std::string value = getConfigVar(name);
    if(value.compare("Yes") == 0  ||
       value.compare("True") == 0 ||
       value.compare("yes") == 0  ||
       value.compare("true") == 0 ||
       value.compare("1") == 0)
        return true;
    return false;
}

// If config variable is int, get int value directly
int Config::getInt(std::string name) {
    std::string value = getConfigVar(name);
    return utility::string2int(value);
}

// If config variable is double, get double value directly
double Config::getDouble(std::string name) {
    std::string value = getConfigVar(name);
    return utility::string2double(value);
}

// If config variable is string, get string value directly
std::string Config::getString(std::string name) {
    return getConfigVar(name);
}

// Return int value of load flow simulation tool
int Config::getLoadFlowSim() {
    std::string value = getConfigVar("loadflowsimulator");
    if(value.compare("testing") == 0)
        return 0;
    else if(value.compare("matlab") == 0)
               return 1;
    else if(value.compare("digsilent") == 0)
               return 2;
    
    return 1;
}

// Return int value of charging algorithm
int Config::getChargingAlg() {
    std::string value = getConfigVar("chargingalgorithm");
    if(value.compare("uncontrolled") == 0)
        return 0;
    else if(value.compare("equalshare") == 0)
        return 1;
    else if(value.compare("distributed") == 0)
        return 2;
    else if(value.compare("ergon") == 0)
        return 3;
    else if(value.compare("TOU") == 0)
        return 4;
    else if(value.compare("optimal1") == 0)
        return 5;
    else if(value.compare("optimal2") == 0)
        return 6;
    else if(value.compare("optimal3") == 0)
        return 7;
    else if(value.compare("discrete") == 0)
        return 8;
    else if(value.compare("mpc") == 0)
        return 9;
    else if(value.compare("wplug") == 0)
        return 10;
    
    return 0;
}

// Return 4-tuple specifying probability distribution to add noise to a given
// parameter.  4-tuple is (mean, std_deviation, max_value, min_value).
double* Config::getRandomParams(std::string name) {
    std::string value = getConfigVar(name);
    std::vector<std::string> tokens;
    utility::tokenize(value, tokens, ",");
    double *vars = new double[4];
    for(int i=0; i<4; i++)
        vars[i] = utility::string2double(tokens.at(i));
    return vars;
}

// Set a given config variable by name
void Config::setConfigVar(std::string name, std::string val) {
    configVars[name].value = val;
}

// Set a given config variable by flag (e.g. from command line)
void Config::setConfigVarByFlag(std::string flagIn, std::string value) {
    std::string flag = flagIn.substr(1,flagIn.size());
    for(std::map<std::string,configvar_t>::iterator it = configVars.begin(); it!=configVars.end(); ++it)
        if(it->second.flag == flag)
            it->second.value = value;
    
    correctValues();
    
    if(flag == "C")
        inputXML(value);
}

// Print all options, flags, defaults
void Config::printOptions() {
    for(std::map<std::string,configvar_t>::iterator it = configVars.begin(); it!=configVars.end(); ++it)
        std::cout << " -" << std::setw(5) << std::left << std::setiosflags(std::ios::fixed) << it->second.flag
                  << " "  << std::setw(20) << std::left << std::setiosflags(std::ios::fixed) << it->first
                  << " (default: " << it->second.value << ")"
                  << std::endl;
}

// Print all config variables and their current values
void Config::printAll() {
    for(std::map<std::string,configvar_t>::iterator it = configVars.begin(); it!=configVars.end(); ++it)
        std::cout << " "  << std::setw(20) << std::right << std::setiosflags(std::ios::fixed) << it->first
                  << ": " << it->second.value << std::endl;
}

// Return string of all config variables and their current values
std::string Config::toString() {
    std::stringstream ss;
    for(std::map<std::string,configvar_t>::iterator it = configVars.begin(); it!=configVars.end(); ++it)
        ss << std::setw(20) << std::right << std::setiosflags(std::ios::fixed) << it->first
           << ": " << it->second.value << std::endl;
    return ss.str();
}
