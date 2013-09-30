function plotTotalDemand(logdir, simInterval)

% Get data
    A = importdata([logdir 'data_demand_Total.csv']);
    B = A.data;
    [numData numVehicles] = size(B);
    
% Plot
    figure('Position', [100 100 800 400]);
    hold on
    grid on
    
    % EV demand background
    area((B(:,1)+B(:,2))/1000, 'FaceColor',[.5 .9 .6],...
             'EdgeColor','k',...
             'LineWidth',1);
         
    % HH demand foreground
    area(B(:,1)/1000, 'FaceColor',[.9 .85 .7],...
               'EdgeColor','k',...
               'LineWidth',1)
           
    legend('EV Demand', 'Household Demand', 2);
    
    setAxes(gca, numData, simInterval);
   
    xlabel('Time of Day');
    ylabel('Demand (kW)');
    title('Total Demand');

    save2pdf([logdir 'results_demand_Total.pdf']);
end

