#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "bvh.h"
#include "scene.h"
#include "v3math.h"

//#define VERBOSE

int use_bvh=0;

// The root node of the BVH
bvh_node    *bvh_root;

static int  max_depth=16;
static int  acceptable_leaf_size=4;

static int  num_leafs, num_inner_nodes;
static int  max_leaf_size;

static bvh_node*
create_leaf_node(boundingbox bbox, triangle* triangles, int num_triangles)
{
    bvh_node    *leaf;

    leaf = (bvh_node*) malloc(sizeof(bvh_node));
    leaf->is_leaf = 1;
    leaf->bbox = bbox;

    // We slightly enlarge the bbox around the group of triangles,
    // to make sure all triangles lie *within* the bbox
    leaf->bbox.min.x -= 1.0e-6;
    leaf->bbox.min.y -= 1.0e-6;
    leaf->bbox.min.z -= 1.0e-6;
    leaf->bbox.max.x += 1.0e-6;
    leaf->bbox.max.y += 1.0e-6;
    leaf->bbox.max.z += 1.0e-6;

    leaf->u.leaf.triangles = triangles;
    leaf->u.leaf.num_triangles = num_triangles;

    num_leafs++;
    if (num_triangles > max_leaf_size)
        max_leaf_size = num_triangles;

    return leaf;
}

static bvh_node*
create_inner_node(bvh_node* left_child, bvh_node* right_child)
{
    bvh_node    *inner;
    boundingbox bbox;

    inner = (bvh_node*) malloc(sizeof(bvh_node));
    inner->is_leaf = 0;

    // An inner node's bbox is always the exact combination of
    // its children bboxes.
    bbox = left_child->bbox;

    if (right_child->bbox.min.x < bbox.min.x)   bbox.min.x = right_child->bbox.min.x;
    if (right_child->bbox.min.y < bbox.min.y)   bbox.min.y = right_child->bbox.min.y;
    if (right_child->bbox.min.z < bbox.min.z)   bbox.min.z = right_child->bbox.min.z;
    if (right_child->bbox.max.x > bbox.max.x)   bbox.max.x = right_child->bbox.max.x;
    if (right_child->bbox.max.y > bbox.max.y)   bbox.max.y = right_child->bbox.max.y;
    if (right_child->bbox.max.z > bbox.max.z)   bbox.max.z = right_child->bbox.max.z;

    inner->bbox = bbox;

    inner->u.inner.left_child = left_child;
    inner->u.inner.right_child = right_child;

    num_inner_nodes++;

    return inner;
}

static int
is_left(triangle *t, int split_axis, float split_value)
{
    float   value, min_value, max_value;

    // Determine the extents that the triangle's vertices span
    // on the given axis

    min_value = v3_component(scene_vertices[t->v[0]], split_axis);
    max_value = min_value;

    for (int i = 1; i < 3; i++)
    {
        value = v3_component(scene_vertices[t->v[i]], split_axis);
        if (value < min_value) min_value = value;
        if (value > max_value) max_value = value;
    }

    // Determine if the triangle is (mostly) on the left side of
    // the split plane. We say "mostly" because a triangle does not
    // need to be fully on the left side of the split plane to be
    // considered "left of", it may span the split plane.

    return (0.5*(min_value+max_value)) <= split_value;
}

// Determine the bounding box that encloses the given triangles
static boundingbox
bound_triangles(triangle *triangles, int num_triangles)
{
    int         t, i;
    boundingbox bbox;

    bbox = bbox_create();

    // Iterate over all triangles
    for (t = 0; t < num_triangles; t++)
    {
        // Iterate over this triangle's vertices
        for (i = 0; i < 3; i++)
            bbox_update(&bbox, scene_vertices[triangles[t].v[i]]);
    }

    return bbox;
}

