# CPE593_Fall2022_Project
CPE593 Fall2022 Project

btai2 = Brandon Tai;
Greensweight = Andrew Greensweight;
RoniRahman = Roni Rahman

The two-dimensional bin packing problem is concerned with the optimal containment of some number of smaller rectangles within a large boundary rectangle.
Herein the problem is defined to be a 2D knapsack problem with no rotation where the containment bound is a large rectangle, where the sub-items to be packed are smaller rectangles and all lengths are integer; the objective is to find the best container for all objects which minimizes the difference between the length and width of the boundary rectangle.
The binary tree corner search approach is used to determine the optimal boundary.


------Code--------

The primary file is located in the folder \src and is titled "src.cc" This file contains all of the code necessary to run the program. Src.cc contains classes for the node, binary tree, and tree generator. Within the same directory are text files containing test cases that are read in during main() - these test cases must be within the same folder as src.cc to compile and run the code. 

How to run: 
1. Compile \src\src.cc
2. Run src.cc
