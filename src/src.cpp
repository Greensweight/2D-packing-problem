// CPE593 Final Project
// Andrew Greensweight, Brandon Tai, Roni Rahman


//Note: When executing the program, ensure the test case text files called out in lines 580 through 644 are in the same directory


// Primary references for code and inspiration:
//https://github.com/liu-fei/texture-atlas-generator
// https://codereview.stackexchange.com/questions/225015/binary-tree-algorithm-for-2d-bin-packing
// https://codeincomplete.com/articles/bin-packing/
//=====================


#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>




//===============================
// Node Struct
// Node is a node of a binary tree
// Each Node represents a rectangular area on a fixed board
//===============================
struct Node
{
    //! @brief Constructor For Creating A New Node
    //! @param x The x Coordinate Of The Top-Left Point Of The Rectangle
    //! @param y The Y Coordinate Of The Top-Left Point Of The Rectangle
    //! @param width The Width Of The Rectangle
    //! @param height The Height Of The Rectangle
    Node(int x, int y, int width, int height)
        : x(x), y(y), width(width), height(height), isUsed(false),
        rectID(-1), rightChild(nullptr), downChild(nullptr)
    {
    };


    Node(int x, int y, int width, int height, int rectID)
        : x(x), y(y), width(width), height(height), isUsed(false),
        rectID(rectID), rightChild(nullptr), downChild(nullptr)
    {
    };


    // the top-left coordinate of the rectangle
    int x;
    int y;

    // the dimension of the rectangle
    int width;
    int height;

    // is this Node used or not
    bool isUsed;

    int rectID;

    // after placing an block on the top left corner of an empty board
    // the remaining space will be split into 2 rectangles by the    
    // horizontal line where the rectangle's bottom line is
    // ---------------------
    // |  rect | rightChild |
    // |--------------------     <== split by the rectangle's bottom line
    // |                    |
    // |   downChild        |
    // |                    |
    // |                    |
    // ---------------------
    Node* rightChild;
    Node* downChild;
};


//==============================================================================
//! BinaryTreeAlgorithm Class
//==============================================================================
class BinaryTreeAlgorithm
{
    public:
    //==========================================================================
    //! @brief Starts A New Packing Process for a fixed outer container (or frame) of the given dimension 
    //! @param frameWidth The frame's width
    //! @param frameHeight The frame's height
    //==========================================================================
    void Init(const int frameWidth, const int frameHeight)
    {
        // initiate the root node which represents the initiate whole empty frame rectangle
        // with top-left coordinate (0,0) and dimension frameWidth, frameHeight
        iRootNode = new Node(0, 0, frameWidth, frameHeight); 

    }




    //==========================================================================
    //! @brief Inserts The rectangle in the binary tree
    //! @param rectWidth The rectangle's Width
    //! @param rectHeight The rectangle's Height
    //! @return The Pointer to the Node that stores the rectangle 
    //!         or nullptr if it didn't fit in the Binary Tree
    //==========================================================================
    Node* Insert(const int rectWidth, const int rectHeight) const
    {
        return Insert(iRootNode, rectWidth, rectHeight);
    }   


    //====================================================================
    //! @brief SplitNode happens every time after placing a rectangle 
    //         on the top left corner of an empty rectangle
    //! @param aNode The Node where the rectangle just inserted  
    //! @param rectWidth The rectangle's Width
    //! @param rectHeight The rectangle Height
    //! @param rectID The corresponding index of the rectangle in sortedRectList is used as rectID
    //====================================================================
    void SplitNode(Node* aNode, const int rectWidth, const int rectHeight, const int aRectID)
    {
        aNode->rightChild = new Node(aNode->x + rectWidth, aNode->y, aNode->width - rectWidth, rectHeight);
        aNode->downChild = new Node(aNode->x, aNode->y + rectHeight, aNode->width, aNode->height - rectHeight);

        aNode->isUsed = true;
        aNode->rectID = aRectID;

        std::cout << "Inserting a block with width: " << rectWidth <<
        " and height: " << rectHeight << " at point x: " << aNode->x << " y: " << aNode->y << std::endl;
    }

