#pragma once

#include <vector>

#include "../Util/Vect3.h"
#include "../Util/Region.h"

// Quadrant class
enum class Octant : unsigned char
{
    Quadrant0 = 0x01,
    Quadrant1 = 0x02,
    Quadrant2 = 0x04,
    Quadrant3 = 0x08,
    Quadrant4 = 0x10,
    Quadrant5 = 0x20,
    Quadrant6 = 0x40,
    Quadrant7 = 0x80
};

#define OCTANTS 8                                                       // Maximum Octants
#define LODDEPTH 16                                                     // Maximum Depth

// Child for a octree
class OctreeNode
{
public:
    OctreeNode();            // Constructor
    OctreeNode(Vect3 inMax); // Constructor (InMax)

    ~OctreeNode(); // Deconstructor

    Region BoundRegion; // Boundary min and maximum

    // Parent Node
    OctreeNode *Parent = nullptr; // Parent

    // Octant quadrant
    OctreeNode *Node[OCTANTS]; // Pointers for each octant

    // depth preset
    unsigned int depth = 0; // Track depth    

    // Points in a vector
    std::vector<PointV3> PointList; // Could be used to hold vertex
    std::queue<PointV3> PointListQueue;

    unsigned char activeOctants; // Set active Octants

    bool hasChildren = false; // Set has children

    bool treeReady = false; // Set tree ready

    void BuildTree(unsigned int indepth); // Build blank tree to a depth

    void CalculateBounds(Octant octant); // Calculate bounds

    void InitPointData(std::vector<PointV3> inData); // In Data

    void BuildTreeFromData(); // Build Tree from data

    void PreCalculateBounds(Octant octant, Vect3 &outMin, Vect3 &outMax);

    void AddQueue(PointV3 inPoint); // Add Point to Queue for auto build tree

    void ClearNodesAll();    // Clear all nodes

    void GetAllNodesAtDepth(unsigned int indepth, TArray<OctreeNode *> &index);
    
    bool HasChild();       // Check child

    bool VectorInNode(Vect3 point, float boundradius, float tolerance);   // Vector in Node
};
