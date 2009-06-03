// Copyright (C) 2008 University of Maryland.
// This file is part of DRAMsimII.
//
// DRAMsimII is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// DRAMsimII is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with DRAMsimII.  If not, see <http://www.gnu.org/licenses/>.

#ifndef QUEUE_H
#define QUEUE_H
#pragma once

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/vector.hpp>

#include <iostream>
#include <cassert>
#include <vector>


namespace DRAMsimII
{
	// forward declaration
	template <typename T>
	class Queue;

	template <typename T>
	std::ostream& operator<<(std::ostream&, const Queue<T>&);


	/// @brief the queue template class, rhs circular queue
	/// @details push/pop are O(1) operations, while random insertions are O(n) operations
	template <typename T>
	class Queue
	{
	private:
		unsigned count;	///< how many elements are in the queue now
		unsigned head;	///< the point where items will be inserted
		unsigned tail;	///< the point where items will be removed
		std::vector<T *> entry;		///< the circular queue
		const bool pool;		///< whether or not this is a pool

	public:
		explicit Queue(): count(0), head(0), tail(0), entry(0), pool(false)
		{}

		/// @brief copy constructor
		/// @details copy the existing queue, making copies of each element
		explicit Queue(const Queue<T>& rhs):
		count(rhs.count),
			head(rhs.head),
			tail(rhs.tail),
			entry(rhs.entry.size()),
			pool(rhs.pool)
		{
			entry.reserve(rhs.entry.size());

			for (unsigned i = 0; i < rhs.count; i++)
			{
				assert(rhs.at(i) != NULL);
				// attempt to copy the contents of this queue
				entry[(head + i) % entry.size()] = new T(*rhs.at(i));
			}

			for (unsigned i = 0; i < count; i++)
				assert(at(i) && rhs.at(i));
		}	

		/// @brief constructor
		/// @details create rhs queue of rhs certain size and optionally fill it with empty elements
		/// @param size the depth of the circular queue
		/// @param preallocate whether or not to fill the queue with blank elements, defaults to false
		explicit Queue(const unsigned size, const bool preallocate = false):
		count(0),
			head(0),
			tail(0),
			entry(size),
			pool(preallocate)
		{
			entry.reserve(size);

			if (preallocate)
			{
				while (count < size)
				{
					push(::new T());
				}
			}
		}

		/// @brief destructor
		/// @details remove the elements and delete them before removing the rest of the queue
		~Queue()
		{
			while (!isEmpty())
			{
				if (pool)
					::delete pop();
				else
					delete pop();			
			}
		}	

		/// @brief change the size of the queue
		/// @details remove all existing elements and create rhs new queue of rhs different size
		/// @param size the depth to set the queue to
		/// @param preallocate whether or not to fill the queue with blank elements, defaults to false
		void resize(unsigned size, bool preallocate = false)
		{
			count = 0;
			head = 0;
			tail = 0;
			entry.resize(size);
			pool = preallocate;

			if (preallocate)
			{
				while (count < entry.size())
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
		/// @details issue rhs warning if the element is null
		/// return false if the queue is already full add to the tail pointer 
		/// position and remove from the head
		/// @param item the item to be inserted into the queue
		bool push(T *item)
		{
			assert(item != NULL);
			if (count == entry.size())
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
				tail = (tail + 1) % entry.size(); 	//advance tail_ptr
				return true;
			}
		}

		/// @brief add an item to the front of the queue efficiently
		bool push_front(T *item)
		{
			assert(item != NULL);
			if (count == entry.size())
				return false;
			else if (item == NULL)
			{
				std::cerr << "Input pointer is NULL" << std::endl;
				return false;
			}
			else
			{
				count++;
				head = ((int)head > 0) ? head - 1 : entry.size() - 1;
				entry[head] = item;
				return true;
			}
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

				entry[head] = NULL; // ensure this item isn't rhs part of the queue anymore

				head = (head + 1) % entry.size();	//advance head_ptr

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
				entry[tail] = NULL;
				count--;
				tail = tail - 1 >= 0 ? tail - 1 : entry.size() - 1; // decrease the tail pointer
				return theItem;

			}
		}

		/// @brief get rhs pointer to the item at the head of the queue
		/// @details similar to peek()
		/// @return rhs pointer to the item at the front of the queue, or NULL if the queue is empty
		const inline T *front() const
		{
#ifdef DEBUG
			assert(count > 0 ? entry[head] != NULL : entry[head] == NULL);
#endif
			return entry[head];
			//return count ? entry[head] : NULL;
		}

		/// @brief to get rhs pointer to the item most recently inserted into the queue
		const T* back() const
		{
			return count ? entry[(head + count - 1) % entry.size()] : NULL;
		}

		/// @brief get the number of entries currently in this queue
		inline unsigned size() const
		{
			return count;
		}

