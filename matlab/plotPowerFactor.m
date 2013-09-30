function plotPowerFactor(logdir, simInterval)

% Get data
    A = importdata([logdir 'data_powerFactor.csv']);
    B = A.data;
    C = (B(:,1)+B(:,2)+B(:,3))/3; % average
    [numData numHouses] = size(B);

% Plot
    figure('Position', [50 50 800 600]);
    display = {'-m^', '-gs', '-bo', '-kx'};
    mfc = {'m', 'g', 'b', 'k'};


    % Phases
    subplot(2,1,1);
    hold on
    grid on
    for i=1:3
        plot(B(:,i), display{i}, 'MarkerFaceColor', mfc{i});
    end
    
    % Average
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

    % Neutral
    subplot(2,1,2);
    hold on
    grid on
    plot(B(:,4), display{4}, 'MarkerFaceColor', mfc{4});
    setAxes(gca, numData, simInterval);
    xlabel('Time of Day');
    ylabel('Power Factor');
    title('Power Factor of Neutral');    

    save2pdf([logdir 'results_powerFactor.pdf']);
end


