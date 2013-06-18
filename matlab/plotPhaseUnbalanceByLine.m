function plotPhaseUnbalanceByLine(logdir, simInterval)
    hf = figure;
    set(hf, 'Position', [50 50 800 600]);
    hold on
    
    A = importdata([logdir 'data_phaseUnbalanceByLine.csv']);
    B = A.data;
    [numData numLines] = size(B);
    C = zeros(numData,1);
    colours = hsv(numLines);
    for i=1:numLines
        plot(B(:,i), 'Color', colours(i,:));
        C = C+B(:,i);
    end
    C = C/numLines;
    %pS = plot(C, 'LineWidth', 4, 'Color', 'k');
    
    setAxes(gca, numData, simInterval);
    xlabel('Time of Day');
    ylabel('Voltage Unbalance (%)');
    title('Voltage Unbalance');
    
    legend(A.textdata{1,2:numLines}, 'Location', 'EastOutside');
    
    save2pdf([logdir 'results_phaseUnbalanceByLine.pdf']);
end

