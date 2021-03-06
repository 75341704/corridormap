//
// Copyright (c) 2014 Alexander Shafranov <shafranov@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef CORRIDORMAP_BUILD_TYPES_H_
#define CORRIDORMAP_BUILD_TYPES_H_

// 
// Types used during construction of the corridor map.
//

namespace corridormap {

// Obstacles represented as a set of 2d convex polygons. Polys are expected to be in CCW order.
struct Footprint
{
    // the number of polygons.
    int num_polys;
    // the total number of vertices.
    int num_verts;
    // x coords indexed in [0..num_verts) range.
    float* x;
    // y coords indexed in [0..num_verts) range.
    float* y;
    // array of vertex counts per poly, indexed in [0..num_polys) range.
    int* num_poly_verts;
};

// 2d bounding box.
struct Bbox2
{
    float min[2];
    float max[2];
};

// 3d vertex used for distance_mesh.
struct Render_Vertex
{
    float x;
    float y;
    float z;
};

// Border gets a distance mesh segment (half tent) per side.
enum { num_border_segments = 4 };

// Segmented distance mesh suitable for rendering.
// each segment represents one footprint polygon.
// triangles are stored as a list of vertices in CCW order.
struct Distance_Mesh
{
    // the number of segments.
    int num_segments;
    // the total number of vertices.
    int num_verts;
    // vertex array indexed in [0..num_verts) range.
    Render_Vertex* verts;
    // the number of vertices per segment. indexed in [0..num_segments) range.
    int* num_segment_verts;
    // segment colors. indexed in [0..num_segments) range.
    unsigned int* segment_colors;
};

// Voronoi diagram vertices and edges detected from the distance mesh render.
struct Voronoi_Features
{
    // rasterization grid width.
    int grid_width;
    // rasterization grid height.
    int grid_height;
    // number of voronoi vertex points.
    int num_vert_points;
    // number of voronoi edge points.
    int num_edge_points;
    // grid indices (y*grid_width + x) of vertex points. [0..num_vert_points).
    unsigned int* verts;
    // grid indices (y*grid_width + x) of edge points. [0..num_edge_points).
    unsigned int* edges;
    // side 1 IDs (colors) of obstacles of each edge point. [0..num_edge_points).
    unsigned int* edge_obstacle_ids_1;
    // side 2 IDs (colors) of obstacles of each edge point. [0..num_edge_points).
    unsigned int* edge_obstacle_ids_2;
};

// Obstacle polygon edge normals.
struct Footprint_Normals
{
    // number of obstacles in footprint (number of polys + 4 border segments).
    int num_obstacles;
    // total number of normals (one per each edge in footprint).
    int num_normals;
    // x coord indexed in [0..num_normals)
    float* x;
    // y coord indexed in [0..num_normals)
    float* y;
    // array of normal counts per obstacle, indexed in [0..num_obstacles) range.
    int* num_obstacle_normals;
    // offsets in x, y arrays for each poly, indexed in [0..num_obstacles) range.
    int* obstacle_normal_offsets;
};

// For each edge point and each side stores vertex_index+1 if the edge point is in the space spanned
// by vertex and its two normals, or 0 if edge point is not part of any span.
struct Voronoi_Edge_Spans
{
    // vertex index on one side.
    // equals i+1 if edge point lies in normals[i], normals[i+1] span. 0 otherwise.
    // [0..num_edge_points]
    int* indices_1;
    // vertex index on another side.
    // equals i+1 if edge point lies in normals[i], normals[i+1] span. 0 otherwise.
    // [0..num_edge_points]
    int* indices_2;
};

// Compressed-Sparse-Row format for boolean grid,
// used as a fast lookup of voronoi features during tracing.
struct CSR_Grid
{
    // number of rows in the grid.
    int num_rows;
    // number of columns in the grid.
    int num_cols;
    // number of non-zero (non-empty) cells in the grid.
    int num_nz;
    // stores column index for each non-zero element. indexed in [0 .. num_nz).
    int* column;
    // columns of the row R: row_offset[R] .. row_offset[R+1]. indexed in [0 .. num_rows + 1).
    int* row_offset;
};

// Rendered distance mesh is a 4-connected grid.
enum { max_grid_neis = 4 };

// Pack of up to max_grid_neis cells.
struct CSR_Grid_Neis
{
    // number of neighbours [0..max_grid_neis].
    int num;
    // index among non-zero cells of grid.
    int nz_idx[max_grid_neis];
    // neighbouring cell row.
    int row[max_grid_neis];
    // neighbouring cell column.
    int col[max_grid_neis];
    // linear index y*num_cols + x.
    int lin_idx[max_grid_neis];
};

// Output of trace_edges function.
struct Voronoi_Traced_Edges
{
    // number of edges found.
    int num_edges;
    // number of event points found.
    int num_events;
    // u linear index (y*num_rows + x). range [0 .. num_edges).
    int* u;
    // u linear index (y*num_rows + x). range [0 .. num_edges).
    int* v;
    // side 1 IDs (colors) of obstacles of each edge. [0..num_edges).
    unsigned int* obstacle_ids_1;
    // side 2 IDs (colors) of obstacles of each edge. [0..num_edges).
    unsigned int* obstacle_ids_2;
    // for each edge [0 .. num_edges) stores the offset to events array.
    int* edge_event_offset;
    // number of event points for each edge [0 .. num_edges).
    int* edge_num_events;
    // absolute values are event point linear indices. (> 0 = side 1 event, < 0 = side 2 event). [0 .. num_events).
    int* events;
};

// Helper struct to pass input data to the final assembly of the walkable space structure.
struct Walkable_Space_Build_Params
{
    Bbox2 bounds;
    Footprint* obstacles;
    Footprint_Normals* obstacle_normals;
    Voronoi_Features* features;
    Voronoi_Traced_Edges* traced_edges;
    Voronoi_Edge_Spans* spans;
    CSR_Grid* edge_grid;
    CSR_Grid* vertex_grid;
};

}

#endif
