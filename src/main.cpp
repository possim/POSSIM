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




#include <stdio.h>

#include "simulator/Simulator.h"
#include "simulator/Config.h"
#include "../cmake/POSSIMConfig.h"

// Please leave the below comment for doxygen documentation
/** \addtogroup main 
 *  This is the main file run on startup.  It takes care of console interaction
 * such as input arguments and their flags.  Run ./possim -h from console to 
 * find out more about available flags (or check the possim_config.xml file).
 *  @{ 
 */


/** Print usage of possim command in console, including available flags and their defaults */
void print_usage(Config* config, char** argv)
{
    std::cout << std::endl << "USAGE:  " << *argv << " [options]" << std::endl << std::endl
              << "Where [options] can be:" << std::endl;
    config->printOptions();
    std::cout << std::endl;
    exit(-1);
} 

/** Parse arguments provided at command line */
void parse_args(Config* config, int argc, char** argv)
{
    for (int i=1; i<argc; i+=2)
        if(strcmp(argv[i],"-h")==0 || strcmp(argv[i],"-?")==0 )
            print_usage(config, argv);
        else if(i+1 < argc)
            config->setConfigVarByFlag(argv[i], argv[i+1]);
}


/** main.cpp:  Show version, start simulator */
int main(int argc, char ** argv) 
{
    Config *config = new Config();
    parse_args(config, argc, argv);

    std::cout << std::endl << "********************************************************" << std::endl
                 << "                      POSSIM v" << POSSIM_VERSION_MAJOR << "." << POSSIM_VERSION_MINOR << std::endl
                 << "********************************************************" << std::endl << std::endl;

    if(config->getBool("showdebug")) {
        std::cout << "Using arguments" << std::endl;
        config->printAll();
        std::cout << std::endl;
    }
    
    Simulator sim(config);
    sim.run();
    
    return 0;
}

// Please leave the below comment for doxygen documentation
/** @} End of main group */
