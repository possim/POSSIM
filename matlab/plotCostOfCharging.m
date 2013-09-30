function plotCostOfCharging(logdir, simInterval)

% Get data
    A = importdata([logdir 'data_spotPrice.csv']);
    PRICE = A.data;  % Historical spot price data in $/MWh

    A = importdata([logdir 'data_demand_EV.csv']);
    CHARGERATES = A.data;  % Vehicle charge rates in W
    [numData numEVs] = size(CHARGERATES);
    
    period = simInterval / 60;  % get size of each interval in hours
    

% Determine values of interest
    for i=1:numData
        TotalRate = sum(CHARGERATES(i,:));
        
        % Remember to convert from W to MW
        IntervalCost(i) = TotalRate / 1000000 * PRICE(i) * period;
        if i==1
            CumulativeCost(i) = IntervalCost(i);
        else
            CumulativeCost(i) = CumulativeCost(i-1) + IntervalCost(i);
        end
    end
    
%  Plot
    %for plot, readjust cumulative to be zero prior to last 24 hours
    CumulativeCost = CumulativeCost - CumulativeCost(numData-24*(60/simInterval)-1);

    figure('Position', [100 100 800 400]);

    % First, plot spot price
    subplot(3,1,1);
    hold on
    grid on
    plot(PRICE, '-sb', 'MarkerFaceColor', 'b');
    setAxes(gca, numData, simInterval);
    ylabel('Spot Price ($/MWh)');
    
    % Second, plot interval and cumulative price
    subplot(3,1,[2 3]);
    hold on
    grid on
    plot(IntervalCost, '-or', 'MarkerFaceColor', 'r');
    plot(CumulativeCost, '-^g', 'MarkerFaceColor', 'g');
    setAxes(gca, numData, simInterval);
    xlabel('Time of Day');
    ylabel('Cost of charging all vehicles ($)');
    
    legend('Interval cost', 'Cumulative cost', 2);
  
    save2pdf([logdir 'results_cost.pdf']);
    
    TotalCostOfCharging = CumulativeCost(numData)
end

