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

#define M_PI 3.14159265358979323846264338327950288

//#include <time.h>
//#include <sys/time.h>
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

class phasor {
public:
    double amplitude;           // amplitude
    double phase;               // phase in radians
    
    phasor() {
        amplitude = 0;
        phase = 0;
    }
    
    phasor(double a, double p) {
        amplitude = a;
        phase = p*M_PI/180;
    }
    
    double real() {return amplitude*cos(phase);}
    double imag() {return amplitude*sin(phase);}
    double toRMS() {return amplitude/sqrt(double(2));}
    
    phasor plus(phasor other) {
        phasor sum;
        double sumReal, sumImag;
        
        sumReal = real() + other.real();
        sumImag = imag() + other.imag();
        sum.phase = atan2(sumImag,sumReal);
        sum.amplitude = sqrt((double)(pow(sumReal,2)+pow(sumImag,2)));
        
        return sum;
    }
    
    phasor times(phasor other) {
        phasor product;
        
        product.amplitude = amplitude * other.amplitude;
        product.phase = phase + other.phase;
        
        return product;
    }
    
    phasor squared() {return times(*this);}
    
    phasor divideByConst(double n) {amplitude /= n; return *this;}
    
    phasor timesConst(double n) {amplitude *= n; return *this;}
    
    std::string toString() {
        std::stringstream ss;
        ss << "RMS " << toRMS() << ", Amp " << amplitude << ", Pha " << phase;
        return ss.str();
    }
};

namespace utility {
    int timediff(boost::posix_time::ptime t1, boost::posix_time::ptime t2);
    int timeElapsed(boost::posix_time::ptime start);
    std::string timeDisplay(long time);
    std::string int2string(int i);
    int string2int(std::string s);
    double string2double(std::string s);
    void tokenize(const std::string& str, std::vector<std::string>& tokens, std::string del);
    std::vector<int>::iterator random_unique(std::vector<int>::iterator begin, std::vector<int>::iterator end, size_t num_random);
    std::string time2string(int n);
    double randomNormal(double mean, double sigma, double min, double max);
    double randomNormal(double *vars);
    double randomUniform();
    double randomUniform(double min, double max);
    double calcPowerFactor(double phaseV, double phaseI);
}

//template<class bidiiter> bidiiter random_unique(bidiiter begin, bidiiter end, size_t num_random);

#endif	/* UTILITY_H */

