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



#include "MatlabInterface.h"



/*   CONSTRUCTOR, DESTRUCTOR   */
    
MatlabInterface::MatlabInterface(std::string modelname) {
    // START Matlab
    std::cout << " - Trying to start MATLAB ...";
    std::cout.flush();
    if (!(eng = engOpen(""))) {
        std::cout << "Fail!" << std::endl;
        return;
    }
    std::cout << "................................... OK" << std::endl;
    
    // Set working directory in matlab, add path to matlab directory
    boost::filesystem::path working_path( boost::filesystem::current_path() );
	setDir(working_path.string());
    
    // Load model
    loadModel(modelname);
}

MatlabInterface::~MatlabInterface() {
    mxDestroyArray(result);
    engClose(eng);
}


/*    UTILITY    */

void MatlabInterface::setDir(std::string dir) {
    ss.str("");
    ss << "cd " << dir << "; "
       << "addpath('" << dir << "/matlab');";
    engEvalString(eng, ss.str().c_str());
}

void MatlabInterface::loadModel(std::string model) {
    // Save model directory and name
    modelDir = model.substr(0,model.find_last_of('/')+1);
    modelName = model.substr(model.find_last_of('/')+1);

    ss.str("");
    ss << "systemHandle = load_system" << "('" << model << "')";
    
    std::cout << " - Loading model " << model << " ...";
    std::cout.flush();
    
    engEvalString(eng, ss.str().c_str());
    
    std::cout << ".......................... OK" << std::endl;
}

void MatlabInterface::runSim() {
    ss.str("");
    ss << "sim" << "('" << modelDir << modelName << "');";
    engEvalString(eng, ss.str().c_str()); 
}

double MatlabInterface::getVar(std::string var) {
    result = engGetVariable(eng, var.c_str());
    return *((double *)mxGetData(result));
}

void MatlabInterface::setVar(std::string component, double value, std::string var) {
    ss.str("");
    ss << "set_param('" << component << "','" << var << "','" << value << "');";
    engEvalString(eng, ss.str().c_str());
}

int MatlabInterface::getNumHouses() {
    return 0;
}

std::vector <std::string> MatlabInterface::getHouseNames() {
    mwSize bufferLength;
    mxArray *componentName;
    char* currHouseName;
    std::string currHouseNameStr;
    int numHouses;
    std::vector <std::string> houseNames;

    ss.str("");
    ss << "A = find_system('" << modelName << "', 'FollowLinks', 'on', 'LookUnderMasks', 'on', 'name', 'House');"; //'regexp', 'on',
    //std::cout << ss.str() << std::endl;
    //std::cout.flush();
    engEvalString(eng, ss.str().c_str());
    // Should now have all house names stored in A
    
    ss.str("");
    ss << "[num x] = size(A);";
    engEvalString(eng, ss.str().c_str());
    result = engGetVariable(eng, "num");
    numHouses = (int)(*((double *)mxGetData(result)));
    //std::cout<<"FOUND " << numHouses << " HOUSES" << std::endl;
    
    for(int i=1; i<=numHouses; i++) {
        ss.str("");
        ss << "curr = A(" << i << ");";
        engEvalString(eng, ss.str().c_str());
        result = engGetVariable(eng, "curr");
        componentName = mxGetCell(result, 0);
        
        bufferLength = mxGetNumberOfElements(componentName) + 1;
        currHouseName = (char *)mxCalloc(bufferLength, sizeof(char));
        mxGetString(componentName, currHouseName, bufferLength);
                
        currHouseNameStr = std::string(currHouseName).c_str();
        houseNames.push_back(currHouseNameStr);
    }
    return houseNames;
}

