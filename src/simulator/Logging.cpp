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

#include "Logging.h"

Logging::Logging() {
    
}

void Logging::initialise(Config* config, GridModel gridmodel) {
    std::ofstream outfile;
    
    std::cout << " - Creating log files ...";
    std::cout.flush();    
    
    // Create directory
    createDir();
    
    // Create file with all parameters for this simulation
    file_parameters = directory + "sim_parameters.txt";
    outfile.open(file_parameters.c_str());
    outfile << config->toString() << std::endl;
    outfile.close();
    
    // Create household demand log file
    file_demandHH = directory + "data_demand_HH.csv";
    outfile.open(file_demandHH.c_str());
    outfile << "Time, ";
    for(std::map<int,Household>::iterator it = gridmodel.households.begin(); it != gridmodel.households.end(); ++it)
        outfile << it->second.getName() << ", ";
    outfile << std::endl;
    outfile.close();

    // Create vehicle demand log file
    file_demandEV = directory + "data_demand_EV.csv";
    outfile.open(file_demandEV.c_str());
    outfile << "Time, ";
    for(std::map<int,Vehicle>::iterator it = gridmodel.vehicles.begin(); it != gridmodel.vehicles.end(); ++it)
        outfile << it->second.getName() << " (" << it->second.getNMI() << ") " << it->second.getProfileRef() << ", ";
    outfile << std::endl;
    outfile.close();
    
    // Create total demand log file
    file_demandTotal = directory + "data_demand_Total.csv";
    outfile.open(file_demandTotal.c_str());
    outfile << "Time, Demand_HH, Demand_EV, Demand_Total" << std::endl;
    outfile.close();
    
    // Create vehicle location log file
    file_locationEV = directory + "data_vehicleLocations.csv";
    outfile.open(file_locationEV.c_str());
    outfile << "Time, ";
    for(std::map<int,Vehicle>::iterator it = gridmodel.vehicles.begin(); it != gridmodel.vehicles.end(); ++it)
        outfile << it->second.getName() << " (" << it->second.getNMI() << "), ";
    outfile << std::endl;
    outfile.close();
    
    // Create spot price log file
    file_spotPrice = directory + "data_spotPrice.csv";
    outfile.open(file_spotPrice.c_str());
    outfile << "Time, Spot Price" << std::endl;
    outfile.close();
    
    // Create phase and neutral voltage log file
    file_phaseV = directory + "data_phaseV.csv";
    outfile.open(file_phaseV.c_str());
    outfile << "Time, "
            << "V_PhaseA_RMS, V_PhaseA_Mag, V_PhaseA_Pha, "
            << "V_PhaseB_RMS, V_PhaseB_Mag, V_PhaseB_Pha, "
            << "V_PhaseC_RMS, V_PhaseC_Mag, V_PhaseC_Pha, "
            << "V_Neutral_RMS, V_Neutral_Mag, V_Neutral_Pha" 
            << std::endl;
    outfile.close();
    
    // Create phase and neutral current log file
    file_phaseI = directory + "data_phaseI.csv";
    outfile.open(file_phaseI.c_str());
    outfile << "Time, "
            << "I_PhaseA_RMS, I_PhaseA_Mag, I_PhaseA_Pha, "
            << "I_PhaseB_RMS, I_PhaseB_Mag, I_PhaseB_Pha, "
            << "I_PhaseC_RMS, I_PhaseC_Mag, I_PhaseC_Pha, "
            << "I_Neutral_RMS, I_Neutral_Mag, I_Neutral_Pha" 
            << std::endl;
    outfile.close();
    
    // Create end-of-line voltage log file
    file_eolV = directory + "data_eolV.csv";
    outfile.open(file_eolV.c_str());
    outfile << "Time, "
            << "V_PhaseA_RMS, V_PhaseA_Mag, V_PhaseA_Pha, "
            << "V_PhaseB_RMS, V_PhaseB_Mag, V_PhaseB_Pha, "
            << "V_PhaseC_RMS, V_PhaseC_Mag, V_PhaseC_Pha, "
            << "V_Neutral_RMS, V_Neutral_Mag, V_Neutral_Pha" 
            << std::endl;
    outfile.close();
    
    // Create power factor log file
    file_powerFactor = directory + "data_powerFactor.csv";
    outfile.open(file_powerFactor.c_str());
    outfile << "Time, PhaseA, PhaseB, PhaseC, Neutral" << std::endl;
    outfile.close();

    // Create household voltage log file
    file_householdV = directory + "data_householdV_RMS.csv";
    outfile.open(file_householdV.c_str());
    outfile << "Time, ";
    for(std::map<int,Household>::iterator it = gridmodel.households.begin(); it != gridmodel.households.end(); ++it)
        outfile << it->second.getName() << " (House " << it->second.getNMI() << "), "
                << "House " << it->second.getNMI() << " Magnitude, "
                << "House " << it->second.getNMI() << " Phase, ";
    outfile << std::endl;
    outfile.close();
    
    // Create battery SOC file
    file_batterySOC = directory + "data_batterySOC.csv";
    outfile.open(file_batterySOC.c_str());
    outfile << "Time, ";
    for(std::map<int,Vehicle>::iterator it = gridmodel.vehicles.begin(); it != gridmodel.vehicles.end(); ++it)
        outfile << it->second.getNMI() << " (Vehicle " << it->second.getNMI() << "), ";
    outfile << std::endl;
    outfile.close();
    
    // Create battery SOC file
    file_probchargeEV = directory + "data_probChargeEV.csv";
    outfile.open(file_probchargeEV.c_str());
    outfile << "Time, ";
    for(std::map<int,Vehicle>::iterator it = gridmodel.vehicles.begin(); it != gridmodel.vehicles.end(); ++it)
        outfile << gridmodel.households.at(it->second.getNMI()).getName() << " (" << it->second.getNMI() << "), , , , , , , ";
    outfile << std::endl;
    outfile << "Time, ";
    for(std::map<int,Vehicle>::iterator it = gridmodel.vehicles.begin(); it != gridmodel.vehicles.end(); ++it)
        outfile << "V_RMS, V_Valley, L, SOC, N, P, SwitchedOn, ";
    outfile << std::endl;
    outfile.close();
    

    std::cout << "....................................... OK" << std::endl;
}

