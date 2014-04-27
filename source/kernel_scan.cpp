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

namespace corridormap {

const char* kernel_compaction_reduce_source = \

"const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE|CLK_ADDRESS_CLAMP_TO_EDGE|CLK_FILTER_NEAREST;                        \n"
"                                                                                                                           \n"
"inline uint value1d(read_only image2d_t img, uint idx)                                                                     \n"
"{                                                                                                                          \n"
"    size_t w = get_image_width(img);                                                                                       \n"
"    int2 uv = (int2)(idx % w, idx / w);                                                                                    \n"
"    return read_imageui(img, sampler, uv).s0 > 0 ? 1 : 0;                                                                  \n"
"}                                                                                                                          \n"
"                                                                                                                           \n"
"#define WARP_SIZE 32                                                                                                       \n"
"                                                                                                                           \n"
"inline uint scan_warp(local uint* data, uint lane)                                                                         \n"
"{                                                                                                                          \n"
"    for (uint offset = 1; offset <= WARP_SIZE >> 1; offset <<= 1)                                                          \n"
"    {                                                                                                                      \n"
"        data[lane] += data[lane - offset];                                                                                 \n"
"        barrier(CLK_LOCAL_MEM_FENCE);                                                                                      \n"
"    }                                                                                                                      \n"
"                                                                                                                           \n"
"    return data[lane];                                                                                                     \n"
"}                                                                                                                          \n"
"                                                                                                                           \n"
"kernel void run(read_only image2d_t image, global uint* global_sums, local uint* block_data, const uint pixel_count)       \n"
"{                                                                                                                          \n"
"    size_t gid = get_global_id(0);                                                                                         \n"
"    size_t lid = get_local_id(0);                                                                                          \n"
"    size_t gwid = gid / WARP_SIZE;                                                                                         \n"
"    size_t lwid = lid / WARP_SIZE;                                                                                         \n"
"    size_t lane = lid & (WARP_SIZE - 1);                                                                                   \n"
"    size_t block_size = get_local_size(0);                                                                                 \n"
"    size_t elements_per_warp = pixel_count / (2 * block_size);                                                             \n"
"                                                                                                                           \n"
"    uint global_base = gwid * elements_per_warp;                                                                           \n"
"    local uint* warp_data = block_data + lwid * WARP_SIZE * 2;                                                             \n"
"                                                                                                                           \n"
"    uint sum = 0;                                                                                                          \n"
"                                                                                                                           \n"
"    for (uint i = 0; i < elements_per_warp; i += WARP_SIZE)                                                                \n"
"    {                                                                                                                      \n"
"        sum += value1d(image, global_base + i + lane);                                                                     \n"
"    }                                                                                                                      \n"
"                                                                                                                           \n"
"    warp_data[lane] = 0;                                                                                                   \n"
"    warp_data[lane+WARP_SIZE] = sum;                                                                                       \n"
"                                                                                                                           \n"
"    barrier(CLK_LOCAL_MEM_FENCE);                                                                                          \n"
"                                                                                                                           \n"
"    sum = scan_warp(warp_data, lane + WARP_SIZE);                                                                          \n"
"                                                                                                                           \n"
"    if (lane == WARP_SIZE-1)                                                                                               \n"
"    {                                                                                                                      \n"
"        global_sums[gwid] = sum;                                                                                           \n"
"    }                                                                                                                      \n"
"}                                                                                                                          \n";

}
