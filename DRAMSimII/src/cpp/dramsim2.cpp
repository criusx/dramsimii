#include <cstdlib>
#include <fstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <map>
#include <sstream>

#include "globals.h"
#include "dramsystem.h"
#include "dramSystemConfiguration.h"
#include "dramTimingSpecification.h"
#include "dramAlgorithm.h"
#include "simulationParameters.h"
#include "dramStatistics.h"
#include "busevent.h"
#include "event.h"

using namespace std;




simulationParameters::simulationParameters(map<file_io_token_t,string> &parameter)
{
	request_count = 0;
	
	input_type = RANDOM;
	
	map<file_io_token_t,string>::iterator temp;

	if ((temp=parameter.find(request_count_token))!=parameter.end())
	{
		stringstream temp2(temp->second);
		temp2 >> request_count;
	}
}



/* convert transation to commands, and insert into bank queues in proper sequence if there is room */
/* need to be cleaned up to handle refresh requests */
/* convert transactions to commands */



transaction::transaction()
{
	arrival_time = 0;
	completion_time = 0;
	status = 0;
	event_no = 0;

	type = CONTROL_TRANSACTION;
}

busEvent::busEvent()
{
	timestamp = 0;
	attributes = CONTROL_TRANSACTION;
}

addresses::addresses()
{
	virt_addr = phys_addr = chan_id = rank_id = bank_id = row_id = col_id = 0;
}





event::event()
{
	event_type = 0;
	time = 0;
	event_ptr = NULL;
}