void MatlabInterface::addVehicle(Vehicle vehicle) {
     std::string vehicleName = vehicle.getName();
     
     ss.str("");
     int lastindex = vehicleName.find_last_of("/");
     std::string subsysRoot = vehicleName.substr(0,lastindex);
     std::string houseName = vehicleName.substr(0,lastindex).append("/House");
     
     ss << "component = find_system('" << houseName << "');"
        << "set_param('" << subsysRoot << "', 'LinkStatus', 'inactive');"
        << "EV = add_block('" << houseName << "','" << vehicleName << "');"
        << "HH = component{1};"
        << "pos = get_param(HH,'Position') + 60;"
        << "set_param(EV, 'Position', pos);"
        << "set_param(EV, 'NamePlacement', 'normal');"
        << "portsEV = get_param(EV, 'PortHandles');"
        << "portsHH = get_param(HH, 'PortHandles');"
        << "add_line('" << subsysRoot << "', portsEV.LConn, portsHH.LConn);"
        << "add_line('" << subsysRoot << "', portsEV.RConn, portsHH.RConn);";
     engEvalString(eng, ss.str().c_str());
}


void MatlabInterface::setDemand(std::string component, double active, double inductive, double capacitive) {
    setVar(component, active+0.001, "ActivePower");  // At least one value must be non zero
    setVar(component, inductive, "InductivePower");
    setVar(component, capacitive, "CapacitivePower");
}

void MatlabInterface::printModel(std::string targetDir) {
     std::cout << " - Printing grid model to file ... ";
     std::cout.flush();    
     ss.str("");
     ss << "saveas(systemHandle, '" << targetDir << "matlabModel.pdf','pdf');";
     engEvalString(eng, ss.str().c_str());
     std::cout << "OK" << std::endl;
}

void MatlabInterface::getOutputs(double phaseV[12], double phaseI[12], double eolV[12], std::map<int, Household> &households) {
    std::string houseName, meterHandle, vmBase;
    std::vector<std::string> tokens;
    int lastIndex;
    
    std::string phaseNames_V[4] = {"Voltage Phase A", "Voltage Phase B", "Voltage Phase C", "Voltage Neutral"};
    std::string phaseNames_I[4] = {"Current Phase A", "Current Phase B", "Current Phase C", "Current Neutral"};
    
    // Get Phase Data
    for(int phasenum = 0; phasenum<4; phasenum++) {
        
        // VOLTAGES TX
        
        // get RMS
        ss.str("");
        ss << "value = logsout.('" << phaseNames_V[phasenum] << "').('SL_RMS (discrete)1').Data;";
        engEvalString(eng, ss.str().c_str());
        phaseV[phasenum*3] = getVar("value");

        // get Magnitude
        ss.str("");
        ss << "value = logsout.('" << phaseNames_V[phasenum] << "').('SL_Discrete Fourier1').Data";
        engEvalString(eng, ss.str().c_str());
        phaseV[phasenum*3 + 1] = getVar("value");

        // get Phase
        ss.str("");
        ss << "value = logsout.('" << phaseNames_V[phasenum] << "').('SL_Discrete Fourier2').Data";
        engEvalString(eng, ss.str().c_str());
        phaseV[phasenum*3 + 2] = getVar("value");

        
        // CURRENTS TX
        
        // get RMS
        ss.str("");
        ss << "value = logsout.('" << phaseNames_I[phasenum] << "').('SL_RMS (discrete)1').Data;";
        engEvalString(eng, ss.str().c_str());
        phaseI[phasenum*3] = getVar("value");

        // get Magnitude
        ss.str("");
        ss << "value = logsout.('" << phaseNames_I[phasenum] << "').('SL_Discrete Fourier1').Data";
        engEvalString(eng, ss.str().c_str());
        phaseI[phasenum*3 + 1] = getVar("value");

        // get Phase
        ss.str("");
        ss << "value = logsout.('" << phaseNames_I[phasenum] << "').('SL_Discrete Fourier2').Data";
        engEvalString(eng, ss.str().c_str());
        phaseI[phasenum*3 + 2] = getVar("value");

        
        // VOLTAGES EOL
        
        // get RMS
        ss.str("");
        ss << "value = logsout.('Distribution Network').('" << phaseNames_V[phasenum] << "').('SL_RMS (discrete)1').Data;";
        engEvalString(eng, ss.str().c_str());
        eolV[phasenum*3] = getVar("value");

        // get Magnitude
        ss.str("");
        ss << "value = logsout.('Distribution Network').('" << phaseNames_V[phasenum] << "').('SL_Discrete Fourier1').Data";
        engEvalString(eng, ss.str().c_str());
        eolV[phasenum*3 + 1] = getVar("value");

        // get Phase
        ss.str("");
        ss << "value = logsout.('Distribution Network').('" << phaseNames_V[phasenum] << "').('SL_Discrete Fourier2').Data";
        engEvalString(eng, ss.str().c_str());
        eolV[phasenum*3 + 2] = getVar("value");

    }

    // Get Individual Household data
    for(std::map<int,Household>::iterator it = households.begin(); it != households.end(); ++it) {
        houseName = it->second.getName();

        tokens.clear();
        utility::tokenize(houseName, tokens, "/");
        
        ss.str("");
        ss << "value = logsout.";
        for(int i=1; i<tokens.size()-1; i++)
            ss << "('" << tokens.at(i) << "').";
        ss << "('Voltage Measurement').";
        vmBase = ss.str();
        
        ss << "('SL_RMS (discrete)1').Data";
        engEvalString(eng, ss.str().c_str());
        it->second.V_RMS = getVar("value");
        
        ss.str("");
        ss << vmBase << "('SL_Discrete Fourier1').Data";
        engEvalString(eng, ss.str().c_str());
        it->second.V_Mag = getVar("value");
        
        ss.str("");
        ss << vmBase << "('SL_Discrete Fourier2').Data";
        engEvalString(eng, ss.str().c_str());
        it->second.V_Pha = getVar("value");   
    }
}



