
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
  queue()
  {
    depth = 0;
    count = 0;
    head_ptr = 0;
    tail_ptr = 0;
    entry = NULL;
  }

  ~queue()
  {
    for (int i=head_ptr;i!=tail_ptr;i=(i+1)%depth)
      delete entry[i];
    delete[] entry;
  }

  queue(const int size, const bool preallocate = false)
  {
    depth = size;
    count = 0;
    head_ptr = 0;
    tail_ptr = 0;
    entry = new T *[size];
    if (preallocate)
    {      
      for (int i = 0; i < size; i++)
        enqueue(new T);
    }
    else
      for (int i=0 ; i<size ; i++)
      entry[i] = NULL;
  }

  void init(const int size)
  {
    depth = size;
    count = 0;
    head_ptr = 0;
    tail_ptr = 0;
    entry = new T *[size];
    for (int i=0 ; i<size ; i++)
      entry[i] = NULL;
  }

  /* queue: When you add stuff, tail pointer increments, if full, return FAILURE flag */
  /* I'm adding to the tail and removing from the head */
input_status_t enqueue(T *item)
  {
    if (count == depth)
      return FAILURE;
    else if (item == NULL)
    {
      cerr << "input transaction pointer is NULL" << endl;
      _exit(2);
    }
    else
    {
      count++;
      entry[tail_ptr] = item;
      tail_ptr	= (tail_ptr+1) % depth; 	/*advance tail_ptr */
      return SUCCESS;
    }
  }


  T *acquire_item()
  {
    T *item = dequeue();
    if (item == NULL)
      return new T;
    else
      return item;
  }

  T *dequeue()
  {
    if(count == 0)
      return NULL;
    else
    {
      count--;
      T *item = entry[head_ptr];
      head_ptr	= (head_ptr+1) % depth; 	/*advance head_ptr */
      return item;
    }
  }

  int get_count() const
  {
    return count;
  }

  T *read(int offset) const
  {
    if((offset >= count) || (offset < 0))
      return NULL;
    else
      return entry[(head_ptr+offset) % depth];
  }

  // release item into pool
  void release_item(T *item)
  {
    if(enqueue(item) == FAILURE)
      delete item;
  }

  // this function makes this queue a non-FIFO queue.  Allows insertion in middle
  input_status_t insert(T *item, int offset)
  {
    if(count == depth)
      return FAILURE;
    else if (item == NULL)
    {
      cerr << "Attempting to insert NULL into queue" << endl;
      _exit(2);
    }
    else
    {
      for(int i = count-1 ; i >= offset ; i++)		/* move everything back by one unit */
        entry[(head_ptr+i+1) % depth] = entry[(head_ptr+i) % depth];
      count++;
      entry[(head_ptr+offset) % depth] = item;
      tail_ptr	= (tail_ptr+1) % depth; 	/*advance tail_ptr */
      return SUCCESS;
    }
  }

  int freecount() const
  {
    return depth - count;
  }

  queue(const queue<T>& a): depth(a.depth), count(0), head_ptr(0), tail_ptr(0)
  {
    T *temp = a.dequeue();
    while (temp)
    {
      enqueue(temp);
      temp=a.dequeue();
    }   
  }
};