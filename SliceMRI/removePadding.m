function removePadding(imageStack, filePath)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

if exist(filePath,'dir')
    rmdir(filePath);
end

mkdir(filePath);

for i = 1:size(imageStack,3)
    fprintf('%d of %d\n', i, size(imageStack,3));
    [row,col] = find(imageStack(:,:,i) > 0);
    
    if ~isempty(row) && ~isempty(col) 
        % NOTE: Only works if black pixels are only in padding
        finalImage = imageStack(min(row):max(row), min(col):max(col), i);
        finalImageContrastAdjusted = imadjust(finalImage, [.5;.6], [0;1]);
        filename = fullfile(filePath, sprintf('slice%04d.png',i));
        imwrite(finalImageContrastAdjusted, filename);
    end
end

end

