function plotEVbehaviour(logdir, simInterval)
    A = importdata([logdir 'data_vehicleLocations.csv']);
    B = A.data;
    [numData numEVs] = size(B);

simInterval
    figure
    hold on
    grid on

    C = zeros(numData,1);
    for i=1:numEVs
        C = C+B(:,i);
    end
    C = C/numEVs * 100;
    plot(C, 'LineWidth', 4, 'Color', 'k');
    
    setAxes(gca, numData, simInterval);
    xlabel('Time of Day');
    ylabel('% Vehicles at home');
    title('EV Behaviour');
    
    save2pdf([logdir 'results_EVbehaviour.pdf']);

    
    figure
    hold on
    colours = hsv(12);
    intervalsPerHour = 60/simInterval;
    startData = numData - 24*intervalsPerHour
    endData = numData
    displayInterval = 6
    tickInterval = displayInterval*intervalsPerHour
    for i=1:min(numEVs,12)
        subplot(3,4,i);
        plot(B(:,i), 'Color', colours(i,:));
        currax = gca;
        axis([startData endData -0.1 1.1]);
        set(currax,'YTick',[0 1]);
        set(currax,'YTickLabel',[{'Away'} {'Home'}]);
        set(currax,'XTick',[startData:tickInterval:endData]);
        set(currax,'XTickLabel',[0:displayInterval:24]);
        xlabel('Time of Day');
    end

    save2pdf([logdir 'results_EVprofiles.pdf']);

end

