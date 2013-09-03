function plotPhaseUnbalance(logdir, simInterval)
    figure
    hold on
    
    A = importdata([logdir 'data_phaseUnbalance.csv']);
    B = A.data;
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
    ylabel('Voltage Unbalance (%)');
    title('Voltage Unbalance');
    
    save2pdf([logdir 'results_phaseUnbalance.pdf']);
end

