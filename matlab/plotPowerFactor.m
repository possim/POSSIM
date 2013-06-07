function plotPowerFactor(logdir, simInterval)
    hf = figure;
    set(hf, 'Position', [50 50 800 900]);
    
    colours = hsv(4);

    A = importdata([logdir 'data_powerFactor.csv']);
    B = A.data;
    C = (B(:,1)+B(:,2)+B(:,3))/3;
    [numData numHouses] = size(B);
    
    subplot(2,1,1);
    hold on
    grid on
    for i=1:3
        plot(B(:,i), 'Color', colours(i,:));
    end
    plot(C, 'k', 'LineWidth', 2);
    setAxes(gca, numData, simInterval);
    xlabel('Time of Day');
    ylabel('Power Factor');
    title('Power Factor of Individual Phases');   
    hleg = legend('Phase A',...
                  'Phase B',...
                  'Phase C',...
                  'Average');    
    set(hleg, 'Position', [0.8 0.92 0.06 0.02]);

    subplot(2,1,2);
    hold on
    grid on
    plot(B(:,4), 'Color', colours(4,:));
    setAxes(gca, numData, simInterval);
    xlabel('Time of Day');
    ylabel('Power Factor');
    title('Power Factor of Neutral');    

    save2pdf([logdir 'results_powerFactor.pdf']);
end


