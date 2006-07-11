class dramSystem
{
private:
	dram_system_configuration system_config;
	dram_timing_specification timing_specification;
	simulation_parameters sim_parameters;
	dram_statistics statistics;
	dram_algorithm algorithm;

	string output_filename;
	input_stream_c input_stream;
	dram_channel *channel;
	tick_t time; // master clock
	queue<command> free_command_pool;	// command objects are stored here to avoid allocating memory after initialization
	queue<transaction> free_transaction_pool;	// transactions are stored here to avoid allocating memory after initialization
	queue<event> free_event_pool;	// same for events
	queue<event> event_q;	// pending event queue	

	void read_dram_config_from_file();

	void set_dram_timing_specification(enum dram_type_t);
	command *getNextCommand(const int);
	enum input_status_t transaction2commands(transaction*);
	int minProtocolGap(const int,const command *) const;
	int find_oldest_channel() const;
	void executeCommand(command *, const int);
	void update_system_time();
	int convert_address(addresses &);
	enum input_status_t get_next_input_transaction(transaction *&);
	void get_next_random_request(transaction *);

public:
	explicit dramSystem(map<file_io_token_t,string> &);
	~dramSystem();
	friend ostream &operator<<(ostream &, const dramSystem &);
	void run_simulations();
	void run_simulations2();
};