// Partition the given triangles into two groups: those (mostly) left of
// and mostly right of the given split plane, based on a triangle's
// bound for the given split axis. Sets the parameters num_triangles_left
// and num_triangles_right when done.
static void
partition_on_split_value(int *num_triangles_left, int *num_triangles_right,
    triangle *first_triangle, int num_triangles,
    int split_axis, float split_value)
{
    triangle    *l, *r, t;
    triangle    *last_triangle;

    last_triangle = first_triangle + num_triangles - 1;

    l = first_triangle;
    r = last_triangle;

    while (l < r)
    {
        // Increase "l" to first triangle that needs to be on the *right* (yes, right)
        while (l <= last_triangle && is_left(l, split_axis, split_value))
            l++;

        // Decrease "r" to first triangle that needs to be on the *left*
        while (r >= first_triangle && !is_left(r, split_axis, split_value))
            r--;

        if (l < r)
        {
            // Swap triangles to put them in the correct lists
            t = *l;
            *l = *r;
            *r = t;

            // Update pointers as we have now processed 2 triangles
            l++;
            r--;
        }
    }

    // When we get here, l >= r

    if (l > last_triangle)
    {
        // All triangles on the left of the split plane
        *num_triangles_left = num_triangles;
        *num_triangles_right = 0;
    }
    else if (r < first_triangle)
    {
        // All triangles on the right
        *num_triangles_left = 0;
        *num_triangles_right = num_triangles;
    }
    else
    {
        // Triangles on both sides of the split plane
        *num_triangles_left = l - first_triangle;
        *num_triangles_right = num_triangles - *num_triangles_left;

        if (l == r)
        {
            // The triangle pointed to by both l and r hasn't been processed yet
            if (is_left(l, split_value, split_axis))
            {
                // Correct the triangle count in this case, as we included
                // the triangle on the right initially, while it is actually
                // on the left of the split plane
                (*num_triangles_left)++;
                (*num_triangles_right)--;
            }

        }
    }

}

// Recursively subdivide triangles into two groups, by determining
// a split plane and sorting the triangles into "left-of" and "right-of".
static bvh_node*
bvh_build_recursive(int depth, boundingbox bbox, triangle* triangles, int num_triangles)
{
    vec3        bbox_size, bbox_center;
    int         i, j, t;
    int         sorted_dimensions[3];
    int         split_axis;
    float       split_value;
    int         num_triangles_left, num_triangles_right;
    boundingbox left_bbox, right_bbox;
    bvh_node    *left_child, *right_child;

    if (depth == max_depth || num_triangles <= acceptable_leaf_size)
    {
        // Create a leaf node
#ifdef VERBOSE
        if (num_triangles > acceptable_leaf_size)
            printf("[%d] Maximum depth reached, forced to create a leaf of %d triangles\n", depth, num_triangles);
        else
            printf("[%d] Creating a leaf node of %d triangles\n", depth, num_triangles);
#endif
        return create_leaf_node(bbox, triangles, num_triangles);
    }

    //
    // Split the triangles into two groups, using a split plane based
    // on largest bbox side
    //

    bbox_size = v3_subtract(bbox.max, bbox.min);
    bbox_center = v3_multiply(v3_add(bbox.min, bbox.max), 0.5);

    // Sort bbox sides by size in descending order

    sorted_dimensions[0] = 0;
    sorted_dimensions[1] = 1;
    sorted_dimensions[2] = 2;

    // Good old bubble sort :)
    for (i = 0; i < 2; i++)
    {
        for (j = i+1; j < 3; j++)
        {
            if (v3_component(bbox_size, sorted_dimensions[i])
                <
                v3_component(bbox_size, sorted_dimensions[j]))
            {
                t = sorted_dimensions[i];
                sorted_dimensions[i] = sorted_dimensions[j];
                sorted_dimensions[j] = t;
            }
        }
    }

    // Iterate over the three split dimensions in descending order of
    // bbox size on that dimension. When the triangles are unsplitable
    // (or not very favorably) continue to the next dimension. Create a
    // leaf with all triangles in case none of dimensions is a good
    // split candidate.

    for (i = 0; i < 3; i++)
    {
        // Partition the triangles on the chosen split axis, with the
        // split plane at the center of the bbox

        split_axis = sorted_dimensions[i];
        split_value = v3_component(bbox_center, split_axis);

#ifdef VERBOSE
        printf("[%d] Splitting on axis %d, value %.3f\n", depth, split_axis, split_value);
#endif

        partition_on_split_value(&num_triangles_left, &num_triangles_right,
            triangles, num_triangles, split_axis, split_value);

#ifdef VERBOSE
        printf("[%d] %d left, %d right\n", depth, num_triangles_left, num_triangles_right);
#endif

        if (num_triangles_left == 0 || num_triangles_right == 0)
            continue;

        // Determine bboxes for the two new groups of triangles

        left_bbox = bound_triangles(triangles, num_triangles_left);

        if (v3_component(v3_subtract(left_bbox.max, left_bbox.min), split_axis)
            >
            0.9 * v3_component(bbox_size, split_axis))
        {
            //printf("[%d] skipping dimension (left)\n", split_axis);
            continue;
        }

#ifdef VERBOSE
        printf("[%d] left bbox:  %.3f, %.3f, %.3f .. %.3f, %.3f, %.3f\n",
            depth, left_bbox.min.x, left_bbox.min.y, left_bbox.min.z,
            left_bbox.max.x, left_bbox.max.y, left_bbox.max.z);
#endif

        right_bbox = bound_triangles(triangles+num_triangles_left, num_triangles_right);

        if (v3_component(v3_subtract(right_bbox.max, right_bbox.min), split_axis)
            >
            0.9 * v3_component(bbox_size, split_axis))
        {
            //printf("[%d] skipping dimension (right)\n", split_axis);
            continue;
        }

#ifdef VERBOSE
        printf("[%d] right bbox: %.3f, %.3f, %.3f .. %.3f, %.3f, %.3f\n",
            depth, right_bbox.min.x, right_bbox.min.y, right_bbox.min.z,
            right_bbox.max.x, right_bbox.max.y, right_bbox.max.z);
#endif

        // Recurse using the two new groups

        left_child = bvh_build_recursive(depth+1, left_bbox, triangles, num_triangles_left);
        right_child = bvh_build_recursive(depth+1, right_bbox, triangles+num_triangles_left, num_triangles_right);

        // Create an inner code with two children

        return create_inner_node(left_child, right_child);
    }

    // Split dimensions exhausted, forced to create a leaf

#ifdef VERBOSE
    printf("Split dimensions exhausted, creating a leaf of %d triangles\n", num_triangles);
#endif

    return create_leaf_node(bbox, triangles, num_triangles);
}

