function plotPhaseV_Wave(logdir, simInterval)
    

% Get data
    A = importdata([logdir 'data_phaseV.csv']);
    B = A.data;
    [numData numHouses] = size(B);
    
% Plot
    figure('Position', [50 50 800 900]);
    display = {'-m^', '-gs', '-bo', '-kx'};
    mfc = {'m', 'g', 'b', 'k'};
    
    % Voltage magnitudes
    subplot(2,1,1);
    hold on
    grid on
    for i=1:4
        plot(B(:,i*3-1), display{i}, 'MarkerFaceColor', mfc{i});
    end
    setAxes(gca, numData, simInterval);
    xlabel('Time of Day');
    ylabel('Voltage Magnitude (V)');
    title('Phase Voltages - Magnitude');   
    
    % Voltage phase angles
    subplot(2,1,2);
    hold on
    grid on
    for i=1:4
        plot(B(:,i*3), display{i}, 'MarkerFaceColor', mfc{i});
    end
    setAxes(gca, numData, simInterval);
    xlabel('Time of Day');
    ylabel('Voltage Angle (^o)');
    title('Phase Voltages - Angle');   
    
    
    hleg = legend('Phase A',...
                  'Phase B',...
                  'Phase C',...
                  'Neutral');    
    set(hleg, 'Position', [0.8 0.92 0.06 0.02]);

    save2pdf([logdir 'results_phaseV_Wave.pdf']);
end