Logging::~Logging() {
}

std::string Logging::getDir() {
    return directory;
}

void Logging::createDir() {
    time_t rawtime;
    struct tm *t;
    struct stat st;
    std::string dirName;
    
    boost::filesystem::path logDir("logs");
    boost::filesystem::create_directory(logDir);
    
    // Create logs directory
    //if(stat("logs",&st) != 0)
    //    system("mkdir logs");

    // Create directory for this date
    time(&rawtime);
    t = localtime(&rawtime);
    dirName = "logs/" + utility::time2string(t->tm_year+1900) + "_" 
                      + utility::time2string(t->tm_mon+1) + "_"
                      + utility::time2string(t->tm_mday);
    //if(stat(dirName.c_str(),&st) != 0)
    //    system((std::string("mkdir ")+dirName).c_str());
    boost::filesystem::path dateDir(dirName);
    boost::filesystem::create_directory(dateDir);
    
    // Create directory for this run
    dirName = dirName + "/" + utility::time2string(t->tm_hour) + "_"
                            + utility::time2string(t->tm_min) + "_"
                            + utility::time2string(t->tm_sec);

    // Make sure directory is unique even if time matches a previously created dir
//    if(stat(dirName.c_str(),&st) != 0) {
    boost::filesystem::path timeDir(dirName);

    if(!boost::filesystem::exists(timeDir)) {
        //system((std::string("mkdir ")+dirName).c_str());
        boost::filesystem::create_directory(timeDir);
        directory = dirName + "/";
    }
    else {
        int index = 2;
        std::string newDirName = dirName + "_" + utility::int2string(index);
        
        timeDir = newDirName;
        while(boost::filesystem::exists(timeDir)) {
            index++;
            timeDir = dirName + "_" + utility::int2string(index);
        }
        //system((std::string("mkdir ")+newDirName).c_str());
        boost::filesystem::create_directory(timeDir);
        directory = newDirName + "/";
    }
}


