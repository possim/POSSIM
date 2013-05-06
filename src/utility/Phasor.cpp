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

#include "Phasor.h"



Phasor::Phasor() {
    amplitude = 0;
    phase = 0;
}

Phasor::Phasor(double a, double p) {
    amplitude = a;
    phase = p*M_PI/180;
}
    
Phasor::~Phasor() {
}
    
double Phasor::real() {return amplitude*cos(phase);}
double Phasor::imag() {return amplitude*sin(phase);}
double Phasor::toRMS() {return amplitude/sqrt(double(2));}

Phasor Phasor::plus(Phasor other) {
    Phasor sum;
    double sumReal, sumImag;

    sumReal = real() + other.real();
    sumImag = imag() + other.imag();
    sum.phase = atan2(sumImag,sumReal);
    sum.amplitude = sqrt((double)(pow(sumReal,2)+pow(sumImag,2)));

    return sum;
}

Phasor Phasor::times(Phasor other) {
    Phasor product;

    product.amplitude = amplitude * other.amplitude;
    product.phase = phase + other.phase;

    return product;
}

Phasor Phasor::squared() {return times(*this);}

Phasor Phasor::divideByConst(double n) {amplitude /= n; return *this;}

Phasor Phasor::timesConst(double n) {amplitude *= n; return *this;}

std::string Phasor::toString() {
    std::stringstream ss;
    ss << "RMS " << toRMS() << ", Amp " << amplitude << ", Pha " << phase;
    return ss.str();
}