    //========================================================================================
    //! @brief Get the rootNode
    //========================================================================================
    Node* rootNode()
    {
        return iRootNode;
    };





//========================================================================================
//! @brief GrowRight Grows Canvas On Right Side
//! @param aRectWidth The Rectangles's Width
//! @param aRectHeight The Rectangle's Height
//! @param aRectID The Corresponding Index Of The Rectangle In iSortedRectList Is Used As aRectID
//! @return The Pointer To The Node That Stores The Newly Added Rectangle
//========================================================================================

Node* GrowRight(int aRectWidth, int aRectHeight, const int aRectID)
    {
    Node* newRoot = new Node(0, 0, iRootNode->width + aRectWidth, iRootNode->height);
    newRoot->isUsed = true;

    // for new rectangle
    newRoot->rightChild = new Node(iRootNode->width, 0, aRectWidth, iRootNode->height);
    newRoot->downChild = iRootNode;

    iRootNode = newRoot;

    // insert the new rectangle in the new root node
    if (Node* node = Insert(iRootNode, aRectWidth, aRectHeight))
        {
        SplitNode(node, aRectWidth, aRectHeight, aRectID);
        return node;
        }
    else
        return nullptr;
}


//========================================================================================
//! @brief GrowDown Grows Canvas On Down Side
//! @param aRectWidth The Rectangle's Width
//! @param aRectHeight The Rectangle's Height
//! @param aRectID The Corresponding Index Of The Rectangle In iSortedRectList Is Used As aRectID
//! @return The Pointer To The Node That Stores The Newly Added Rectangle
//========================================================================================
Node* GrowDown(int aRectWidth, int aRectHeight, const int aRectID)
{
    Node* newRoot = new Node(0, 0, iRootNode->width, iRootNode->height + aRectHeight);
    newRoot->isUsed = true;

    newRoot->rightChild = iRootNode;
    newRoot->downChild = new Node(0, iRootNode->height, iRootNode->width, aRectHeight);

    iRootNode = newRoot;

    if (Node* node = Insert(iRootNode, aRectWidth, aRectHeight))
        {
        SplitNode(node, aRectWidth, aRectHeight, aRectID);
        return node;
        }
    else
        return nullptr;
}

//========================================================================================
//! @brief GrowContainer Provides An Algorithm Which Decides Where The Canvas Should Expend,
//         On Its Right Side by Call GrowRight Method Or On Its Down Side by Call GrowDown Method
//! @param aRectWidth The Rect Width
//! @param aRectHeight The Rect Height
//! @param aRectID The Corresponding Index Of The rectangle In iSortedRectList Is Used As aRectID
//========================================================================================
    Node* GrowContainer(const int aRectWidth, const int aRectHeight, const int aRectID)
    {
        bool canGrowRight = (aRectHeight <= iRootNode->height);
        bool canGrowDown = (aRectWidth <= iRootNode->width);

        bool shouldGrowRight = canGrowRight && (iRootNode->height >= (iRootNode->width + aRectWidth));
        bool shouldGrowDown = canGrowDown && (iRootNode->width >= (iRootNode->height + aRectHeight));

        if (shouldGrowRight)
            return GrowRight(aRectWidth, aRectHeight, aRectID);
        else if (shouldGrowDown)
            return GrowDown(aRectWidth, aRectHeight, aRectID);
        else if (canGrowRight)
            return GrowRight(aRectWidth, aRectHeight, aRectID);
        else if (canGrowDown)
            return GrowDown(aRectWidth, aRectHeight, aRectID);
        else
            return nullptr;
    }


    private:
    //====================================================================
    //! @brief Inserts a rectangle in the subtree rooted at the given Node
    //! @param aNode The given Node where to Insert the rectangle
    //! @param rectWidth The rectangle Width
    //! @param rectHeight The rectangle Height
    //! @return The pointer to the Node that stores the newly added rectangle
    //!         or nullptr if it didn't fit in the Binary Tree
    //====================================================================
    Node* Insert(Node* aNode, const int rectWidth, const int rectHeight) const
    {
        if (aNode->isUsed)
            {
            // recursively try aNode's children branchs
            if (Node* here = Insert(aNode->rightChild, rectWidth, rectHeight))
                return here;
            else if (Node* there = Insert(aNode->downChild, rectWidth, rectHeight))
                return there;
            else
                return nullptr;
            }
        // the rectangle fits in this node
        else if (rectWidth <= aNode->width && rectHeight <= aNode->height)
            return aNode;
        else    // the rectangle can not fit in this node
            return nullptr;
    }

    private:
    Node* iRootNode;
};



