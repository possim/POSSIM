function plotHouseholdV(logdir, simInterval)
    hf = figure;
    set(hf, 'Position', [50 50 600 900]);
    
    A = importdata([logdir 'data_householdV_RMS.csv']);
    B = A.data;
    [numData numHouses] = size(B);
    numHouses = numHouses/3; % 3 cols for each house
    
    C = importdata([logdir 'house_phases.csv']);
    
    phaseA = zeros(numData,1);
    phaseB = zeros(numData,1);
    phaseC = zeros(numData,1);
    namesPhaseA = {};
    namesPhaseB = {};
    namesPhaseC = {};
    numPhaseA = 0;
    numPhaseB = 0;
    numPhaseC = 0;
    
    
    for i=1:numHouses
        if C(i,2)==0
            numPhaseA = numPhaseA + 1;
            phaseA(:,numPhaseA) = B(:,i*3-2);
            namesPhaseA{numPhaseA} = num2str(C(i,1));
        else if C(i,2)==1
            numPhaseB = numPhaseB + 1;
            phaseB(:,numPhaseB) = B(:,i*3-2);
            namesPhaseB{numPhaseB} = num2str(C(i,1));
        else
            numPhaseC = numPhaseC + 1;
            phaseC(:,numPhaseC) = B(:,i*3-2);
            namesPhaseC{numPhaseC} = num2str(C(i,1));
        end
        end
    end
  

    colours = hsv(numPhaseA);
    subplot(4,1,1);   
    hold on
    grid on
    for i=1:numPhaseA
        plot(phaseA(:,i), 'Color', colours(i,:));
    end
    setAxes(gca, numData, simInterval);
    xlabel('Time of Day');
    ylabel('Voltage (V)');
    title('Household Voltages - Phase A');
    %legend(namesPhaseA, 'Location', 'EastOutside', 'Interpreter', 'none');

    colours = hsv(numPhaseB);
    subplot(4,1,2);   
    hold on
    grid on
    for i=1:numPhaseB
        plot(phaseB(:,i), 'Color', colours(i,:));
    end
    setAxes(gca, numData, simInterval);
    xlabel('Time of Day');
    ylabel('Voltage (V)');
    title('Household Voltages - Phase B');
    %legend(housenameB, 'Location', 'EastOutside', 'Interpreter', 'none');

    colours = hsv(numPhaseC);
    subplot(4,1,3);   
    hold on
    grid on
    for i=1:numPhaseC
        plot(phaseC(:,i), 'Color', colours(i,:));
    end
    setAxes(gca, numData, simInterval);
    xlabel('Time of Day');
    ylabel('Voltage (V)');
    title('Household Voltages - Phase C');
    %legend(housenameC, 'Location', 'EastOutside', 'Interpreter', 'none');

    colours = hsv(4);
    subplot(4,1,4);   
    hold on
    grid on
    plot(sum(phaseA')/numPhaseA, 'Color', colours(1,:));
    plot(sum(phaseB')/numPhaseB, 'Color', colours(2,:));
    plot(sum(phaseC')/numPhaseC, 'Color', colours(3,:));
    plot((sum(phaseA')+sum(phaseB')+sum(phaseC'))/numHouses, 'k', 'LineWidth', 2);
    
    setAxes(gca, numData, simInterval);
    xlabel('Time of Day');
    ylabel('Voltage (V)');
    title('Household Voltages - Average for each phase');
    legend('Phase A', 'PhaseB', 'Phase C', 'Average', 'Location', 'EastOutside');


    save2pdf([logdir 'results_householdV_RMS.pdf']);
end

