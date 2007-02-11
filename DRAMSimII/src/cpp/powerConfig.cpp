#include <map>
#include <sstream>
#include <string>

#include "globals.h"
#include "enumTypes.h"
#include "powerConfig.h"





using namespace std;



powerConfig::powerConfig(std::map<file_io_token_t,std::string> &parameter)
{
	std::map<file_io_token_t, std::string>::iterator temp;

	// defaults based on the Micron MT47H128M8BT-3
	// 1Gb, DDR2@333MHz, x8
	VDD = 1.8;
	IDD0 = 90;
	IDD2P = 7;
	IDD2N = 60;
	IDD3P = 40;
	IDD3N = 70;
	IDD4R = 160;
	IDD4W = 160;
	IDD5 = 270;

	if ((temp=parameter.find(vdd_token))!=parameter.end())
	{
		toNumeric<float>(VDD,temp->second,std::dec);
	}

	if ((temp=parameter.find(idd0_token))!=parameter.end())
	{
		toNumeric<int>(IDD0,temp->second,std::dec);
	}

	if ((temp=parameter.find(idd2p_token))!=parameter.end())
	{
		toNumeric<int>(IDD2P,temp->second,std::dec);
	}

	if ((temp=parameter.find(idd2n_token))!=parameter.end())
	{
		toNumeric<int>(IDD2N,temp->second,std::dec);
	}

	if ((temp=parameter.find(idd3p_token))!=parameter.end())
	{
		toNumeric<int>(IDD3P,temp->second,std::dec);
	}

	if ((temp=parameter.find(idd3n_token))!=parameter.end())
	{
		toNumeric<int>(IDD3N,temp->second,std::dec);
	}

	if ((temp=parameter.find(idd4r_token))!=parameter.end())
	{
		toNumeric<int>(IDD4R,temp->second,std::dec);
	}

	if ((temp=parameter.find(idd4w_token))!=parameter.end())
	{
		toNumeric<int>(IDD4W,temp->second,std::dec);
	}

	if ((temp=parameter.find(idd5_token))!=parameter.end())
	{
		toNumeric<int>(IDD5,temp->second,std::dec);
	}

}

powerConfig::~powerConfig(void)
{
}

void recordCommand(const command *)
{

}
