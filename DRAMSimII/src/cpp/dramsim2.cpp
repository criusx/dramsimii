#include <cstdlib>
#include <fstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <map>
#include <sstream>

#include "globals.h"
#include "dramSystem.h"
#include "dramSystemConfiguration.h"
#include "dramTimingSpecification.h"
#include "dramAlgorithm.h"
#include "simulationParameters.h"
#include "dramStatistics.h"
#include "busevent.h"
#include "event.h"
#include "rank_c.h"

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





busEvent::busEvent()
{
	timestamp = 0;
	attributes = CONTROL_TRANSACTION;
}

addresses::addresses():
virt_addr(0),
phys_addr(0),
chan_id(0),
rank_id(0),
bank_id(0),
row_id(0),
col_id(0)
{}
