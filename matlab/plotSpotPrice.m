function plotSpotPrice(logdir, simInterval)

% Get data
    A = importdata([logdir 'data_spotPrice.csv']);
    B = A.data;
    [numData numHouses] = size(B);

% Plot
    figure
    hold on
    grid on
    
    plot(B, '-o', 'MarkerFaceColor', 'b');
    
    setAxes(gca, numData, simInterval);
    xlabel('Time of Day');
    ylabel('Spot Price ($/MWh)');
    title('Spot Price');

    save2pdf([logdir 'results_spotPrice.pdf']);
end

