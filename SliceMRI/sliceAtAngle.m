function slices = sliceAtAngle(imStack, angle, xCenter, yCenter)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

% Assign variables for size of imStack for easy access
[ySize, xSize, zSize] = size(imStack);

% Ensure values for centerX and centerY are within boundaries of imStack
assert(xCenter >= 1 & xCenter <= xSize);
assert(yCenter >= 1 & yCenter <= ySize);

% Pad images with zeros so they may be centered at (centerX, centerY)
xDiff = round(xSize / 2 - xCenter);
yDiff = round(ySize / 2 - yCenter);

if xDiff < 0
    xStart = 1+ abs(xDiff);
    xEnd = xSize + abs(xDiff);
else
    xStart = 1;
    xEnd = xSize; %- abs(xDiff);
end

if yDiff < 0
    yStart = 1+ abs(yDiff);
    yEnd = ySize + abs(yDiff);
else
    yStart = 1;
    yEnd = ySize; %- abs(yDiff);
end

for i = 1:zSize
%     paddedImage = zeros(xSize + xDiff, ySize + yDiff);
    paddedImage = zeros(ySize + abs(yDiff), xSize + abs(xDiff));
    paddedImage(yStart:yEnd, xStart:xEnd) = squeeze(imStack(:,:,i));
    
    rotatedImage = imrotate(paddedImage, angle);
    if i == 1
       rotatedStack = zeros(size(rotatedImage,1), size(rotatedImage,2), zSize);
    end
    
    rotatedStack(:,:,i) = rotatedImage;
end

nonEmptyIndex = 1;
slices = zeros(size(rotatedStack,3), size(rotatedStack,1), size(rotatedStack,2));
for i = 1:size(rotatedStack,2)
    mat = squeeze(rotatedStack(:,i,:));
    if sum(sum(mat)) > 0
        slices(:,:, nonEmptyIndex) = mat';
        nonEmptyIndex = nonEmptyIndex + 1;
    end
end
    


end