// Build a BVH for the triangles in the scene
void
bvh_build(void)
{
    boundingbox bbox;
    int         estimated_num_leafs;

    // Compute the bounding box of all the triangles.
    // This equals the bounding box of all the vertices (assuming
    // all vertices are actually used)

    bbox = bbox_create();

    for (int i = 0; i < scene_num_vertices; i++)
        bbox_update(&bbox, scene_vertices[i]);

    printf("bvh_build():\n");
    printf("... scene bounding box:\n");
    printf("... (min) %.3f, %.3f, %.3f\n", bbox.min.x, bbox.min.y, bbox.min.z);
    printf("... (max) %.3f, %.3f, %.3f\n", bbox.max.x, bbox.max.y, bbox.max.z);

    num_leafs = 0;
    num_inner_nodes = 0;
    max_leaf_size = 0;

    // A balanced BVH built fully to depth D wil have
    //     2^(D-1)-1    inner nodes
    // and
    //     2^(D-1)      leaf nodes
    // Divide the total number of triangles to store by acceptable_leaf_size
    // to get an estimate of the number of leaf nodes needed. Then, set
    // the maximum depth to a value that will allow that number of
    // leafs to be reached.
    // As the tree in reality will almost always not be balanced, this
    // does not guarantee that there won't be instances in which the
    // maximum depth is reached and a leaf needs to be created. So we
    // add 10 more levels just to be sure :)

    estimated_num_leafs = (int)(1.0 * scene_num_triangles / acceptable_leaf_size + 0.5);
    printf("... Estimated number of leaf nodes needed = %d\n", estimated_num_leafs);

    max_depth = 10 + ceil(1 + log10(estimated_num_leafs) / log10(2.0));
    printf("... Setting max_depth to %d\n", max_depth);

    // Build the BVH

    bvh_root = bvh_build_recursive(1, bbox, scene_triangles, scene_num_triangles);

    // Done

    printf("Done building BVH for %d scene triangles\n", scene_num_triangles);
    printf("... tree has %d leaf nodes, %d inner nodes\n",
        num_leafs, num_inner_nodes);
    printf("... maximum leaf size %d\n", max_leaf_size);
}
