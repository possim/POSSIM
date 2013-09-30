function plotBatterySOC(logdir, simInterval)

    % Input battery SOC data
    in_1 = importdata([logdir 'data_batterySOC.csv']);
    SOC = in_1.data;
    NAMES = in_1.textdata;
    
    % Input home / away data
    in_2 = importdata([logdir 'data_vehicleLocations.csv']);
    HOME = in_2.data;

    figure('Position', [100 100 800 400]);
    hold on
    grid on
    
    [numData numVehicles] = size(SOC);
    colours = hsv(numVehicles);

    vehiclename = {};
    for i=1:numVehicles
        if HOME(1,i)==0
            home = 0;
        else
            home = 1;
        end
        
        startIndex = 1;
        for j=2:numData
            if(HOME(j-1,i) == 0 && HOME(j,i) ~= 0  || ...
               HOME(j-1,i) ~= 0 && HOME(j,i) == 0 || ...
               j == numData)

               if(home)
                   lh = plot([startIndex:j], SOC(startIndex:j,i), '-', 'Color', colours(i,:));
                   legendHandles(i) = lh;
               else
                   plot([startIndex j], [SOC(startIndex,i) SOC(j,i)], '--o', 'Color', colours(i,:), 'MarkerFaceColor', colours(i,:));
               end
               
               startIndex = j;
               home = 1-home;
            end
        end
        vehiclename{i} = NAMES{1,i+1};
    end
    
    setAxes(gca, numData, simInterval);
    
    % uncomment below to see axes across whole simulation period, not just
    % last 24 hours
    % axis([1 numData 1 2]);
    % axis 'auto y';
    
    xlabel('Time of Day');
    ylabel('Battery SOC (%)');
    title('EV Batteries: State of Charge');

    % Only show legend if small number of EVs (messy otherwise)
    if numVehicles <= 20
        legend(legendHandles, vehiclename, 'Location', 'EastOutside', 'Interpreter', 'none');
    end

    save2pdf([logdir 'results_batterySOC.pdf']);
end

