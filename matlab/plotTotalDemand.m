function plotTotalDemand(logdir, simInterval)
    figure
    hold on
    
    A = importdata([logdir 'data_demand_Total.csv']);
    B = A.data;
    [numData numVehicles] = size(B);
    
    %plot(B(:,1), 'r');
    %plot(B(:,2), 'g');
    %plot(B(:,3), 'k');
    %h = area([B(:,1) B(:,2)]);
    area((B(:,1)+B(:,2))/1000, 'FaceColor',[.5 .9 .6],...
             'EdgeColor','k',...
             'LineWidth',1);
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