void MatlabInterface::generateReport(std::string dir, int month, bool isWeekday, int simInterval) {
    // Generate household demand plot
    ss.str("");
    ss << "plotHHDemand('" << dir << "', " << month << ", " << isWeekday << ", " << simInterval << ");";
    engEvalString(eng, ss.str().c_str());

    // Generate EV behaviour plot
    ss.str("");
    ss << "plotEVbehaviour('" << dir << "', " << simInterval << ");";
    engEvalString(eng, ss.str().c_str());

    // Generate EV demand plot
    ss.str("");
    ss << "plotEVDemand('" << dir << "', " << simInterval << ");";
    engEvalString(eng, ss.str().c_str());

    // Generate Total demand plot
    ss.str("");
    ss << "plotTotalDemand('" << dir << "', " << simInterval << ");";
    engEvalString(eng, ss.str().c_str());

    // Generate spot price plot
    ss.str("");
    ss << "plotSpotPrice('" << dir << "', " << simInterval << ");";
    engEvalString(eng, ss.str().c_str());

    // Generate phase current and voltage plot RMS
    ss.str("");
    ss << "plotPhaseVI_RMS('" << dir << "', " << simInterval << ");";
    engEvalString(eng, ss.str().c_str());

    // Generate individual household voltage plot
    ss.str("");
    ss << "plotHouseholdV('" << dir << "', " << simInterval << ");";
    engEvalString(eng, ss.str().c_str());

    // Generate phase voltage plot waveform
    ss.str("");
    ss << "plotPhaseV_Wave('" << dir << "', " << simInterval << ");";
    engEvalString(eng, ss.str().c_str());

    // Generate phase current plot waveform
    ss.str("");
    ss << "plotPhaseI_Wave('" << dir << "', " << simInterval << ");";
    engEvalString(eng, ss.str().c_str());

    // Generate phase power factors plot
    ss.str("");
    ss << "plotPowerFactor('" << dir << "', " << simInterval << ");";
    engEvalString(eng, ss.str().c_str());

    // Generate battery SOC plot
    ss.str("");
    ss << "plotBatterySOC('" << dir << "', " << simInterval << ");";
    engEvalString(eng, ss.str().c_str());

}