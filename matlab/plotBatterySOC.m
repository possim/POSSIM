function plotBatterySOC(logdir, simInterval)
    figure
    hold on
    grid on
    
    A = importdata([logdir 'data_batterySOC.csv']);
    B = A.data;
    C = A.textdata;

    [numData numVehicles] = size(B);
    colours = hsv(numVehicles);

    vehiclename = {};
    for i=1:numVehicles
        plot(B(:,i), 'Color', colours(i,:));
        vehiclename{i} = C{1,i+1};
    end
    
    setAxes(gca, numData, simInterval);
    axis([1 numData 1 2]);
    axis 'auto y';
    xlabel('Time of Day');
    ylabel('Battery SOC (%)');
    title('EV Batteries: State of Charge');
    legend(vehiclename, 'Location', 'EastOutside', 'Interpreter', 'none');

    save2pdf([logdir 'results_batterySOC.pdf']);
end

