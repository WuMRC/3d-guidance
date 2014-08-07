function stack = dcm2stack(filePath)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

files = dir(filePath);

initialized = false;
actualIndex = 1;

for i = 1:length(files)
    if files(i).bytes > 0
        info = dicominfo(fullfile(filePath, files(i).name));
        currentIm = dicomread(info);
        currentIm = im2double(currentIm);
        
        if ~initialized
           %stack = mat2cell(length(files) - 2);
           stack = zeros(size(currentIm,1), size(currentIm,2), length(files) - 2);
           initialized = true;
        end
        
        stack(:,:,actualIndex) = currentIm;
        actualIndex = actualIndex + 1;
    end
end

end
