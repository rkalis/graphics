#ifndef BVH_H
#define BVH_H

#include <assert.h>
#include "bbox.h"

// Flag that controls if intersection testing is done with the use
// of the BVH
int use_bvh;

//
// One node in the Bounding Volume Hierarchy
// We use a single data type for this to avoid having to cast
// pointer values and such.
//

// We can't create a typedef bvh_node, as a _bvh_node contains
// pointers to _bvh_node's (its children)

#define bvh_node    struct _bvh_node

struct _bvh_node
{
    // Boolean determining if this node is a leaf node or an inner node
    int is_leaf;

    // The bounding box for this node
    boundingbox bbox;

    // Depending on the is_leaf flag access .leaf or .inner
    union
    {
        // Leaf node
        struct
        {
            int         num_triangles;
            triangle    *triangles;
        }
        leaf;

        // Inner node
        struct
        {
            // Neither of these will be NULL
            bvh_node    *left_child, *right_child;
        }
        inner;
    }
    u;
};

// Build a BVH for the triangles in the scene.
// This function is called from read_scene().
void        bvh_build(void);

// The root node of the hierarchy.
// Set by bvh_build()
bvh_node    *bvh_root;


// Return a child node for an inner node
// (either left or right child)

static inline bvh_node*
inner_node_left_child(const bvh_node *n)
{
    assert (!n->is_leaf && "Node n is not an inner node!");
    return n->u.inner.left_child;
}

static inline bvh_node*
inner_node_right_child(const bvh_node *n)
{
    assert (!n->is_leaf && "Node n is not an inner node!");
    return n->u.inner.right_child;
}

// Return the (number of) triangles for a leaf node

static inline int
leaf_node_num_triangles(const bvh_node *n)
{
    assert (n->is_leaf && "Node n is not a leaf node!");
    return n->u.leaf.num_triangles;
}

static inline triangle*
leaf_node_triangles(const bvh_node *n)
{
    assert (n->is_leaf && "Node n is not a leaf node!");
    return n->u.leaf.triangles;
}

#endif
