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

#include "Power.h"



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

double Phasor::getAmplitude() {return amplitude;}
double Phasor::getPhase() {return phase;}

void Phasor::set(double a, double p) {
    amplitude = a;
    phase = p*M_PI/180;
}

void Phasor::setRC(double r, double c) {
    phase = atan2(c,r);
    amplitude = sqrt((double)(pow(r,2)+pow(c,2)));
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

Phasor Phasor::minus(Phasor other) {
    Phasor diff;
    double diffReal, diffImag;

    diffReal = real() - other.real();
    diffImag = imag() - other.imag();
    diff.phase = atan2(diffImag,diffReal);
    diff.amplitude = sqrt((double)(pow(diffReal,2)+pow(diffImag,2)));

    return diff;
}

Phasor Phasor::times(Phasor other) {
    Phasor product;

    product.amplitude = amplitude * other.amplitude;
    product.phase = phase + other.phase;

    return product;
}

Phasor Phasor::times(Impedance Z) {
    Phasor other;
    other.setRC(Z.resistance, Z.reactance);
    return this->times(other);
}

Phasor Phasor::dividedBy(Phasor other) {
    Phasor quotient;

    quotient.amplitude = amplitude / other.amplitude;
    quotient.phase = phase - other.phase;

    return quotient;
}

Phasor Phasor::squared() {return times(*this);}

Phasor Phasor::divideByConst(double n) {amplitude /= n; return *this;}

Phasor Phasor::timesConst(double n) {amplitude *= n; return *this;}

void Phasor::addReal(double r) {
    double x = real() + r;
    double y = imag();
    setRC(x,y);
}

void Phasor::addImaginary(double i) {
    double x = real();
    double y = imag() + i;
    setRC(x,y);
}

void Phasor::addAmp(double a) {
    amplitude += a;
}

void Phasor::addPhase(double p) {
    phase += p*M_PI/180;
}

std::string Phasor::toString() {
    std::stringstream ss;
    ss << "RMS " << toRMS() << ", Amp " << amplitude << ", Pha " << phase;
    return ss.str();
}


double power::calculatePhaseUnbalance(Phasor vAB, Phasor vBC, Phasor vCA) {
    Phasor a, a2;
    a.set(1,120);
    a2.set(1,240);
    
    Phasor vMinus = vAB.plus(vBC.times(a2).plus(vCA.times(a)));
    Phasor vPlus = vAB.plus(vBC.times(a).plus(vCA.times(a2)));
    
    /* Optional output for debug
    std::cout << "\n------------------------"
              << "\n         Vab: " << vAB.toString()
              << "\n         Vbc: " << vBC.toString()
              << "\n         Vca: " << vCA.toString()
              << "\n       Vca*a: " << vCA.times(a).toString()
              << "\n      Vbc*a2: " << vBC.times(a2).toString()
              << "\nVbc*a2+Vca*a: " << (vBC.times(a2).plus(vCA.times(a))).toString()
              << "\n          V-: " << vMinus.toString()
              << "\n          V+: " << vPlus.toString()
              << "\n       V-/V+: " << vMinus.dividedBy(vPlus).toString()
              << "\n           %: " << vMinus.getAmplitude() / vPlus.getAmplitude() * 100
              << "\n________________________________________________________\n"
              << std::endl; */
              
    return (vMinus.getAmplitude() / vPlus.getAmplitude() * 100);
}