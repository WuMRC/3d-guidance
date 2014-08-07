function flipImages(sourceFolder)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
destinationFolder = sprintf('%s_flipped',sourceFolder);

if exist(destinationFolder, 'dir')
    rmdir(destinationFolder);
end

mkdir(destinationFolder);

files = dir(sourceFolder);

for i = 3:length(files)
   original = imread(fullfile(sourceFolder,files(i).name));
   flipped = uint8(flipud(original));
   
   imwrite(flipped,fullfile(destinationFolder, sprintf('slice%04d.png',i - 2)));
end

end

