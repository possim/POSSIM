function plotEVDemand(logdir, simInterval)
    figure
    hold on
    
    A = importdata([logdir 'data_demand_EV.csv']);
    B = A.data / 1000;
    [numData numEVs] = size(B);
    C = zeros(numData,1);
    colours = hsv(numEVs);
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

