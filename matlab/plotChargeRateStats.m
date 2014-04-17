function plotChargeRateStats(logdir, simInterval)

% Get data
    A = importdata([logdir 'data_demand_EV.csv']);
    CHARGERATES = A.data / 1000;
    [numData numEVs] = size(CHARGERATES);
    
    A = importdata([logdir 'data_vehicleLocations.csv']);
    LOCATIONS = A.data;
    
    A = importdata([logdir 'data_batterySOC.csv']);
    SOC = A.data;

% Determine values of interest
    TotalRate = zeros(numData,1);
    NumCharging = zeros(numData,1);
    MinChargeRate = zeros(numData,1);
    MaxChargeRate = zeros(numData,1);
    StdDev = zeros(numData,1);
    
    for i=1:numData
        TempRateVector = zeros(1);

        MinChargeRate(i) = 100;
        MaxChargeRate(i) = -100;
        
        for j=1:numEVs
            % Should only be charging if home, connected, with SOC<98
            if(LOCATIONS(i,j) == 1  &&  SOC(i,j) < 98)
                NumCharging(i) = NumCharging(i) + 1;
                TotalRate(i) = TotalRate(i) + CHARGERATES(i,j);
                TempRateVector(NumCharging(i)) = CHARGERATES(i,j);
                if(CHARGERATES(i,j) < MinChargeRate(i))
                    MinChargeRate(i) = CHARGERATES(i,j);
                end
                if(CHARGERATES(i,j) > MaxChargeRate(i))
                    MaxChargeRate(i) = CHARGERATES(i,j);
                end
            end
        end
        
        if(NumCharging(i) == 0)
            MinChargeRate(i) = 0;
            MaxChargeRate(i) = 0;
        end
        
        StdDev(i) = std(TempRateVector);
    end
    
% Plot avg rate, max, min, stddev
    figure('Position', [100 100 800 300]);
    hold on
    grid on
    
    % Slight y-shift to prevent plot overlap
    plot(TotalRate ./ NumCharging+0.02, 'b-o', 'MarkerFaceColor', 'b');
    plot(MaxChargeRate+0.03, 'r-^', 'MarkerFaceColor', 'r');
    plot(MinChargeRate, 'g-d', 'MarkerFaceColor', 'g');
    plot(StdDev+0.01, 'm-x', 'MarkerFaceColor', 'm');
    
    setAxes(gca, numData, simInterval);
    xlabel('Time of Day');
    ylabel('Charge rate (kW)');
    title('Vehicle charge rates');
    legend('Average', 'Max', 'Min', 'StdDev', 'Location', 'NorthEastOutside');
    
    save2pdf([logdir 'results_chargeRates.pdf']);
end

