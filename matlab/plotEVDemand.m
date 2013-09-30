function plotEVDemand(logdir, simInterval)

% NOTE!  This function plots average of ALL vehicles, including those that
% may already be full (and have charge rate of 0).  For more in depth
% analysis of charge rates, see function plotChargeRateStats.m.
    
% Get data
    A = importdata([logdir 'data_demand_EV.csv']);  % Charge rates in W
    B = A.data / 1000;  % Convert to kW
    [numData numEVs] = size(B);
    C = zeros(numData,1); % For calculating average
    colours = hsv(numEVs);

% Plot
    figure('Position', [100 100 800 400]);
    hold on
    grid on
    
    for i=1:numEVs
        plot(B(:,i), 'Color', colours(i,:));
        C = C+B(:,i);
    end
    C = C/numEVs;
    plot(C, 'LineWidth', 4, 'Color', 'k');
    
    setAxes(gca, numData, simInterval);
    xlabel('Time of Day');
    ylabel('Demand (kW)');
    title('EV Demand profiles');
    
    save2pdf([logdir 'results_demand_EV.pdf']);
end

