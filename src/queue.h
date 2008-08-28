#ifndef QUEUE_H
#define QUEUE_H
#pragma once

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/vector.hpp>

#include <iostream>
#include <cassert>
#include <vector>


namespace DRAMSimII
{
	// forward declaration
	template <typename T>
	class Queue;

	template <typename T>
	std::ostream& operator<<(std::ostream&, const Queue<T>&);


	/// @brief the queue template class, a circular queue
	/// @detail push/pop are O(1) operations, while random insertions are O(n) operations
	template <typename T>
	class Queue
	{
	private:
		unsigned maxCount; ///< how many elements can there be total
		unsigned count;	///< how many elements are in the queue now
		unsigned head;	///< the point where items will be inserted
		unsigned tail;	///< the point where items will be removed
		std::vector<T *> entry;		///< the circular queue

	public:
		explicit Queue(): maxCount(0), count(0), head(0), tail(0), entry(0)
		{}

		/// @brief copy constructor
		/// @detail copy the existing queue, making copies of each element
		explicit Queue(const Queue<T>& a):
		maxCount(a.maxCount),
			count(a.count),
			head(a.head),
			tail(a.tail),
			entry(a.maxCount)
		{
			for (unsigned i = 0; i < a.count; i++)
			{
				assert(a.at(i) != NULL);
				// attempt to copy the contents of this queue
				entry[(head + i) % maxCount] = new T(*a.at(i));
			}
		}	

		/// @brief constructor
		/// @detail create a queue of a certain size and optionally fill it with empty elements
		/// @param size the depth of the circular queue
		/// @preallocate whether or not to fill the queue with blank elements, defaults to false
		explicit Queue(const unsigned size, const bool preallocate = false):
		maxCount(size),
			count(0),
			head(0),
			tail(0),
			entry(size)
		{
			if (preallocate)
			{      
				while (entry.size() < size)
				{
					push(::new T());
				}
			}
		}

		/// @brief destructor
		/// @detail remove the elements and delete them before removing the rest of the queue
		~Queue()
		{
			while (!isEmpty())
			{
				delete pop();			
			}
		}	

		/// @brief change the size of the queue
		/// @detail remove all existing elements and create a new queue of a different size
		/// @param size the depth to set the queue to
		/// @param preallocate whether or not to fill the queue with blank elements, defaults to false
		void resize(unsigned size, bool preallocate = false)
		{
			count = 0;
			head = 0;
			tail = 0;
			entry.resize(size);

			if (preallocate)
			{      
				while (count < maxCount)
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
			if (count == maxCount)
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
				tail = (tail + 1) % maxCount; 	//advance tail_ptr
				return true;
			}
		}

		/// @brief treat this queue like an object pool and retrieve an item
		/// @detail if there is no available object, then create one
		/// @return a new item which may or may not be initialized
		T *acquireItem()
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

				entry[head] = NULL; // ensure this item isn't a part of the queue anymore

				head = (head + 1) % maxCount;	//advance head_ptr

				return item;
			}
		}

		/// @brief remove the item from the tail of the queue
		/// @return the item formerly at the tail of the queue, or NULL if the queue is empty
		/// TODO untested
		T* popback()
		{
			if (count == 0)
				return NULL;
			else
			{
				T* theItem = entry[tail];
				count--;
				tail = tail - 1 >= 0 ? tail - 1 : maxCount - 1; // decrease the tail pointer
				return theItem;

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
			return count ? entry[(head + count - 1) % maxCount] : NULL;
		}

		/// @brief get the number of entries currently in this queue
		inline unsigned size() const
		{
			return count;
		}

		/// @brief get the number of entries this queue can hold
		inline unsigned get_depth() const
		{
			return maxCount;
		}

		/// @brief get a pointer to the item at this offset without removing it
		T *read(const unsigned offset) const
		{
			if ((offset >= count) || (offset < 0))
				return NULL;
			else
				return entry[(head + offset) % maxCount];
		}

		/// @brief release item into pool
		/// @detail This is useful for when the queue holds preallocated pieces of memory
		/// and one would like to store them when they are not in use
		void releaseItem(T *item)
		{
			if(!push(item))
			{
				::delete item;
				item = NULL;
			}
		}

		/// @brief this function makes this queue a non-FIFO queue.  
		/// @detail Allows insertion into the middle or at any end
		bool insert(T *item, const int offset)
		{
			assert(offset <= (((int)count) - 1));

			if (count == maxCount)
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
					entry[(head + i + 1) % maxCount] = entry[(head + i) % maxCount];

				count++;

				entry[(head+offset) % maxCount] = item;

				tail = (tail+1) % maxCount;	// advance tail_ptr

				return true;
			}
		}

		/// @brief the number of entries still available in this queue
		unsigned freecount() const
		{
			return maxCount - count;
		}

		/// @brief whether or not there is room for any more entries in this queue
		bool isFull() const
		{
			return (maxCount - count) == 0;
		}

		/// @brief whether or not this queue has no entries in it
		bool isEmpty() const
		{
			return count == 0;
		}

		const T* at(const unsigned value) const
		{
			assert(value >= 0 && value < count);
			return read(value);
		}

		const T* operator[](const unsigned value) const
		{
			return at(value);
		}

		/// @brief do a comparison to see if the queues are equal
		bool operator==(const Queue<T>& right) const
		{
			if (maxCount == right.maxCount && count == right.count && entry.size() == right.entry.size() &&
				head == right.head && tail == right.tail)
			{
				for (unsigned i = 0; i < count; i++)
				{
					if (at(i) && right.at(i))
					{
						if (!(*(at(i)) == *(right.at(i))))
							return false;
					}
					else if (at(i) && !right.at(i) || !at(i) && right.at(i))
						return false;
				}	
				return true;
			}
			else
				return false;
		}

		/// @brief assignment operator overload
		/// @detail moves all the objects from the rhs object to the lhs object
		Queue<T> &operator=(const Queue<T> &right)
		{
			if (&right == this)
				return *this;

			entry.resize(right.maxCount);
			for (unsigned i = 0; i < right.maxCount; i++)
			{
				if (right.entry[i])
					entry[i] = new T(*(right.entry[i]));
			}

			head = right.head;
			tail = right.tail;
			count = right.count;
			maxCount = right.maxCount;

			return *this;
		}

		//friend std::ostream& operator<<(std::ostream&, const Queue<T>&);
		friend std::ostream& operator<< <T>(std::ostream&, const Queue<T>&);


		// serialization
		friend class boost::serialization::access;

		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{			
			ar & maxCount & count & head & tail;	

			ar & entry;
		}
	};

	template<typename T>
	std::ostream& operator<<(std::ostream& in, const Queue<T>& theQueue)
	{
		in << "Queue S[" << std::dec << theQueue.maxCount << "] C[" << std::dec << theQueue.count << "] H[" << std::dec << theQueue.head << "] T[" << std::dec << theQueue.tail << "]" << std::endl;
		if (theQueue.maxCount)
		{
			for (unsigned i = 0; i < theQueue.count; i++)
			{
				in << "\t" << theQueue[i] << std::endl;
			}
		}
		return in;
	}
}
#endif