void Logging::update(DateTime currtime, GridModel gridModel, ChargingBaseClass *charger, SpotPrice spotPrice) {
    std::map<int,Household> households = gridModel.households;
    std::map<int,Vehicle> vehicles = gridModel.vehicles;
    std::ofstream outfile;
    double powerEV=0, powerHH=0;

    outfile.open(file_demandHH.c_str(), std::ofstream::app);
    outfile << currtime.toString() << ", ";
    for(std::map<int,Household>::iterator it = households.begin(); it != households.end(); ++it) {
        outfile << it->second.activePower << ", ";
        powerHH += it->second.activePower;
    }
    outfile << std::endl;
    outfile.close();
    
    outfile.open(file_demandEV.c_str(), std::ofstream::app);
    outfile << currtime.toString() << ", ";
    for(std::map<int,Vehicle>::iterator it = vehicles.begin(); it != vehicles.end(); ++it) {
        outfile << it->second.activePower << ", ";
        powerEV += it->second.activePower;
    }
    outfile << std::endl;
    outfile.close();
    
    outfile.open(file_demandTotal.c_str(), std::ofstream::app);
    outfile << currtime.toString() << ", " << powerHH << ", " << powerEV << ", " << (powerHH+powerEV) << std::endl;
    outfile.close();
    
    outfile.open(file_locationEV.c_str(), std::ofstream::app);
    outfile << currtime.toString() << ", ";
    for(std::map<int,Vehicle>::iterator it = vehicles.begin(); it != vehicles.end(); ++it)
        outfile << (int)(it->second.location) << ", ";
    outfile << std::endl;
    outfile.close();
    
    outfile.open(file_spotPrice.c_str(), std::ofstream::app);
    outfile << currtime.toString() << ", " << spotPrice.price << std::endl;
    outfile.close();

    outfile.open(file_phaseV.c_str(), std::ofstream::app);
    outfile << currtime.toString() << ", ";
    for(int i=0; i<12; i++)
        outfile << gridModel.phaseV[i] << ", ";
    outfile << std::endl;
    outfile.close();
    
    outfile.open(file_phaseI.c_str(), std::ofstream::app);
    outfile << currtime.toString() << ", ";
    for(int i=0; i<12; i++)
        outfile << gridModel.phaseI[i] << ", ";
    outfile << std::endl;
    outfile.close();
    
    outfile.open(file_eolV.c_str(), std::ofstream::app);
    outfile << currtime.toString() << ", ";
    for(int i=0; i<12; i++)
        outfile << gridModel.eolV[i] << ", ";
    outfile << std::endl;
    outfile.close();
    
    outfile.open(file_powerFactor.c_str(), std::ofstream::app);
    outfile << currtime.toString() << ", "
            << utility::calcPowerFactor(gridModel.phaseV[2], gridModel.phaseI[2]) << ", "
            << utility::calcPowerFactor(gridModel.phaseV[5], gridModel.phaseI[5]) << ", "
            << utility::calcPowerFactor(gridModel.phaseV[8], gridModel.phaseI[8]) << ", "
            << utility::calcPowerFactor(gridModel.phaseV[11], gridModel.phaseI[11])
            << std::endl;
    outfile.close();

    outfile.open(file_householdV.c_str(), std::ofstream::app);
    outfile << currtime.toString() << ", ";
    for(std::map<int,Household>::iterator it = households.begin(); it != households.end(); ++it) 
        outfile << it->second.V_RMS << ", " << it->second.V_Mag << ", " << it->second.V_Pha << ", ";
    outfile << std::endl;
    outfile.close();
    
    outfile.open(file_batterySOC.c_str(), std::ofstream::app);
    outfile << currtime.toString() << ", ";
    for(std::map<int,Vehicle>::iterator it = vehicles.begin(); it != vehicles.end(); ++it) 
        outfile << it->second.getSOC()  << ", ";
    outfile << std::endl;
    outfile.close();
        
    outfile.open(file_probchargeEV.c_str(), std::ofstream::app);
    outfile << currtime.toString() << ", ";
    for(std::map<int,Vehicle>::iterator it = vehicles.begin(); it != vehicles.end(); ++it) 
        outfile << gridModel.households.at(it->second.getNMI()).V_RMS << ", "
                << gridModel.households.at(it->second.getNMI()).V_valley << ", "
                << it->second.L << ", " 
                << it->second.getSOC() << ", "
                << it->second.N << ", "
                << it->second.P << ", "
                << it->second.switchon << ", ";
    outfile << std::endl;
    outfile.close();
    
}
