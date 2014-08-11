function [axial,coronal,sagittal] = sliceViews(imStack)
% Slices perpendicular view of a matrix through the center of the matrix
%   INPUT:
%       imStack: 3-dimensional matrix containing pixel data for images
%
%   OUTPUT:
%       axial: 3-dimensional matrix containing pixel data for planes sliced
%              along the axial view. This corresponds to slicing along
%              dimension 3, which is Matlab's default.
%       coronal: 3-dimensional matrix containing pixel data for planes sliced
%                along the axial view. This corresponds to slicing along
%                dimension 1
%       sagittal: 3-dimensional matrix containing pixel data for planes sliced
%                 along the axial view. This corresponds to slicing along
%                 dimension 2

% Initialize output matrices
axial = zeros(size(imStack,1), size(imStack,2), size(imStack,3));
coronal = zeros(size(imStack,3), size(imStack,2), size(imStack,1));
sagittal = zeros(size(imStack,3), size(imStack,1), size(imStack,2));

% Loop through the maximum size, slicing each view.
% Use imadjust to make the image is easily seen
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