//==============================================================================
//! TreeGenerator Class
//==============================================================================
class TreeGenerator
{
    public:
    //==========================================================================
    //! @brief Constructor
    //! @param aRectList A list of all of the rectangles you want to pack
    //=========================================================================
    TreeGenerator(const std::vector<Node> aRectList)
        : iRectList(aRectList), iPackingAlgorithm(new BinaryTreeAlgorithm)
    {
    };

    //==========================================================================
    //! @brief Destructor
    // Comment: This needs work. Could use smart pointers throughout program to 
    //          remove the need for this destructor
    //==========================================================================
    ~TreeGenerator()
    {
    delete iPackingAlgorithm;
    iPackingAlgorithm = nullptr;

    for (auto rect : iSortedRectList)
        {
        delete[] rect.rightChild;
        delete[] rect.downChild;
        rect.rightChild = nullptr;
        rect.downChild = nullptr;
        }
    };

    //==============================================================================
    //! @brief Run TreeGenerator
    //==============================================================================
    void Run()
    {
        Packing();

     
        //printRootToLeafPaths(iPackingAlgorithm->rootNode()); // Should start with the true root of the Binary Tree
        
        //testing printing the longest path only
        std::vector<Node> output = longestPath(iPackingAlgorithm->rootNode());
        Node finalContainer = output.back();
        std::cout << "Final Container width: " << finalContainer.width << " height: " << finalContainer.height << std::endl;
    }



    
    //===============================================================================
    //! @brief Function to check if a given node is a leaf node or not
    //! @param node 
    // Ref: https://www.techiedelight.com/print-all-paths-from-root-to-leaf-nodes-binary-tree/ 
    //================================================================================
    bool isLeaf(Node* node) 
    {
        return (node->downChild == nullptr && node->rightChild == nullptr);
    }



    //================================================================================
    //! @brief Recursive function to find paths from the root node to every leaf node
    //! @param node The current node of interest
    //! @param path pat from the root node to every leaf node
    // Ref: https://www.techiedelight.com/print-all-paths-from-root-to-leaf-nodes-binary-tree/ 
    //======================================================================================
    void printRootToLeafPaths(Node* node, std::vector<Node> &path)
    {
        // base case
        if (node == nullptr) {
            std::cout << "Found nullptr at the node, returning" << std::endl;
            return;
        }
    
        // include the current node to the path
        path.push_back(*node);
    
        // if a leaf node is found, print the path
        if (isLeaf(node))
        {
            for (Node node: path) {
                std::cout << "ID: " << node.rectID << " " << "Width: " << node.width 
                << " " << "Height: " << node.height << " " << "isUsed: " << node.isUsed << " " 
                << "x: " << node.x << " " << "y: " << node.y << std::endl;
            }
            std::cout << std::endl;
            // return;
        }
    
        // recur for the left and right subtree
        std::cout << "Recursing to downChild" << std::endl;
        printRootToLeafPaths(node->downChild, path);
        std::cout << "Recursing to rightChild" << std::endl;
        printRootToLeafPaths(node->rightChild, path);
    
        // backtrack: remove the current node after the left, and right subtree are done
        std::cout << "Coming back up a level" << std::endl;
        path.pop_back();
    }
    

    //===============================================================================================
    //! @brief The main function to print paths from the root node to every leaf node
    //! @param node root node
    // Ref: https://www.techiedelight.com/print-all-paths-from-root-to-leaf-nodes-binary-tree/ 
    //================================================================================================
    void printRootToLeafPaths(Node* node)
    {
        // vector to store root-to-leaf path
        std::vector<Node> path;
    
        printRootToLeafPaths(node, path);
    }
    



    //====================================================================
    //! @brief Print longest path in binary tree
    // Ref: https://www.geeksforgeeks.org/print-the-longest-path-from-root-to-leaf-in-a-binary-tree/
    //===========================================================================
    std::vector<Node> longestPath(Node* root)
    {
    
        // If root is null means there
        // is no binary tree so
        // return a empty vector
        if (root == nullptr) {
            std::vector<Node> temp = {};
            return temp;
        }
    
        // Recursive call on root->right
        std::vector<Node> rightVect = longestPath(root->rightChild);
    
        // Recursive call on root->left
        std::vector<Node> leftVect = longestPath(root->downChild);
    
        // Compare the size of the two vectors
        // and insert current node accordingly
        if (leftVect.size() > rightVect.size())
            leftVect.push_back(*root);
    
        else
            rightVect.push_back(*root);
    
        // Return the appropriate vector
        return (leftVect.size() > rightVect.size()
                    ? leftVect
                    : rightVect);
    }