		/// @brief get the number of entries this queue can hold
		inline unsigned depth() const
		{
			return entry.size();
		}

		/// @brief get rhs pointer to the item at this offset without removing it
		T *read(const int offset) const
		{
			if ((offset >= (int)count) || (offset < 0))
				return NULL;
			else
				return entry[(head + offset) % entry.size()];
		}

		/// @brief release item into pool
		/// @details This is useful for when the queue holds preallocated pieces of memory
		/// and one would like to store them when they are not in use
		void releaseItem(T *item)
		{
			//#pragma omp critical
			{

#if 0
				// look around to see if this was already in there, slows things down a lot, so use only when this might be a problem
				for (typename std::vector<T *>::iterator i = entry.begin(); i != entry.end(); i++)
				{
					assert(item != *i);
				}
#endif
				assert(pool);
				if (!push(item))
				{
					::delete item;
					item = NULL;
				}
			}
		}

		/// @brief treat this queue like an object pool and retrieve an item
		/// @details if there is no available object, then create one
		/// @return rhs new item which may or may not be initialized
		T *acquireItem()
		{
			assert(pool);
			T* newItem;
			//#pragma omp critical
			{
				if (count == 0)
				{
					newItem = ::new T;
					assert(newItem != NULL);
				}
				else
					newItem = pop();
			}
			return newItem;
		}

		/// @brief this function makes this queue rhs non-FIFO queue.
		/// @details Allows insertion into the middle or at any end
		bool insert(T *item, const int offset)
		{
			assert(offset <= (int)count);

			if (count == entry.size())
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
					entry[(head + i + 1) % entry.size()] = entry[(head + i) % entry.size()];

				count++;

				entry[(head + offset) % entry.size()] = item;

				tail = (tail + 1) % entry.size();	// advance tail_ptr

				return true;
			}
		}

		T *remove(const int offset)
		{
			assert(offset <= (int)count && offset >= 0);

			// first get the item
			T *item = entry[(head + offset) % entry.size()];
			count--;

			tail = (head + count) % entry.size();

			// then shift the other items up
			for (unsigned i = (unsigned)offset; i < count; i++)
			{
				entry[(head + i) % entry.size()] = entry[(head + i + 1) % entry.size()];
			}


			entry[(head + count) % entry.size()] = NULL;

			return item;
		}

		/// @brief the number of entries still available in this queue
		unsigned freecount() const
		{
			return entry.size() - count;
		}

		/// @brief whether or not there is room for any more entries in this queue
		bool isFull() const
		{
			return (entry.size() == count);
		}

		/// @brief whether or not this queue has no entries in it
		bool isEmpty() const
		{
			return (count == 0);
		}

		const T* at(const unsigned value) const
		{
			assert(value < count);
			return read(value);
		}

		const T* operator[](const unsigned value) const
		{
			return at(value);
		}

		/// @brief do rhs comparison to see if the queues are equal
		bool operator==(const Queue<T>& rhs) const
		{
			if (count == rhs.count && entry.size() == rhs.entry.size() &&
				head == rhs.head && tail == rhs.tail && pool == rhs.pool)
			{
				for (unsigned i = 0; i < count; i++)
				{
					if (at(i) && rhs.at(i))
					{
						if (!(*(at(i)) == *(rhs.at(i))))
							return false;
					}
					else if (at(i) != NULL && rhs.at(i) == NULL)
						return false;
					else if (at(i) == NULL && rhs.at(i) != NULL)
						return false;
				}	
				return true;
			}
			else
				return false;
		}

		/// @brief assignment operator overload
		/// @details moves all the objects from the rhs object to the lhs object
		Queue<T> &operator=(const Queue<T> &rhs)
		{
			if (&rhs == this)
				return *this;

			count = rhs.count;
			head = rhs.head;
			tail = rhs.tail;
			const_cast<bool&>(pool) = rhs.pool;

			entry.resize(rhs.entry.size());

			for (unsigned i = 0; i < rhs.count; i++)
			{
				assert(rhs.at(i));

				entry[(head + i) % entry.size()] = new T(*(rhs.at(i)));
			}

			return *this;
		}

		friend std::ostream& operator<< <T>(std::ostream&, const Queue<T>&);


		// serialization
		friend class boost::serialization::access;

		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & count & head & tail & pool;	

			ar & entry;

			for (unsigned i = 0; i < count; i++)
				assert(at(i));
		}
	};

	template<typename T>
	std::ostream& operator<<(std::ostream& in, const Queue<T>& theQueue)
	{
		in << "Queue S[" << std::dec << theQueue.entry.size() << "] C[" << std::dec << theQueue.count << "] H[" << std::dec << theQueue.head << "] T[" << std::dec << theQueue.tail << "] P[" << theQueue.pool << "]" << std::endl;
		if (theQueue.entry.size())
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
