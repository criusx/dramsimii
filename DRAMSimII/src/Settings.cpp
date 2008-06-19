#include <libxml/xmlreader.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>

#include "Settings.h"

using namespace std;
using namespace DRAMSimII;

Settings::Settings(const int argc, const char **argv)
{
	// if there are not enough arguments or one is for help, print the help msg and quit
	bool printHelp = false;
	const string help = "help";
	for (int i = argc - 1; i > 0; --i)
	{
		if (help == argv[i])
		{
			printHelp = true;
			break;
		}
	}
	if ((argc < 2) || printHelp)
	{
		cout << "Usage: " << argv[0] << " -options optionswitch" << endl;
		cout << "-input_type [k6|mase|random]" << endl;
		cout << "-trace_file TRACE_FILENAME" << endl;
		cout << "-dram:spd_input SPD_FILENAME" << endl;
		cout << "-output_file OUTPUT_FILENAME" << endl;
		cout << "-debug" << endl;
		exit(0);
	}
	// first find the settings file
	const string settings = "--settings";
	string settingsFile = "";
	for (int i = argc - 1; i >= 0; --i)
	{
		if (settings == argv[i])
		{
			settingsFile = argv[i+1];
			break;
		}
	}
	if (settingsFile == "")
	{
		cout << "No settings file specified, use --settings <filename.xml> " << endl;
		exit (-1);
	}

	xmlTextReader *reader = xmlReaderForFile(
		settingsFile.c_str(),
		NULL,
		XML_PARSE_RECOVER | XML_PARSE_DTDATTR | XML_PARSE_NOENT | XML_PARSE_DTDVALID);

	if (reader == NULL)
	{
		cout << "Unable to open settings file \"" << settingsFile << "\"" << endl;
		exit(-2);
	}
	else
	{
		if (xmlTextReaderIsValid(reader) != 1)
		{
			cerr << "\"" << settingsFile << "\" does not validate." << endl;
			exit(-2);
		}		

		int ret;		
		string nodeName;
		string nodeValue;

		while ((ret = xmlTextReaderRead(reader)) == 1)
		{	
			switch (xmlTextReaderNodeType(reader))
			{
			case XML_ELEMENT_NODE:
				{
					if (!xmlTextReaderConstName(reader))
						break;
					nodeName = (const char *)xmlTextReaderConstName(reader);
					if (nodeName == "dramspec")
					{
						const xmlChar *attr = xmlTextReaderGetAttribute(reader, (xmlChar *)"type");
						string attrS = (const char *)attr;
						if (attrS == "ddr2")
							dramType = DDR2;
						else if (attrS == "ddr")
							dramType = DDR;
						else if (attrS == "ddr3")
							dramType = DDR3;
						else if (attrS == "drdram")
							dramType = DRDRAM;
						else
							dramType = DDR2;
					}
					else if (nodeName == "inputFile")
					{
						const xmlChar *attr = xmlTextReaderGetAttribute(reader, (xmlChar *)"type");
						string type = (const char *)attr;
						if (type == "mase")
							inFileType = MASE_TRACE;
						else if (type == "k6")
							inFileType = K6_TRACE;
						else if (type == "mapped")
							inFileType = MAPPED;
						else if (type == "random")
							inFileType = RANDOM;
					}
					else if (nodeName == "outFile")
					{
						const xmlChar *attr = xmlTextReaderGetAttribute(reader, (xmlChar *)"type");
						const string type = (const char *)attr;
						if (type == "gz" || type == "GZ")
							outFileType = GZ;
						else if (type == "bz" || type == "BZ" || type == "bzip" || type == "bzip2" || type == "bz2")
							outFileType = BZ;
						else if (type == "cout" || type == "stdout" || type == "COUT")
							outFileType = COUT;
						else if (type == "uncompressed" || type == "plain")
							outFileType = UNCOMPRESSED;
						else
							outFileType = NONE;
					}					
				}
				break;
			case XML_TEXT_NODE:				
				if (xmlTextReaderConstName(reader))
				{
					nodeValue = (const char *)xmlTextReaderConstValue(reader);
					
					switch (dramTokenizer(nodeName))
					{
					case unknown_token:
						break;
					case cpu_to_memory_clock_ratio:
						toNumeric<float>(cpuToMemoryClockRatio,nodeValue,std::dec);
						break;
					case input_file_token:
						inFile = nodeValue;
						break;
					case output_file_token:
						outFile = nodeValue;
						break;
					case frequency_spec_token:
						toNumeric<unsigned>(frequencySpec,nodeValue,std::dec);
						break;
					case p_dq_rd_token:
						toNumeric<float>(PdqRD,nodeValue,std::dec);
						break;
					case p_dq_wr_token:
						toNumeric<float>(PdqWR,nodeValue,std::dec);
						break;
					case p_dq_rd_oth_token:
						toNumeric<float>(PdqRDoth,nodeValue,std::dec);
						break;
					case p_dq_wr_oth_token:
						toNumeric<float>(PdqWRoth,nodeValue,std::dec);
						break;
					case dq_per_dram_token:
						toNumeric<unsigned>(DQperDRAM,nodeValue,std::dec);
						break;
					case dqs_per_dram_token:
						toNumeric<unsigned>(DQSperDRAM,nodeValue,std::dec);
						break;
					case dm_per_dram_token:
						toNumeric<unsigned>(DMperDRAM,nodeValue,std::dec);
						break;
					case request_count_token:
						toNumeric<unsigned>(requestCount,nodeValue,std::dec);
						break;
					case idd5_token:
						toNumeric<unsigned>(IDD5,nodeValue,std::dec);
						break;
					case idd4r_token:
						toNumeric<unsigned>(IDD4R,nodeValue,std::dec);
						break;
					case idd4w_token:
						toNumeric<unsigned>(IDD4W,nodeValue,std::dec);
						break;
					case idd3n_token:
						toNumeric<unsigned>(IDD3N,nodeValue,std::dec);
						break;
					case idd3p_token:
						toNumeric<unsigned>(IDD3P,nodeValue,std::dec);
						break;
					case idd2n_token:
						toNumeric<unsigned>(IDD2N,nodeValue,std::dec);
						break;
					case idd2p_token:
						toNumeric<unsigned>(IDD2P,nodeValue,std::dec);
						break;
					case idd0_token:
						toNumeric<unsigned>(IDD0,nodeValue,std::dec);
						break;
					case vdd_token:
						toNumeric<float>(VDD,nodeValue,std::dec);
						break;
					case t_cwd_token:
						toNumeric<unsigned>(tCWD,nodeValue,std::dec);
						break;
					case t_buffer_delay_token:
						toNumeric<unsigned>(tBufferDelay,nodeValue,std::dec);
						break;
					case t_cmd_token:
						toNumeric<unsigned>(tCMD,nodeValue,std::dec);
						break;
					case t_wtr_token:
						toNumeric<unsigned>(tWTR,nodeValue,std::dec);
						break;
					case t_wr_token:
						toNumeric<unsigned>(tWR,nodeValue,std::dec);
						break;
					case t_refi_token:
						toNumeric<unsigned>(tREFI,nodeValue,std::dec);
						break;
					case t_rtrs_token:
						toNumeric<unsigned>(tRTRS,nodeValue,std::dec);
						break;
					case t_rtp_token:
						toNumeric<unsigned>(tRTP,nodeValue,std::dec);
						break;
					case t_rrd_token:
						toNumeric<unsigned>(tRRD,nodeValue,std::dec);
						break;
					case t_rfc_token:
						toNumeric<unsigned>(tRFC,nodeValue,std::dec);
						break;
					case t_rcd_token:
						toNumeric<unsigned>(tRCD,nodeValue,std::dec);
						break;
					case t_rc_token:
						toNumeric<unsigned>(tRC,nodeValue,std::dec);
						break;
					case t_rp_token:
						toNumeric<unsigned>(tRP,nodeValue,std::dec);
						break;
					case t_ras_token:
						toNumeric<unsigned>(tRAS,nodeValue,std::dec);
						break;
					case t_faw_token:
						toNumeric<unsigned>(tFAW,nodeValue,std::dec);
						break;
					case t_cas_token:
						toNumeric<unsigned>(tCAS,nodeValue,std::dec);
						break;
					case t_burst_token:
						toNumeric<unsigned>(tBurst,nodeValue,std::dec);
						break;
					case t_al_token:
						toNumeric<unsigned>(tAL,nodeValue,std::dec);
						break;
					case refresh_queue_depth_token:
						toNumeric<unsigned>(refreshQueueDepth,nodeValue,std::dec);
						break;
					case bank_count_token:
						toNumeric<unsigned>(bankCount,nodeValue,std::dec);
						break;
					case rank_count_token:
						toNumeric<unsigned>(rankCount,nodeValue,std::dec);
						break;
					case chan_count_token:
						toNumeric<unsigned>(channelCount,nodeValue,std::dec);
						break;
					case short_burst_ratio_token:
						toNumeric<float>(shortBurstRatio,nodeValue,std::dec);
						break;
					case read_percentage_token:
						toNumeric<float>(readPercentage,nodeValue,std::dec);
						break;					
					case per_bank_queue_depth_token:
						toNumeric<unsigned>(perBankQueueDepth,nodeValue,std::dec);
						break;
					case event_queue_depth_token:
						toNumeric<unsigned>(eventQueueDepth,nodeValue,std::dec);
						break;
					case transaction_queue_depth_token:
						toNumeric<unsigned>(transactionQueueDepth,nodeValue,std::dec);
						break;
					case completion_queue_depth_token:
						toNumeric<unsigned>(completionQueueDepth,nodeValue,std::dec);
						break;
					case history_queue_depth_token:
						toNumeric<unsigned>(historyQueueDepth,nodeValue,std::dec);
						break;
					case cacheline_size_token:
						toNumeric<unsigned>(cacheLineSize,nodeValue,std::dec);
						break;
					case row_size_token:
						toNumeric<unsigned>(rowSize,nodeValue,std::dec);
						break;
					case col_size_token:
						toNumeric<unsigned>(columnSize,nodeValue,std::dec);
						break;
					case channel_width_token:
						toNumeric<unsigned>(channelWidth,nodeValue,std::dec);
						break;
					case col_count_token:
						toNumeric<unsigned>(columnCount,nodeValue,std::dec);
						break;
					case row_count_token:
						toNumeric<unsigned>(rowCount,nodeValue,std::dec);
						break;
					case cachelines_per_row_token:
						toNumeric<unsigned>(cachelinesPerRow,nodeValue,std::dec);
						break;
					case posted_cas_token:
						postedCAS = (nodeValue == "true") ? true : false;
						break;
					case seniority_age_limit_token:
						toNumeric<unsigned>(seniorityAgeLimit,nodeValue,std::dec);
						break;
					case refresh_policy_token:
						if (nodeValue == "none" || nodeValue == "no refresh")
							refreshPolicy = NO_REFRESH;
						else if (nodeValue == "bankConcurrent")
							refreshPolicy = BANK_CONCURRENT;
						else if (nodeValue == "bankStaggeredHidden")
							refreshPolicy = BANK_STAGGERED_HIDDEN;
						else if (nodeValue == "refreshOneChanAllRankAllBank")
							refreshPolicy = ONE_CHANNEL_ALL_RANK_ALL_BANK;
						else
							refreshPolicy = NO_REFRESH;
						break;
					case read_write_grouping_token:
						readWriteGrouping = (nodeValue == "true") ? true : false;
						break;
					case refresh_time_token:
						toNumeric<unsigned>(refreshTime,nodeValue,std::dec);
						break;
					case system_configuration_type_token:
						// TODO: if baseline, then normal system, if FBD, then make a FBD system

						break;
					case transaction_ordering_policy_token:
						if (nodeValue == "strict")
							transactionOrderingAlgorithm = STRICT;
						else if (nodeValue == "RIFF" || nodeValue == "riff")
							transactionOrderingAlgorithm = RIFF;
						else
							transactionOrderingAlgorithm = STRICT;						
						break;
					case command_ordering_algorithm_token:
						if (nodeValue == "strict")
							commandOrderingAlgorithm = STRICT_ORDER;
						else if (nodeValue == "bankRoundRobin")
							commandOrderingAlgorithm = BANK_ROUND_ROBIN;
						else if (nodeValue == "rankRoundRobin")
							commandOrderingAlgorithm = RANK_ROUND_ROBIN;
						else if (nodeValue == "wangHop")
							commandOrderingAlgorithm = WANG_RANK_HOP;
						else if (nodeValue == "greedy")
							commandOrderingAlgorithm = GREEDY;
						else {
							cerr << "Unrecognized ordering algorithm: " << nodeValue << endl;
							commandOrderingAlgorithm = BANK_ROUND_ROBIN;
						}
						break;
					case addr_mapping_scheme_token:
						if (nodeValue == "burgerBase")
							addressMappingScheme = BURGER_BASE_MAP;
						else if (nodeValue == "closePageBaseline")
							addressMappingScheme = CLOSE_PAGE_BASELINE;
						else if (nodeValue == "intel845g")
							addressMappingScheme = INTEL845G_MAP;
						else if (nodeValue == "sdramBase")
							addressMappingScheme = SDRAM_BASE_MAP;
						else if (nodeValue == "sdramClosePage")
							addressMappingScheme = SDRAM_CLOSE_PAGE_MAP;
						else if (nodeValue == "sdramHiperf")
							addressMappingScheme = SDRAM_HIPERF_MAP;
						else
							addressMappingScheme = SDRAM_HIPERF_MAP;
						break;
					case auto_precharge_token:
						autoPrecharge = (nodeValue == "true") ? true : false;
						break;
					case row_buffer_management_policy_token:
						if (nodeValue == "openPage")
							rowBufferManagementPolicy = OPEN_PAGE;
						else if (nodeValue == "closePage")
							rowBufferManagementPolicy = CLOSE_PAGE;
						else if (nodeValue == "closePageOptimized")
							rowBufferManagementPolicy = CLOSE_PAGE_OPTIMIZED;
						else
							rowBufferManagementPolicy = AUTO_PAGE;
						break;
					case clock_granularity_token:
						toNumeric<unsigned>(clockGranularity,nodeValue,std::dec);
						break;
					case datarate_token:
						toNumeric<unsigned>(dataRate,nodeValue,std::dec);
						break;
					case max_vcc_token:
						toNumeric<float>(maxVCC,nodeValue,std::dec);
						break;					
					default:
						break;
					}
				}
			case XML_CDATA_SECTION_NODE:
				break;
			}
		}

		if ((ret == -1) || (xmlTextReaderIsValid(reader) != 1))
		{
			cerr << "There was an error reading/parsing " << settingsFile << "." << endl;
			exit(-2);
		}
		else
		{
			ifstream settingsInFile;
			settingsInFile.open(settingsFile.c_str());		

			if (settingsInFile.is_open())
			{
				char currentWord[8192];
				while (!settingsInFile.eof())
				{
					settingsInFile.read(currentWord,8192);
					int lengthRead = settingsInFile.gcount();
					settingsOutputFile.append(currentWord,lengthRead);
				}
				settingsInFile.close();
			}
			else
			{
				cerr << "Could not create output for settings file" << endl;
				exit(-2);
			}
		}
		// close the reader
		xmlFreeTextReader(reader);
	}

	

	
}
