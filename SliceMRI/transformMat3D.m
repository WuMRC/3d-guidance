function transformedMat = transformMat3D(mat, xAng, yAng, zAng, xCenter, yCenter, zCenter)
% Applies rotation and translation transformations to a specified 3D matrix
%   INPUT:
%       mat: 3-dimensional matrix to be transformed
%       xAng: angle in degrees that mat will be rotated about the x-axis
%       yAng: angle in degrees that mat will be rotated about the y-axis
%       zAng: angle in degrees that mat will be rotated about the z-axis
%       xCenter: x-coordinate of center of rotation
%       yCenter: y-coordinate of center of rotation
%       zCenter: z-coordinate of center of rotation
%
%   OUTPUT:
%       transformedMat: 3-dimensional matrix containing the values of
%                       mat, transformed by rotation and translation

% Set up translation and roation matrices
T1 = [1 0 0 round(xCenter); 0 1 0 round(yCenter); 0 0 1 round(zCenter); 0 0 0 1];
T2 = [1 0 0 -round(xCenter); 0 1 0 -round(yCenter); 0 0 1 -round(zCenter); 0 0 0 1];
R1 = [cosd(zAng) -sind(zAng) 0 0; sind(zAng) cosd(zAng) 0 0; 0 0 1 0; 0 0 0 1];
R2 = [cosd(yAng) 0 sind(yAng) 0; 0 1 0 0; -sind(yAng) 0 cosd(yAng) 0; 0 0 0 1];
R3 = [1 0 0 0; 0 cosd(xAng) -sind(xAng) 0; 0 sind(xAng) cosd(xAng) 0; 0 0 0 1];

% Combine individual transformations into one overlying transformation
T = T2 * R1 * R2 * R3 * T1;

% Set up matrix of 8-corners of mat to find boundaries for transformedMat
[rowSize, colSize, stackSize] = size(mat);

p1 = [1;1;1];
p2 = [1;colSize;1];
p3 = [rowSize;1;1];
p4 = [rowSize;colSize;1];

corners = [p1 p2 p3 p4 p1 p2 p3 p4; ones(1,8)];
corners(3,5:8) = stackSize;

% Apply T to 8-corners of mat
transCorners = T * corners;

% Use the minimum and maximum values of transformed corners to determine
% the size of transformedCoords
Min = min(transCorners, [], 2);
Max = max(transCorners, [], 2);

Min = round(Min);
Max = round(Max);

rowSizeNew = Max(1) - Min(1);
colSizeNew = Max(2) - Min(2);
stackSizeNew = Max(3) - Min(3);

% Initialize transformedMat to be full of zeros
transformedMat = zeros(rowSizeNew, colSizeNew, stackSizeNew);

% Loop through each plane on dimension 3
for k = 1:stackSize
    fprintf('%d\n',k);
    
    % Loop through each vector on dimension 1
    for i = 1:rowSize
        
        % Set up the 4 x colSize matrix of indices in the current vector
        mat2trans = [ones(1,colSize) * i; 1:colSize; ones(1,colSize)*k; ones(1,colSize)];
        
        % Transform these indices
        transformed = T * mat2trans;

        % Loop through each point along dimension 2 on current vector
        for j = 1:size(mat2trans,2)
           % Determine the indices at which data will be mapped on
           % transformedMat
           row = round(transformed(1,j)) + abs(Min(1)) + 1;
           col = round(transformed(2,j)) + abs(Min(2)) + 1;
           stack = round(transformed(3,j)) + abs(Min(3)) + 1;
           
           if row == 0 || col == 0 || stack == 0
              fprintf('bad');
           end

           % Set values from mat to corresponding indices of transformedMat
           transformedMat(row,col,stack) = mat(mat2trans(1,j), mat2trans(2,j), mat2trans(3,j));
        end
    end
end


end

