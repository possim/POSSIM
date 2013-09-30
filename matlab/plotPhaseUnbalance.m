function plotPhaseUnbalance(logdir, simInterval)
    figure('Position', [50 50 800 400]);
    hold on
    grid on
    
    A = importdata([logdir 'data_phaseUnbalance.csv']);
    B = A.data;
    A.textdata
    [numData numPoles] = size(B);

    colours = hsv(numPoles);
    for i=1:numPoles
        plot(B(:,i), 'Color', colours(i,:));
    end
    
    setAxes(gca, numData, simInterval);
    xlabel('Time of Day');
    ylabel('Voltage Unbalance (%)');
    title('Voltage Unbalance');
    
    legend(A.textdata{1,2:numPoles+1}, 'Location', 'EastOutside');
    
    save2pdf([logdir 'results_phaseUnbalance.pdf']);
end


