#ifndef QUEUE_H
#define QUEUE_H
#pragma once

#include "enumTypes.h"
#include <iostream>


template <class T>
class queue
{
private:
	int depth;
	int count;
	int head_ptr;
	int tail_ptr;
	T **entry;

public:
	explicit queue(): depth(0),count(0), head_ptr(0), tail_ptr(0), entry(NULL)
	{}

	explicit queue(const queue<T>& a):
	depth(a.depth),
	count(0),
	head_ptr(0),
	tail_ptr(0),
	entry(new T *[a.depth])
	{
		//entry = new T *[a.depth];
		
		if (a.entry[0] != NULL)
		{
			for (int i = a.depth - 1; i >= 0; i--)
			{
				enqueue(::new T);
			}
		}
		else
		{
			for (int i = a.depth - 1; i >= 0; i--)
			{
				entry[i] = NULL;
			}
		}

	}	

	explicit queue(int size, bool preallocate = false):
	depth(size),
		count(0),
		head_ptr(0),
		tail_ptr(0),
		entry(new T *[size])
	{
		//entry = new T *[size];

		if (preallocate)
		{      
			while (enqueue(::new T) == SUCCESS) {}
		}
		else
		{
			for (int i=0 ; i<size ; i++)
			{
				entry[i] = NULL;
			}
		}
	}

	~queue()
	{
		while (count > 0)
		{
			::delete dequeue();			
		}
		if (entry != NULL)
		{
			delete[] entry;
			entry = NULL;
		}
	}

	void init(int size, bool preallocate = false)
	{
		depth = size;
		count = 0;
		head_ptr = 0;
		tail_ptr = 0;
		entry = new T *[size];

		if (preallocate)
		{      
			for (int i = 0; i < size; i++)
				enqueue(::new T);
		}
		else
		{
			for (int i=0 ; i<size ; i++)
			{
				entry[i] = NULL;
			}
		}
	}	

	// queue: When you add stuff, tail pointer increments, if full, return FAILURE flag
	// I'm adding to the tail and removing from the head
	input_status_t enqueue(T *item)
	{
		if (count == depth)
			return FAILURE;
		else if (item == NULL)
		{
			std::cerr << "Input pointer is NULL" << std::endl;
			return FAILURE;
			//_exit(2);
		}
		else
		{
			count++;
			entry[tail_ptr] = item;
			tail_ptr = (tail_ptr + 1) % depth; 	/*advance tail_ptr */
			return SUCCESS;
		}
	}


	// acquire an item from the pool, same as new, but does not involve allocating new memory
	T *acquire_item()
	{
		if (count == 0)
			return ::new T;
		else
			return dequeue();
	}

	T *dequeue()
	{
		if (count == 0)
			return NULL;
		else
		{
			count--;

			T *item = entry[head_ptr];

			head_ptr = (head_ptr + 1) % depth;	//advance head_ptr

			return item;
		}
	}

	T *read_back() const
	{
		return count ? entry[head_ptr] : NULL;
	}

	inline int get_count() const
	{
		return count;
	}

	T *read(const int offset) const
	{
		if((offset >= count) || (offset < 0))
			return NULL;
		else
			return entry[(head_ptr + offset) % depth];
	}

	// release item into pool
	// This is useful for when the queue holds preallocated pieces of memory
	// and one would like to store them when they are not in use
	void release_item(T *item)
	{
		if(enqueue(item) == FAILURE)
			::delete item;
	}

	// this function makes this queue a non-FIFO queue.  
	// Allows insertion into the middle or at any end
	input_status_t insert(T *item,int offset)
	{
		if(count == depth)
			return FAILURE;
		else if (item == NULL)
		{
			std::cerr << "Attempting to insert NULL into queue" << std::endl;
			return FAILURE;
			//_exit(2);
		}
		else
		{
			// move everything back by one unit
			for (int i = count-1 ; i >= offset ; ++i)
				entry[(head_ptr+i+1) % depth] = entry[(head_ptr+i) % depth];

			count++;

			entry[(head_ptr+offset) % depth] = item;

			tail_ptr = (tail_ptr+1) % depth;	// advance tail_ptr

			return SUCCESS;
		}
	}

	int freecount() const
	{
		return depth - count;
	}

	T* newest() const
	{
		return read(count - 1);
	}

	T &operator=(const queue<T> &right)
	{
		while (count > 0)
		{
			delete dequeue();
		}
		delete entry;
		entry = new T *[right.depth];
		head_ptr = tail_ptr = 0;
		count = right.count;
		depth = right.depth;
		while (right.count > 0)
		{
			enqueue(right.dequeue());
		}
	}
};

#endif
