// pending event queue
class event
{
public:
	int event_type; // what kind of event is this?
	tick_t time;
	void *event_ptr; // point to a transaction or command to be enqueued or executed
	event();
};