function stack = dcm2stack(filePath)
% Creates a 3-dimensional matrix containing the image data from dicoms
% found in filePath. Assumes each image is stored in a seperate dicom.
%   INPUT:
%       filePath: string representing file path to folder containing dicoms
%
%   OUTPUT:
%       stack: 3-dimensional matrix containing pixel values representing
%              the voxel data of dicoms stored in filePath

% Obtain an array of structs for each file in filePath
files = dir(filePath);

% As we do not know the size of images in filePath before reading one in,
% we cannot initialize stack until we have read one. This boolean keeps
% track whether or not stack has been initialized
initialized = false;

% Matlab's 'dir' command returns two empty files '.' and '..'. These empty
% (0 bytes) and thus will throw an error if we attempt to access them. In
% the following loop, we will bypass them and use actualIndex to keep track
% of the true index we wish to access.
actualIndex = 1;

% Loop through structs in files
for i = 1:length(files)
    % Only access files for with data (not '.' and '..')
    if files(i).bytes > 0
        % Read dicom data
        info = dicominfo(fullfile(filePath, files(i).name));
        currentIm = dicomread(info);
        
        % Convert dicom pixel data to doubles
        currentIm = im2double(currentIm);
        
        % If stack has not yet been initialized , do so now
        if ~initialized
           stack = zeros(size(currentIm,1), size(currentIm,2), length(files) - 2);
           initialized = true;
        end
        
        % Place pixel data plane in stack
        stack(:,:,actualIndex) = currentIm;
        
        % Increment actualIndex only if we have placed a pixel data plane
        actualIndex = actualIndex + 1;
    end
end

end
