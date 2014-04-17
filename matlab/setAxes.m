function setAxes(handle, numData, simInterval) 

    intervalsPerHour = 60/simInterval;
    startData = numData - 24*intervalsPerHour;
    endData = numData;
    displayInterval = 4;  % show every 4 hours on x axis
    tickInterval = displayInterval*intervalsPerHour;
    
    HourTicks = [startData:tickInterval:endData];
    
    % Old version: 0:24 hours
    % HourLabels = [0:displayInterval:24];

    % New version:  format with minutes to make clear it's time
    % 6:6 hours (from 6am to 6am next day)
    % HourLabels = {'6:00', '10:00', '14:00', '18:00', '22:00', '2:00'};
    % 8:8 hours (from 8am to 8am next day)
    % HourLabels = {'8:00', '12:00', '16:00', '20:00', '0:00', '4:00'};
    % 0:24 hours (from midnight to midnight)
    HourLabels = {'0:00', '4:00', '8:00', '12:00', '16:00', '20:00'};
    
    axis([startData endData 1 2]);
    axis 'auto y';
    set(handle,'XTick',HourTicks);
    set(handle,'XTickLabel',HourLabels);
end