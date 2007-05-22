#ifndef DRAMSETTINGS
#define DRAMSETTINGS

#include <string>
#include "enumTypes.h"

class dramSettings
{
public:
	//////////////////////////////////////////////////////////////////////////	
	// command data
	std::string inFile;
	input_type_t inFileType;
	std::string outFile;
	output_file_t outFileType;
	unsigned requestCount;
	refresh_policy_t refreshPolicy;
	dram_type_t dramType;
	unsigned dataRate; // frequency
	command_ordering_algorithm_t commandOrderingAlgorithm;
	transaction_ordering_algorithm_t transactionOrderingAlgorithm;
	unsigned perBankQueueDepth;
	unsigned columnSize;
	unsigned rowSize;
	unsigned channelWidth;
	unsigned columnCount;
	unsigned rowCount;
	unsigned cacheLineSize;
	unsigned historyQueueDepth;
	unsigned completionQueueDepth;
	unsigned transactionQueueDepth;
	unsigned eventQueueDepth;
	unsigned refreshQueueDepth;
	unsigned refreshTime;
	unsigned seniorityAgeLimit;
	row_buffer_policy_t rowBufferManagementPolicy;
	address_mapping_scheme_t addressMappingScheme;
	bool postedCAS;
	bool readWriteGrouping;
	bool autoPrecharge;
	unsigned clockGranularity;
	unsigned cachelinesPerRow;
	unsigned channelCount;
	unsigned rankCount;
	unsigned bankCount;
	float shortBurstRatio;
	float readPercentage;
	//////////////////////////////////////////////////////////////////////////	
	// timing data
	unsigned tRTRS;
	unsigned tAL;
	unsigned tBurst;
	unsigned tCAS;
	unsigned tCWD;
	unsigned tFAW;
	unsigned tRAS;
	unsigned tRC;
	unsigned tRCD;
	unsigned tREFI;
	unsigned tRFC;
	unsigned tRP;
	unsigned tRRD;
	unsigned tRTP;
	unsigned tWR;
	unsigned tWTR;
	unsigned tCMD;
	unsigned tInternalBurst;
	unsigned tBufferDelay;
	//////////////////////////////////////////////////////////////////////////
	// power data
	float PdqRD;
	float PdqWR;
	float PdqRDoth;
	float PdqWRoth;
	unsigned DQperDRAM;
	unsigned DQSperDRAM;
	unsigned DMperDRAM;
	unsigned frequencySpec;
	float maxVCC;
	float VDD;
	unsigned IDD0;
	unsigned IDD2P;
	unsigned IDD2N;
	unsigned IDD3P;
	unsigned IDD3N;
	unsigned IDD4W;
	unsigned IDD4R;
	unsigned IDD5;


	explicit dramSettings(const int, const char **);
};
#endif