    private:
    //====================================================================================
    //! @brief Sort rectangles By Their Max Side, Max(Width, Height) In Descendent Order
    //!        So The One Who Has Largest Side Get Packed First
    //===============================================================================
    void sortRects()
    {
        std::vector<std::pair<int, int>> maxsideIndexList;  // pair<maxside, index>
        for (auto i = 0; i != iRectList.size(); ++i)
            {
            int maxSide = std::max(iRectList[i].width, iRectList[i].height);
            std::pair<int, int>  maxSideIndex(maxSide, i);
            maxsideIndexList.push_back(maxSideIndex);
            }

        // sort the list in descending order according to maxSide value
        std::sort(maxsideIndexList.begin(), maxsideIndexList.end(), std::greater<std::pair<int, int>>());

        iSortedRectList.reserve(iRectList.size());

        for (auto iter = maxsideIndexList.begin(); iter != maxsideIndexList.end(); ++iter)
            iSortedRectList.push_back(iRectList[iter->second]);
    }


    void printRects()
    {
        std::cout << "The list of sorted rectangles for placement" << std::endl;

        //Since it is descending order, we expect the higher indexs first 
        for (Node rect: iSortedRectList) {
            std::cout << "RectIndex: " << rect.rectID << " " << "Width: " << rect.width 
            << " " << "Height: " << rect.height << std::endl;
        }
    }



    //==============================================================================
    //! @brief Packing rectangles into the outer frame
    //==============================================================================
    void Packing()
    {
        // sort rectangles by their max side, max(width, height) in descendent order
        
        sortRects();

        //print the order in which the rectangles will be placed
        printRects();
        // the initial frame dimensions (outer container)
        const int initialWidth = iSortedRectList[0].width;
        const int initialHeight = iSortedRectList[0].height;
        // std::cout << "Initial Width of Boundary = " << initialWidth << std::endl;
        // std::cout << "Initial Height of Boundary = " << initialHeight << std::endl;
        
        iPackingAlgorithm->Init(initialWidth, initialHeight);
        Node* rn = iPackingAlgorithm->rootNode();
        //std::cout << "Root Node width: " << rn->width << " " << "Root Node Height: " << rn->height << std::endl;

        for (auto i = 0; i != iSortedRectList.size(); ++i)
            {
            int width  = iSortedRectList[i].width;
            int height = iSortedRectList[i].height;
            //int id     = iSortedRectList[i].rectID;
            // std::cout << "iSortedRectList[" << i << "].width =" << iSortedRectList[i].width <<std::endl;
            // std::cout << "iSortedRectList[" << i << "].height =" << iSortedRectList[i].height <<std::endl;
            // std::cout << "iSortedRectList[" << i << "].id =" << iSortedRectList[i].rectID <<std::endl;
            Node* node = iPackingAlgorithm->Insert(width, height);

            if (node){
                //std::cout << "Splitting node on Rectangle " << i << std::endl;
                iPackingAlgorithm->SplitNode(node, width, height, i); // <-- i is rectID
            }
            else{    // run out space
                //std::cout << "Growing node on Rectangle " << i << std::endl;
                node = iPackingAlgorithm->GrowContainer(width, height, i); //<-- i 
            }
            }
    }
    private:
    BinaryTreeAlgorithm*        iPackingAlgorithm;
    std::vector<Node>          iRectList;
    std::vector<Node>          iSortedRectList;
};



//=========================================================================================
//! @brief Create a vector of 'semi-random' rectangles for placement. This is a function
//      primarily for testing
//! @param rectangles the address in memory to shovel semi-random rectangles
//=========================================================================================
void createRectangles(std::vector<Node> &rectangles)
{
    for (int i = 1; i < 4; ++i) {
        int w = i * 5; // semi-random
        int h = i * 5; // semi-random

        Node rectangle = Node(0, 0, w, h, i); //i is the rectID
        rectangles.push_back(rectangle);
    }
}

