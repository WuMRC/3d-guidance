function imageMatrixCycle(imStack)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

[~,~,stackSize] = size(imStack);

currentSlice = round(stackSize / 2);

str = input(sprintf('CURRENT SLICE: %d\nENTER CHANGE VALUE: ', currentSlice), 's');

while ~strcmp(str,'q') && currentSlice > 1
    delta = str2double(str);
    
    if ~isnan(delta)
        if currentSlice + delta < 1
            currentSlice = 1;
        elseif currentSlice + delta > stackSize
            currentSlice = stackSize;
        else
            currentSlice = currentSlice + delta;
        end
    end
    imshow(imStack(:,:,currentSlice));
    clc;
    str = input(sprintf('CURRENT SLICE: %d\nENTER CHANGE VALUE: ', currentSlice), 's');
end

end

