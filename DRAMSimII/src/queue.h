#ifndef QUEUE_H
#define QUEUE_H
#pragma once

#include <iostream>
#include <cassert>

namespace DRAMSimII
{
	template <class T>
	class queue
	{
	private:
		unsigned depth; // how big is this queue
		unsigned count; // how many elements are in the queue now
		unsigned head; // the point where items will be inserted
		unsigned tail; // the point where items will be removed
		T **entry; // the circular queue

	public:
		explicit queue(): depth(0),count(0), head(0), tail(0), entry(NULL)
		{}

		explicit queue(const queue<T>& a):
			depth(a.depth),
			count(0),
			head(0),
			tail(0),
			entry(new T *[a.depth])
			{
				for (unsigned i = 0; i < a.count; i++)
				{
					assert(a.read(i) != NULL);
					// attempt to copy the contents of this queue
					push(::new T(*a.read(i)));
				}
			}	

		explicit queue(const unsigned size, const bool preallocate = false):
		depth(size),
			count(0),
			head(0),
			tail(0),
			entry(new T *[size])
		{
			if (preallocate)
			{      
				T *newOne = ::new T();
				while (push(newOne))
				{
					newOne = ::new T();
				}
				::delete newOne;
			}
		}

		~queue()
		{
			while (count > 0)
			{
				::delete pop();			
			}
			if (entry != NULL)
			{
				delete[] entry;
				entry = NULL;
			}
		}

		void init(unsigned size, bool preallocate = false)
		{
			depth = size;
			count = 0;
			head = 0;
			tail = 0;
			entry = new T *[size];

			if (preallocate)
			{      
				for (unsigned i = 0; i < size; i++)
					push(::new T);
			}
			else
			{
				for (unsigned i = 0 ; i < size; i++)
				{
					entry[i] = NULL;
				}
			}
		}	

		// queue: When you add stuff, tail pointer increments, if full, return false
		// I'm adding to the tail and removing from the head
		bool push(T *item)
		{
			assert(item != NULL);
			if (count == depth)
				return false;
			else if (item == NULL)
			{
				std::cerr << "Input pointer is NULL" << std::endl;
				return false;
			}
			else
			{
				count++;
				entry[tail] = item;
				tail = (tail + 1) % depth; 	//advance tail_ptr
				return true;
			}
		}


		// acquire an item from the pool, same as new, but does not involve allocating new memory
		T *acquire_item()
		{
			if (count == 0)
				return ::new T;
			else
				return pop();
		}

		T *pop()
		{
			if (count == 0)
				return NULL;
			else
			{
				count--;

				T *item = entry[head];

				head = (head + 1) % depth;	//advance head_ptr

				return item;
			}
		}

		T *front() const
		{
			return count ? entry[head] : NULL;
		}

		inline unsigned size() const
		{
			return count;
		}

		inline unsigned get_depth() const
		{
			return depth;
		}

		// get a pointer to the item at this offset without removing it
		T *read(const unsigned offset) const
		{
			if((offset >= count) || (offset < 0))
				return NULL;
			else
				return entry[(head + offset) % depth];
		}

		// release item into pool
		// This is useful for when the queue holds preallocated pieces of memory
		// and one would like to store them when they are not in use
		void release_item(T *item)
		{
			if(!push(item))
				::delete item;
		}

		// this function makes this queue a non-FIFO queue.  
		// Allows insertion into the middle or at any end
		bool insert(T *item, const unsigned offset)
		{
			if (count == depth)
				return false;
			else if (item == NULL)
			{
				std::cerr << "Attempting to insert NULL into queue" << std::endl;
				return false;
				//_exit(2);
			}
			else
			{
				// move everything back by one unit
				for (unsigned i = count - 1 ; i >= offset ; ++i)
					entry[(head+i+1) % depth] = entry[(head+i) % depth];

				count++;

				entry[(head+offset) % depth] = item;

				tail = (tail+1) % depth;	// advance tail_ptr

				return true;
			}
		}

		unsigned freecount() const
		{
			return depth - count;
		}

		T* newest() const
		{
			return read(count - 1);
		}

		queue<T> &operator=(const queue<T> &right)
		{
			if (&right == this)
				return *this;

			while (count > 0)
			{
				delete pop();
			}
			delete entry;
			entry = new T *[right.depth];
			head = tail = 0;
			count = right.count;
			depth = right.depth;

			for (unsigned i = 0; i < right.depth; i++)
			{
				push(right.read(i));
			}
			return *this;
		}
	};
}
#endif
