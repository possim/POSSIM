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
    int start, end;
    std::string line;
    configvar_t currvar;

    // Open config file     
    std::ifstream infile("possim_config.xml");
    if(!infile){
       std::cout << "Cannot open possim config file!" << std::endl;
       exit (1);
    } 

    // Ignore first line
    getline(infile, line);

    // Loop through remaining lines
    while(getline(infile, line)) {
        if(!line.empty() && line.size()>2) {
            start = line.find_first_of("<")+1;
            end = line.find_first_of(" ", start);
            currvar.name = line.substr(start, end-start);
            
            start = line.find_first_of("\"", end) + 1;
            end = line.find_first_of("\"", start);
            currvar.flag = line.substr(start, end-start);
            
            start = line.find_first_of("\"", end+1) + 1;
            end = line.find_first_of("\"", start);
            currvar.value = line.substr(start, end-start);
            
            configVars.push_back(currvar);
        }
    }
    
    // Ensure right values for particular variables
    correctValues();
}

// Destructor
Config::~Config() {
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
}

// Return string value of a given config variable.
std::string Config::getConfigVar(std::string name) {
    for(size_t i=0; i<configVars.size(); i++)
        if(configVars.at(i).name == name)
            return configVars.at(i).value;
    
    std::cout << std::endl << "WARNING: DID NOT FIND VALUE FOR CONFIG VARIABLE \"" << name << "\"!" << std::endl;
    return NULL;
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
void Config::setConfigVar(std::string name, std::string value) {
    for(size_t i=0; i<configVars.size(); i++)
        if(configVars.at(i).name == name)
            configVars.at(i).value = value;
}

// Set a given config variable by flag (e.g. from command line)
void Config::setConfigVarByFlag(std::string flagIn, std::string value) {
    std::string flag = flagIn.substr(1,flagIn.size());
    for(size_t i=0; i<configVars.size(); i++)
        if(configVars.at(i).flag == flag)
            configVars.at(i).value = value;
    
    correctValues();
}

// Print all options, flags, defaults
void Config::printOptions() {
    for(size_t i=0; i<configVars.size(); i++)
        std::cout << " -" << std::setw(5) << std::left << std::setiosflags(std::ios::fixed) << configVars.at(i).flag
                  << " "  << std::setw(20) << std::left << std::setiosflags(std::ios::fixed) << configVars.at(i).name
                  << " (default: " << configVars.at(i).value << ")"
                  << std::endl;
}

// Print all config variables and their current values
void Config::printAll() {
    for(size_t i=0; i<configVars.size(); i++)
        std::cout << " "  << std::setw(20) << std::right << std::setiosflags(std::ios::fixed) << configVars.at(i).name
                  << ": " << configVars.at(i).value << std::endl;
}

// Return string of all config variables and their current values
std::string Config::toString() {
    std::stringstream ss;
    for(size_t i=0; i<configVars.size(); i++)
        ss << std::setw(20) << std::right << std::setiosflags(std::ios::fixed) << configVars.at(i).name
           << ": " << configVars.at(i).value << std::endl;
    return ss.str();
}
