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

#ifndef UTILITY_H
#define	UTILITY_H

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif /*M_PI*/

#include <sstream>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "DateTime.h"
#include "Phasor.h"


namespace utility {
// Please leave the below comment for doxygen documentation
/** \addtogroup Utility 
 *  Various generally useful functions accessible throughout POSSIM. 
 *  @{ 
 */

    /** Utility
      * Calculate difference between two time points (mainly for displaying
      * simulation progress) */
    long timeDiff(boost::posix_time::ptime t1, boost::posix_time::ptime t2);
    
    /** Return time since start in milliseconds */
    long timeSince(boost::posix_time::ptime start);
    
    /** Add leading zeros as required for time output */
    std::string time2string(int n);
    
    /** Display a time (where input is in milliseconds) in a convenient format. */
    std::string timeDisplay(long time);
    
    /** Convert an integer to a string */
    std::string int2string(int i);
    
    /** Convert a string to an integer */
    int string2int(std::string s);
    
    /** Convert a string to a double */
    double string2double(std::string s);
    
    /** Tokenize a string according to a specific delimiter */
    void tokenize(const std::string& str, std::vector<std::string>& tokens, std::string del);
    
    /** Return random ordering of components of a vector, using Fisher-Yates shuffle */
    std::vector<int>::iterator random_unique(std::vector<int>::iterator begin, std::vector<int>::iterator end, size_t num_random);
    
    /** Return random var according to normal distribution */
    double randomNormal(double mean, double sigma, double min, double max);
    
    /** Return random var according to normal distribution, 4-tuple input */
    double randomNormal(double *vars);
    
    /** Return random var according to uniform distribution */
    double randomUniform();
    
    /** Return random var according to uniform distribution scaled by range (min, max) */
    double randomUniform(double min, double max);
    
    /** Find power factor given amplitudes of V and I */
    double calcPowerFactor(double phaseV, double phaseI);

// Please leave the below comment for doxygen documentation
/** @} End of Utility group */

}

#endif	/* UTILITY_H */

