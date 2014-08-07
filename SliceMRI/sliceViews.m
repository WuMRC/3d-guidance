function [axial,coronal,sagittal] = sliceViews(filePath)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

files = dir(filePath);

currentImage = imread(fullfile(filePath,files(3).name));
imageStack = zeros(size(currentImage,1), size(currentImage,2), length(files) - 2);
imageStack(:,:,1) = currentImage;

for i = 4:length(files)
   currentImage = imread(fullfile(filePath,files(i).name));
   imageStack(:,:,i-2) = currentImage;
end

axial = imageStack;
coronal = zeros(size(imageStack,3), size(imageStack,2), size(imageStack,1));
sagittal = zeros(size(imageStack,3), size(imageStack,1), size(imageStack,2));

for i = 1:size(imageStack,1)
   coronal(:,:,i) = squeeze(imageStack(i,:,:))';
   sagittal(:,:,i) = squeeze(imageStack(:,i,:))';
end

end

