phaseNames = {'Phase A', 'Phase B', 'Phase C', 'Neutral'};
numHouses = size(householdNames);
out = zeros(36+3*numHouses,1);

for i=0:3
    out(3*i+1) = logsout.(['Voltage ' phaseNames{i+1}]).('SL_RMS (discrete)1').Data;
    out(3*i+2) = logsout.(['Voltage ' phaseNames{i+1}]).('SL_Discrete Fourier1').Data;
    out(3*i+3) = logsout.(['Voltage ' phaseNames{i+1}]).('SL_Discrete Fourier2').Data;
end

for i=0:3
    out(12+3*i+1) = logsout.(['Current ' phaseNames{i+1}]).('SL_RMS (discrete)1').Data;
    out(12+3*i+2) = logsout.(['Current ' phaseNames{i+1}]).('SL_Discrete Fourier1').Data;
    out(12+3*i+3) = logsout.(['Current ' phaseNames{i+1}]).('SL_Discrete Fourier2').Data;
end

for i=0:3
    out(24+3*i+1) = logsout.('Distribution Network').(['Voltage ' phaseNames{i+1}]).('SL_RMS (discrete)1').Data;
    out(24+3*i+2) = logsout.('Distribution Network').(['Voltage ' phaseNames{i+1}]).('SL_Discrete Fourier1').Data;
    out(24+3*i+3) = logsout.('Distribution Network').(['Voltage ' phaseNames{i+1}]).('SL_Discrete Fourier2').Data;
end

for i=0:numHouses-1
    out(36+3*i+1) = logsout.('Distribution Network').(householdNames{i+1}).('Voltage Measurement').('SL_RMS (discrete)1').Data;
    out(36+3*i+2) = logsout.('Distribution Network').(householdNames{i+1}).('Voltage Measurement').('SL_Discrete Fourier1').Data;
    out(36+3*i+3) = logsout.('Distribution Network').(householdNames{i+1}).('Voltage Measurement').('SL_Discrete Fourier2').Data;
end

save('temp_output.txt', 'out', '-ascii');

