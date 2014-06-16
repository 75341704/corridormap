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

#ifndef CORRIDORMAP_RUNTIME_TYPES_H_
#define CORRIDORMAP_RUNTIME_TYPES_H_

namespace corridormap {

enum { null_idx = ~0u };

struct Vec2
{
    float x;
    float y;
};

// vertex of the voronoi diagram.
struct Vertex
{
    // pool link.
    int link;
    // index of the first outgoing half-edge.
    int half_edge;
    // position.
    Vec2 pos;
};

// one direction of the edge in voronoi diagram.
struct Half_Edge
{
    // link to the next incident half-edge.
    int next;
    // index of the target vertex.
    int target;
    // index of the head of the event list when going this half-edge direction.
    int event;
    // closest points on left and right obstacles along the half-edge direction.
    Vec2 sides[2];
};

// edge of the voronoi diagram.
struct Edge
{
    // pool link.
    int link;
    // two halves of the edge.
    Half_Edge dir[2];
};

// edge event point - position on the edge where closest obstacle changes.
struct Event
{
    // pool link.
    int link;
    // next event for both half-edge directions.
    int next[2];
    // position.
    Vec2 pos;
    // closest points on left and right obstacles.
    Vec2 sides[2];
};

// array based free list.
template <typename T>
struct Free_List
{
    // head of the list of allocated objects.
    int head;
    // tail of the list of allocated objects.
    int tail;
    // head of the list of free objects.
    int head_free;
    // total number of allocated objects.
    int num_items;
    // size of the items array.
    int max_items;
    // array of items of size max_items.
    T* items;
};

// Generalized Voronoi Diagram where edges and vertices are annotated with the closest obstacle information.
struct Voronoi_Diagram
{
    // pool of vertices.
    Free_List<Vertex> vertices;
    // pool of edges.
    Free_List<Edge> edges;
    // pool of edge events.
    Free_List<Event> events;
};

}

#endif
