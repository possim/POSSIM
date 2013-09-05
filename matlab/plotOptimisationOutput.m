function plotOptimisationOutput(optdir, simInterval)

% Get data
    B = importdata([optdir 'out.csv']);
    [numData cols] = size(B);
    X = [1:1:numData];
    
%  FIRST:   Plot decision variables, constraints
    figure
    hold on
    grid on
    
    % Plot dec vars and constraints
    [AX,H1,H2] = plotyy(X, B(:,1), X, B(:,2));

    % Set x axis
    linkprop(AX,{'Xlim','XTickLabel','Xtick'}); 
    setAxes(AX, numData, simInterval);

    % Set y axes
    set(get(AX(1),'YLabel'), 'String', 'Num Decision Variables');
    set(get(AX(2),'YLabel'), 'String', 'Num Constraints');
    ylim('auto');
    axes(AX(2));
    ylim('auto');
    title('Decision variables and constraints');
    save2pdf([optdir '../results_optOutput1.pdf']);
    
    
%  SECOND:   Plot fval (opt sum) and exit flag
    figure
    hold on
    grid on
    
    % Plot dec vars and constraints
    [AX2,H3,H4] = plotyy(X, B(:,3), X, B(:,4));

    % Set x axis
    linkprop(AX2,{'Xlim','XTickLabel','Xtick'}); 
    setAxes(AX2, numData, simInterval);

    % Set y axes
    set(get(AX2(1),'YLabel'), 'String', 'Objective Value');
    set(get(AX2(2),'YLabel'), 'String', 'Optimisation Success');
    ylim('auto');
    axes(AX2(2));
    ylim('auto');
    
    lims = get(AX2(1), 'YLim');
    lims = [lims(1):lims(2)/3:lims(2)];
    set(AX2(1), 'YTick', lims)
    set(AX2(1), 'YTickLabel', lims)
    
    title('Optimisation output');
    save2pdf([optdir '../results_optOutput2.pdf']);
end

