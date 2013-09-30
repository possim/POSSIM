function plotPhaseVI_RMS(logdir, simInterval)

% I.  VOLTAGES

% Get data
    A = importdata([logdir 'data_phaseV.csv']);
    B = A.data;
    [numData numHouses] = size(B);

% Plot
    figure('Position', [50 50 600 900]);
    display = {'-m^', '-gs', '-bo', '-kx'};
    mfc = {'m', 'g', 'b', 'k'};

    subplot(12,4,[5:16]);
    hold on
    grid on
    for i=1:3
        plot(B(:,i*3-2), display{i}, 'MarkerFaceColor', mfc{i});
    end
    setAxes(gca, numData, simInterval);
    xlabel('Time of Day');
    ylabel('Voltage (V)');
    title('Phase Voltages');
    
    subplot(12,4,[21:28]);
    hold on
    grid on
    plot(B(:,10), display{4}, 'MarkerFaceColor', mfc{4});
    setAxes(gca, numData, simInterval);
    xlabel('Time of Day');
    ylabel('Voltage (V)');
    title('Neutral Voltage');
    
    
% II.  CURRENTS

% Get data
    A = importdata([logdir 'data_phaseI.csv']);
    B = A.data;

% Plot
    subplot(12,4,[33:48]);
    hold on
    grid on
    for i=1:4
        plot(B(:,i*3-2), display{i}, 'MarkerFaceColor', mfc{i});
    end
    setAxes(gca, numData, simInterval);
    xlabel('Time of Day');
    ylabel('Current (A)');
    title('Phase and Neutral Currents');
 
    hleg = legend('Phase A',...
                  'Phase B',...
                  'Phase C',...
                  'Neutral');    
    set(hleg, 'Position', [0.8 0.92 0.06 0.02]);

    save2pdf([logdir 'results_phaseVI_RMS.pdf']);
end

