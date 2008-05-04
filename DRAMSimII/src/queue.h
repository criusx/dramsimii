#ifndef QUEUE_H
#define QUEUE_H
#pragma once

#include <iostream>
#include <cassert>

namespace DRAMSimII
{
	/// @brief the queue template class, a circular queue
	/// @detail push/pop are O(1) operations, while random insertions are O(n) operations
	template <class T>
	class Queue
	{
	private:
		unsigned depth;	///< how big is this queue
		unsigned count;	///< how many elements are in the queue now
		unsigned head;	///< the point where items will be inserted
		unsigned tail;	///< the point where items will be removed
		T **entry;		///< the circular queue

	public:
		explicit Queue(): depth(0),count(0), head(0), tail(0), entry(NULL)
		{}

		/// @brief copy constructor
		/// @detail copy the existing queue, making copies of each element
		explicit Queue(const Queue<T>& a):
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

		/// @brief constructor
		/// @detail create a queue of a certain size and optionally fill it with empty elements
		/// @param size the depth of the circular queue
		/// @preallocate whether or not to fill the queue with blank elements, defaults to false
		explicit Queue(const unsigned size, const bool preallocate = false):
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

		/// @brief destructor
		/// @detail remove the elements and delete them before removing the rest of the queue
		~Queue()
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

		/// @brief change the size of the queue
		/// @detail remove all existing elements and create a new queue of a different size
		/// @param size the depth to set the queue to
		/// @param preallocate whether or not to fill the queue with blank elements, defaults to false
		void resize(unsigned size, bool preallocate = false)
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

		/// @brief add an item to the back of the queue
		/// @detail issue a warning if the element is null\nreturn false if the queue is already full\nadd to the tail pointer position and remove from the head
		/// @param item the item to be inserted into the queue
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

		/// @brief treat this queue like an object pool and retrieve an item
		/// @detail if there is no available object, then create one
		/// @return a new item which may or may not be initialized
		T *acquire_item()
		{
			if (count == 0)
			{
				T* item = ::new T;
				assert(item != NULL);
				return item;
			}
			else
				return pop();
		}

		/// @brief remove the item at the front of the queue
		/// @return the item at the head of the queue, NULL if the queue is empty
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

		/// @brief get a pointer to the item at the head of the queue
		/// @detail similar to peek()
		/// @return a pointer to the item at the front of the queue, or NULL if the queue is empty
		const T *front() const
		{
			return count ? entry[head] : NULL;
		}

		/// @brief to get a pointer to the item most recently inserted into the queue
		const T* back() const
		{
			return count ? entry[(head + count - 1) % depth] : NULL;
		}

		/// @brief get the number of entries currently in this queue
		inline unsigned size() const
		{
			return count;
		}

		/// @brief get the number of entries this queue can hold
		inline unsigned get_depth() const
		{
			return depth;
		}

		/// @brief get a pointer to the item at this offset without removing it
		T *read(const unsigned offset) const
		{
			if ((offset >= count) || (offset < 0))
				return NULL;
			else
				return entry[(head + offset) % depth];
		}

		/// @brief release item into pool
		/// @detail This is useful for when the queue holds preallocated pieces of memory
		/// and one would like to store them when they are not in use
		void release_item(T *item)
		{
			if(!push(item))
				::delete item;
		}

		/// @brief this function makes this queue a non-FIFO queue.  
		/// @detail Allows insertion into the middle or at any end
		bool insert(T *item, const int offset)
		{
			assert(offset <= count - 1);
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
				for (int i = count - 1 ; i >= offset ; --i)
					entry[(head + i + 1) % depth] = entry[(head + i) % depth];

				count++;

				entry[(head+offset) % depth] = item;

				tail = (tail+1) % depth;	// advance tail_ptr

				return true;
			}
		}

		/// @brief the number of entries still available in this queue
		unsigned freecount() const
		{
			return depth - count;
		}

		/// @brief whether or not there is room for any more entries in this queue
		bool isFull() const
		{
			return (depth - count) == 0;
		}

		/// @brief whether or not this queue has no entries in it
		bool isEmpty() const
		{
			return count == 0;
		}
		
		/// @brief assignment operator overload
		/// @detail moves all the objects from the rhs object to the lhs object
		Queue<T> &operator=(const Queue<T> &right)
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
