function plotHHDemand(logdir, simInterval)

% Get data
    A = importdata([logdir 'data_demand_HH.csv']);
    B = A.data / 1000; % Demand in kW
    [numData numHouses] = size(B);
    C = zeros(numData,1); % For finding average
    colours = hsv(numHouses);
    
% Plot
    figure('Position', [100 100 800 400]);
    hold on
    grid on
    
    for i=1:numHouses
        plot(B(:,i), '-o', 'Color', colours(i,:), 'MarkerFaceColor', colours(i,:));
        C = C+B(:,i);
    end
    
    C = C/numHouses;
    pS = plot(C, 'LineWidth', 4, 'Color', 'k');
    
    setAxes(gca, numData, simInterval);
    xlabel('Time of Day');
    ylabel('Demand (kW)');
    title('Household Demand profiles');
    
    save2pdf([logdir 'results_demand_HH.pdf']);
end

