#include "simulationParameters.h"

using namespace std;

simulationParameters::simulationParameters(const dramSettings *settings):
input_type(RANDOM),
request_count(settings->requestCount)
{}

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
