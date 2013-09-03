function setloads(filename)

allData = importdata(filename);
names = allData.textdata;
loads = allData.data;

for i=1:size(names)
    set_param(names{i}, 'ActivePower', num2str(loads(i,1)));
    set_param(names{i}, 'InductivePower', num2str(loads(i,2)));
    set_param(names{i}, 'CapacitivePower', num2str(loads(i,3)));
end

end
