class transaction
{
public:
	int event_no;
	transaction_type_t type;	// transaction type
	int status;
	int length;					// how long?
	tick_t arrival_time;		// time when first seen by memory controller in DRAM ticks
	tick_t completion_time;		// time when transaction has completed in DRAM ticks
	addresses addr;
	transaction();
	friend ostream &operator<<(ostream &, const transaction *);
};