function plotHHDemand(logdir, simInterval)
    figure
    hold on
    
    A = importdata([logdir 'data_demand_HH.csv']);
    B = A.data / 1000;
    [numData numHouses] = size(B);
    C = zeros(numData,1);
    colours = hsv(numHouses);
    for i=1:numHouses
        plot(B(:,i), 'Color', colours(i,:));
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

