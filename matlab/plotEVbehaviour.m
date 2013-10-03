function plotEVbehaviour(logdir, simInterval)

% Get data
    A = importdata([logdir 'data_vehicleLocations.csv']);
    B = A.data;
    [numData numEVs] = size(B);

    
% I.  PLOT AGGREGATED BEHAVIOUR

    figure('Position', [100 100 800 400]);
    hold on
    grid on

    C = zeros(numData,1);
    for i=1:numEVs
        C = C+B(:,i);
    end
    C = C/numEVs * 100;
    plot(C, 'o-', 'MarkerFaceColor', 'b');
    
    setAxes(gca, numData, simInterval);
    ylim([0 102]);
    xlabel('Time of Day');
    ylabel('% Vehicles at home');
    title('EV Behaviour');
    
    save2pdf([logdir 'results_EVbehaviour.pdf']);

    
% II.  PLOT SOME INDIVIDUAL PROFILES

    intervalsPerHour = 60/simInterval;
    startData = numData - 24*intervalsPerHour;
    endData = numData;
    displayInterval = 6;
    tickInterval = displayInterval*intervalsPerHour;
    
    figure
    hold on

    for i=1:min(numEVs,12)
        subplot(3,4,i);
        
        % normalise to 0/1.  Away always 0, but home could be 1 or 2.
        for j=1:numData
            if B(j,i) ~= 0
                B(j,i) = 1;
            end
        end
        plot(B(:,i));
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

