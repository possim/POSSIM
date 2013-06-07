function plotPhaseI_Wave(logdir, simInterval)
    hf = figure;
    set(hf, 'Position', [50 50 800 900]);
    
    colours = hsv(4);

    A = importdata([logdir 'data_phaseI.csv']);
    B = A.data;
    [numData numHouses] = size(B);
    
    subplot(2,1,1);
    hold on
    grid on
    for i=1:4
        plot(B(:,i*3-1), 'Color', colours(i,:));
    end
    setAxes(gca, numData, simInterval);
    xlabel('Time of Day');
    ylabel('Current Magnitude (I)');
    title('Phase Currents - Magnitude');   
    
    subplot(2,1,2);
    hold on
    grid on
    for i=1:4
        plot(B(:,i*3), 'Color', colours(i,:));
    end
    setAxes(gca, numData, simInterval);
    xlabel('Time of Day');
    ylabel('Current Phase Shift');
    title('Phase Current - Phase Shift');   
    
    
    hleg = legend('Phase A',...
                  'Phase B',...
                  'Phase C',...
                  'Neutral');    
    set(hleg, 'Position', [0.8 0.92 0.06 0.02]);

    save2pdf([logdir 'results_phaseI_Wave.pdf']);
end


