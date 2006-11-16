#include <map>
#include <sstream>
#include "enumTypes.h"
#include "m5-dramSystem.h"


M5dramSystem::M5dramSystem(Params *p)
{
	map<file_io_token_t,std::string> parameter;
	stringstream temp;

	parameter[output_file_token] = p->outFilename;


}