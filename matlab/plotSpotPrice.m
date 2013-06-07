function plotSpotPrice(logdir, simInterval)
    figure
    hold on
    
    A = importdata([logdir 'data_spotPrice.csv']);
    B = A.data;
    [numData numHouses] = size(B);

    plot(B);
    
    setAxes(gca, numData, simInterval);
    xlabel('Time of Day');
    ylabel('Spot Price ($/MWh)');
    title('Spot Price');

    save2pdf([logdir 'results_spotPrice.pdf']);
end

