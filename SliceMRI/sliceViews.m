function [axial,coronal,sagittal] = sliceViews(imStack)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

axial = zeros(size(imStack,1), size(imStack,2), size(imStack,3));
coronal = zeros(size(imStack,3), size(imStack,2), size(imStack,1));
sagittal = zeros(size(imStack,3), size(imStack,1), size(imStack,2));

for i = 1:max(size(imStack))
    
   if i < size(axial,3)
      axial(:,:,i) = imadjust(imStack(:,:,i), [.5;.6], [0;1]); 
   end

   if i < size(coronal,3)
       coronal(:,:,i) = imadjust(squeeze(imStack(i,:,:))', [.5;.6],[0;1]);
   end

   if i < size(sagittal,3)
       sagittal(:,:,i) = imadjust(squeeze(imStack(:,i,:))', [.5;.6],[0;1]);
   end
end
end

