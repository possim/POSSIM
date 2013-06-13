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

#ifndef CONFIG_H
#define	CONFIG_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdio.h>
#include "../utility/DateTime.h"
#include "../utility/Utility.h"

/** Name-flag-value triple for command-line parameters. */
struct configvar_t {
    std::string flag;
    std::string value;
};

/** Global simulation configuration parameters.  These are set in the file 
  * possim_config.xml, but may also be changed via command-line flags. */
class Config {
    
private:
    /** Mapping of sim parameter names to their command line flag and value. */
    std::map<std::string,configvar_t> configVars;
    
public:
    /** Constructor.  Open simulator config file (located at 
      * /POSSIM_home/possim_config.xml), and read all simulation parameters
      * into a globally accessible config object. */
    Config();
    
    /** Destructor */
    virtual ~Config();

    /** Returns string equivalent of a config variable's value. */
    std::string getConfigVar(std::string name);
    
    /** Returns boolean value directly of a boolean config variable. */
    bool        getBool(std::string name);
    
    /** Returns int value directly of an integer config variable. */
    int         getInt(std::string name);
    
    /** Returns double value directly of a double config variable. */
    double      getDouble(std::string name);
    
    /** Returns string value directly of a string config variable. */
    std::string getString(std::string name);
    
    /** There are multiple load flow simulator options; this returns the int
      * value associated with a given load flow simulation option. */
    int         getLoadFlowSim();
    
    /** There are multiple charging algorithms available; this returns the int
      * value associated with a given charging algorithm. */
    int         getChargingAlg();
    
    /** Randomness (noise) can be added to various simulation parameters via
      * probability distributions defined by "randomness 4-tuples" of 
      * (mean, std_deviation, max_value, min_value).  This returns a randomness
      * 4-tuple config variable. */
    double*     getRandomParams(std::string name);
    
    /** Set a config variable by name. */
    void        setConfigVar(std::string name, std::string value);
    
    /** Set a config variable by flag (e.g. from command line). */
    void        setConfigVarByFlag(std::string flag, std::string value);
    
    /** Display all config variables, their flags and defaults. */
    void printOptions();
    
    /** Display all config variables and their current values. */
    void printAll();
    
    /** Return config variable list as a string. */
    std::string toString();
    
private:
    /** Input all config variables specified in the given XML file. */
    void inputXML(std::string filename);
    
    /** Some simple input error checking for load flow, charge algorithm inputs. */
    void correctValues();
};

#endif	/* CONFIG_H */

