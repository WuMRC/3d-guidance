function transformedMat = transformMat3D(mat, ang1, ang2, ang3)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

R1 = [cosd(ang1) sind(ang1) 0; -sind(ang1) cosd(ang1) 0; 0 0 1];
R2 = [cosd(ang2) 0 -sind(ang2); 0 1 0; sind(ang2) 0 cosd(ang2)];
R3 = [1 0 0; 0 cosd(ang3) sind(ang3); 0 -sind(ang3) cosd(ang3)];

T = R1 * R2 * R3;

[rowSize, colSize, stackSize] = size(mat);

p1 = [1;1;1];
p2 = [1;colSize;1];
p3 = [rowSize;1;1];
p4 = [rowSize;colSize;1];

tot = [p1 p2 p3 p4 p1 p2 p3 p4];
tot(3,5:8) = stackSize;

transTot = T * tot;


Min = min(transTot');
Max = max(transTot');

Min = round(Min);
Max = round(Max);

rowSizeNew = Max(1) - Min(1);
colSizeNew = Max(2) - Min(2);
stackSizeNew = Max(3) - Min(3);

centerNew = ceil([rowSizeNew / 2; colSizeNew / 2; stackSizeNew / 2]);

transformedMat = zeros(rowSizeNew, colSizeNew, stackSizeNew);
for k = 1:stackSize
    fprintf('%d\n',k);
    for i = 1:rowSize
        mat2trans = [ones(1,colSize) * i; 1:colSize; ones(1,colSize)*k];
        transformed = T * mat2trans;

        for j = 1:size(mat2trans,2)
           row = round(transformed(1,j)) + abs(Min(1)) + 1;
           col = round(transformed(2,j)) + abs(Min(2)) + 1;
           stack = round(transformed(3,j)) + abs(Min(3)) + 1;
           
           if row == 0 || col == 0 || stack == 0
              fprintf('bad');
           end

           transformedMat(row,col,stack) = mat(mat2trans(1,j), mat2trans(2,j), mat2trans(3,j));
        end
    end
    
    %transformedMat(:,:,k) = imadjust(transformedMat(:,:,k),[.5;.55], [0;1]);
end


end

