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

#ifndef CORRIDORMAP_POOL_H_
#define CORRIDORMAP_POOL_H_

#include "runtime_types.h"

namespace corridormap {

template <typename T>
void init(Pool<T>& lst)
{
    lst.head = null_idx;
    lst.tail = null_idx;
    lst.head_free = 0;
    lst.num_items = 0;

    for (int i = 0; i < lst.max_items - 1; ++i)
    {
        lst.items[i].link = i + 1;
    }

    lst.items[lst.max_items - 1].link = null_idx;
}

template <typename T>
T* allocate(Pool<T>& lst)
{
    if (lst.head_free == null_idx)
    {
        return 0;
    }

    int result_index = lst.head_free;
    T* result = lst.items + result_index;

    lst.head_free = lst.items[lst.head_free].link;

    if (lst.head == null_idx)
    {
        lst.head = result_index;
        lst.tail = result_index;
        lst.items[result_index].link = null_idx;
    }
    else
    {
        lst.items[lst.tail].link = result_index;
        lst.tail = result_index;
        lst.items[result_index].link = null_idx;
    }

    lst.num_items++;

    return result;
}

template <typename T>
void deallocate(Pool<T>& lst, T* item)
{
    int idx = int(item - lst.items);

    if (lst.head == idx)
    {
        lst.head = item->link;

        if (lst.tail == idx)
        {
            lst.tail = null_idx;
        }
    }
    else
    {
        for (int i = 0; i < lst.max_items; ++i)
        {
            if (lst.items[i].link == idx)
            {
                lst.items[i].link = item->link;

                if (lst.tail == idx)
                {
                    lst.tail = i;
                }

                break;
            }
        }
    }

    item->link = lst.head_free;
    lst.head_free = idx;

    lst.num_items--;
}

/// Pool

template <typename T>
inline T* ptr(const Pool<T>& lst, int offset)
{
    return (offset != null_idx) ? lst.items + offset : 0;
}

template <typename T>
inline T* first(const Pool<T>& lst)
{
    return ptr(lst, lst.head);
}

template <typename T>
inline T* next(const Pool<T>& lst, T* item)
{
    int next_index = lst.items[int(item - lst.items)].link;
    return ptr(lst, next_index);
}

}

#endif
