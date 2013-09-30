figure
hold on
colours = hsv(15);
for i=1:15
A = importdata([int2str(i) '.csv']);
B = A.data;
plot(B(:,1), 'Color', colours(i,:));
end