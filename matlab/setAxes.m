function setAxes(handle, numData, simInterval) 

    intervalsPerHour = 60/simInterval;
    startData = numData - 24*intervalsPerHour;
    endData = numData;
    displayInterval = 4;  % show every 4 hours on x axis
    tickInterval = displayInterval*intervalsPerHour;
    
    HourTicks = [startData:tickInterval:endData];
    
    % Old version: 0:24 hours
    % HourLabels = [0:displayInterval:24];

    % New version:  12:12 hours (from noon to noon)
    HourLabels = {'8:00', '12:00', '16:00', '20:00', '0:00', '4:00', '8:00'};
    
    axis([startData endData 1 2]);
    axis 'auto y';
    set(handle,'XTick',HourTicks);
    set(handle,'XTickLabel',HourLabels);
end