//========================================================================================
//! @brief Loads parameters from a text file in order to create a list of rectangles to place
//  in a box 
//! @param rectangles Initialized vector to store the rectangles for placement
//! @param filename File name for the read-in parameters
//=========================================================================================
void loadRectangles(std::vector<Node> &rectangles, std::string filename)
{
    //Create Input Filestream
    std::ifstream file(filename);
    //Throw an error if unable to open file
    if(!file.is_open()) throw std::runtime_error("Could not open file");
    //Check if file is open
    if (file.is_open()){
        //Read and store each line from the file
        std::string line;
        while(std::getline(file,line)){
            //Pass each line into an istringstream and read each comma separated valye
            std::stringstream s(line);
            std::string rowNum, height, width;
            //Store height, width and row number
            std::getline(s,rowNum,',');
            std::getline(s,width,',');
            std::getline(s,height,',');
            //Create a new rectangle
            Node rectangle = Node(0, 0, std::stoi(width), std::stoi(height),  std::stoi(rowNum)); //rowNum is the rectID
            rectangles.push_back(rectangle);
        }
      file.close();
    }
}


int main()
{
    const auto start = std::chrono::steady_clock::now();
    std::vector<Node> rectangles = std::vector<Node>();   
    //========Hard coded case:
    createRectangles(rectangles);

    TreeGenerator tree(rectangles);
    //std::cout << "Starting the bin packing..." << std::endl;
    //print input rectangles
    std::cout << "==========Hard coded case===========" << std::endl;
    std::cout << "ID    W    H" << std::endl;
    for (int i = 0; i < rectangles.size(); i++)
    {
        std::cout << rectangles[i].rectID << "    " << rectangles[i].width 
        <<  "    " << rectangles[i].height << std::endl;

    }
    tree.Run();

    //===============Test Case 1========================
    std::vector<Node> rectangles1 = std::vector<Node>();
    loadRectangles(rectangles1,"test1.txt");

    TreeGenerator tree1(rectangles1);
    std::cout << "\nStarting the bin packing for test case 1: " << std::endl;
    tree1.Run();

    std::cout << "Finished" << std::endl;

    //===============Test Case 2========================
    std::vector<Node> rectangles2 = std::vector<Node>();
    loadRectangles(rectangles2,"test2.txt");

    TreeGenerator tree2(rectangles2);
    std::cout << "\nStarting the bin packing for test case 2: " << std::endl;
    tree2.Run();

    std::cout << "Finished" << std::endl;

    //===============Test Case 3========================
    std::vector<Node> rectangles3 = std::vector<Node>();
    loadRectangles(rectangles3,"test3.txt");

    TreeGenerator tree3(rectangles3);
    std::cout << "\nStarting the bin packing for test case 3: " << std::endl;
    tree3.Run();

    std::cout << "Finished" << std::endl;

    //===============Test Case 4========================
    std::vector<Node> rectangles4 = std::vector<Node>();
    loadRectangles(rectangles4,"test4.txt");

    TreeGenerator tree4(rectangles4);
    std::cout << "\nStarting the bin packing for test case 4: " << std::endl;
    tree4.Run();

    std::cout << "Finished" << std::endl;

    //===============Test Case 5========================
    std::vector<Node> rectangles5 = std::vector<Node>();
    loadRectangles(rectangles5,"test5.txt");

    TreeGenerator tree5(rectangles5);
    std::cout << "\nStarting the bin packing for test case 5: " << std::endl;
    tree5.Run();

    std::cout << "Finished" << std::endl;

    //===============Test Case 6========================
    // 10 randomly generated width and heights (values between 1 and 100) for rectangles to place using
    // https://www.random.org/integers/?num=10&min=1&max=100&col=1&base=10&format=html&rnd=new
    std::vector<Node> rectangles6 = std::vector<Node>();
    loadRectangles(rectangles6,"test6.txt");

    TreeGenerator tree6(rectangles6);
    std::cout << "\nStarting the bin packing for test case 6: " << std::endl;
    tree6.Run();

    std::cout << "Finished" << std::endl;

    //===============Test Case 7========================
    // Randomly generated rectangles using excel random function

    std::vector<Node> rectangles7 = std::vector<Node>();
    loadRectangles(rectangles7,"test7.txt");

    TreeGenerator tree7(rectangles7);
    std::cout << "\nStarting the bin packing for test case 6: " << std::endl;
    tree7.Run();

    std::cout << "Finished" << std::endl;

  const auto end = std::chrono::steady_clock::now();
  std::chrono::duration<double> elapsed = end - start;
  std::cout << "Time in seconds: " << elapsed.count() << '\n';
}
