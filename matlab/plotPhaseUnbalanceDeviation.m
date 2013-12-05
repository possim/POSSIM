function plotPhaseUnbalanceDeviation(logdir, simInterval)

% Get data
    A = importdata([logdir 'data_phaseUnbalanceDeviation.csv']);
    B = A.data;
    [numData numCols] = size(B);

% Plot
    figure
    hold on
    grid on
    
    plot(B, '-o', 'MarkerFaceColor', 'b');
    
    setAxes(gca, numData, simInterval);
    xlabel('Time of Day');
    ylabel('Phase Unbalance (% deviation from average)');
    title('Phase Unbalance');

    save2pdf([logdir 'results_phaseUnbalanceDeviation.pdf']);
end

