function setAxes(handle, numData, simInterval) 

    intervalsPerHour = 60/simInterval;
    startData = numData - 24*intervalsPerHour;
    endData = numData;
    displayInterval = 4;
    tickInterval = displayInterval*intervalsPerHour;
    
    HourTicks = [startData:tickInterval:endData];
    HourLabels = [0:displayInterval:24];

    axis([startData endData 1 2]);
    axis 'auto y';
    set(handle,'XTick',HourTicks);
    set(handle,'XTickLabel',HourLabels);
end