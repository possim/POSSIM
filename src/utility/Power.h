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

#ifndef PHASOR_H
#define	PHASOR_H

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif /*M_PI*/

#include <cstdlib>
#include <iostream>
#include <cmath>
#include <sstream>

/** A single constant impedance load having active and reactive components.  */
struct S_Load {
    /** Demand - Active */
    double P;
    
    /** Demand - Reactive*/
    double Q;
};


/** An impedance, having resistance and reactance */
struct Impedance {
    /** Resistance */
    double resistance;
    
    /** Reactance */
    double reactance;
};

/** A simple Phasor class for straightforward manipulation of phasor quantities. */
class Phasor {

private:
    double amplitude;           // amplitude
    double phase;               // phase in radians
    
public:
    /** Constructor - zero phasor. */
    Phasor();
    
    /** Constructor - non-zero phasor. */
    Phasor(double a, double p);
    
    /** Destructor */
    virtual ~Phasor();
    
    /** Return real part of phasor */
    double real();
    
    /** Return imaginary part of phasor */
    double imag();
    
    /** Return root mean square of phasor */
    double toRMS();
    
    Phasor plus(Phasor other);
    
    Phasor times(Phasor other);
    
    Phasor squared();
    
    Phasor divideByConst(double n);
    
    Phasor timesConst(double n);
    
    std::string toString();
};
    
#endif	/* PHASOR_H */

