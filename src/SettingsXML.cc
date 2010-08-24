// Copyright (C) 2010 University of Maryland.
// This file is part of DRAMsimII.
//
// DRAMsimII is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// DRAMsimII is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with DRAMsimII.  If not, see <http://www.gnu.org/licenses/>.

#include <boost/random.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/program_options/option.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/lexical_cast.hpp>
#include <libxml/xmlreader.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/encoding.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "globals.hh"
#include "Settings.hh"

using namespace std;
using namespace DRAMsimII;
using boost::is_any_of;
using boost::token_compress_on;
using boost::split;
using boost::lexical_cast;

//////////////////////////////////////////////////////////////////////////
/// @brief update the value in the settings file with a new value at runtime
//////////////////////////////////////////////////////////////////////////
bool updateKeyValue(const string &nodeName, const string &nodeValue, xmlDocPtr doc)
{
	FileIOToken token = unknown_token;
	if (Settings::tokenize(nodeName, token) == false)
		return false;
	else
	{
		string realName;
		if (Settings::detokenize(token, realName) == true)
		{
			xmlXPathContextPtr context = xmlXPathNewContext(doc);
			if (context == NULL)
			{
				cerr << "Problem updating nodes." << endl;				
				return false;
			}
			//string lookupValue = (realName.find('@') < 0) ? "//" + realName : realName;
			string lookupValue = "//" + realName;

			xmlXPathObjectPtr path = xmlXPathEvalExpression((const xmlChar *)lookupValue.c_str(), context);
			if (path == NULL)
			{
				cerr << "Problem creating node path." << endl;
				xmlXPathFreeContext(context);
				return false;
			}
			//updateNodes(path->nodesetval,(const xmlChar *)nodeValue.c_str());
			int size = (path->nodesetval) ? path->nodesetval->nodeNr : 0;

			for (int i = size - 1; i >= 0; i--)
			{
				assert(path->nodesetval->nodeTab[i]);

				xmlNodeSetContent(path->nodesetval->nodeTab[i], (const xmlChar *)nodeValue.c_str());
			}

			xmlXPathFreeObject(path);
			xmlXPathFreeContext(context);

			return true;
		}
		else
			return false;
	}
}

namespace opt = boost::program_options;

//////////////////////////////////////////////////////////////////////////
/// @brief construct a set of settings based upon parameters passed from the command line
/// @details will parse parameters passed from the command line without modification
/// and print any errors that occur
//////////////////////////////////////////////////////////////////////////
Settings::Settings(int argc, char **argv):
addressMappingPolicy(Address::SDRAM_BASE_MAP),
cachelinesPerRow(0),
tInternalBurst(0)
{
	loadSettingsFromFile(argc, argv);
}

//////////////////////////////////////////////////////////////////////////
/// @brief load a default settings file, update it with current settings and then modified settings
//////////////////////////////////////////////////////////////////////////
bool Settings::loadSettings(vector<string> &settingsList)
{
	// first update the current settings with the settings list passed in
	if (settingsList.size() > 0)
	{
		if (settingsList.size() % 2 != 0)
		{
			cerr << "Invalid memory override format, should be {<parameter name> <value>}+" << endl;
			cerr << settingsList.size() << endl;
			exit(-14);
		}

		for (vector<string>::size_type i = 0; i < settingsList.size(); i += 2)
		{
			if (!setKeyValue(settingsList[i], settingsList[i+1]) )
				cerr << "warn: Unrecognized key/value pair (" << settingsList[i] << "," << settingsList[i+1] << ")" << endl;
			else
				cerr << "note: setting " << settingsList[i] << "=" << settingsList[i+1] << endl;
		}
	}

	// then generate a valid xml document of the existing settings and store these as a string
	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	xmlNodePtr rootNode = xmlNewNode(NULL, BAD_CAST "dramspec");
	xmlDocSetRootElement(doc,rootNode);
	/// @TODO make this more general
	xmlNewProp(rootNode, BAD_CAST "type", BAD_CAST "ddr3");
	xmlNodePtr node = NULL;

	//xmlDtdPtr dtd = 
		xmlCreateIntSubset(doc, BAD_CAST "dramspec", NULL, BAD_CAST "dramspec.dtd");

	// create the setup parameter section
	node = xmlNewChild(rootNode, NULL, BAD_CAST "setup", NULL);
	xmlNewChild(node, NULL, BAD_CAST "datarate", (const xmlChar *)lexical_cast<string>(dataRate).c_str());
	xmlNewChild(node, NULL, BAD_CAST "clockGranularity", (const xmlChar *)lexical_cast<string>(clockGranularity).c_str());
	xmlNewChild(node, NULL, BAD_CAST "channels", (const xmlChar *)lexical_cast<string>(channelCount).c_str());
	xmlNewChild(node, NULL, BAD_CAST "dimms", (const xmlChar *)lexical_cast<string>(dimmCount).c_str());
	xmlNewChild(node, NULL, BAD_CAST "ranks", (const xmlChar *)lexical_cast<string>(rankCount).c_str());
	xmlNewChild(node, NULL, BAD_CAST "banks", (const xmlChar *)lexical_cast<string>(bankCount).c_str());
	xmlNewChild(node, NULL, BAD_CAST "rows", (const xmlChar *)lexical_cast<string>(rowCount).c_str());
	xmlNewChild(node, NULL, BAD_CAST "columns", (const xmlChar *)lexical_cast<string>(columnCount).c_str());
	xmlNewChild(node, NULL, BAD_CAST "channelWidth", (const xmlChar *)lexical_cast<string>(channelWidth).c_str());
	xmlNewChild(node, NULL, BAD_CAST "physicalAddressMappingPolicy", (const xmlChar *)lexical_cast<string>(addressMappingPolicy).c_str());
	xmlNewChild(node, NULL, BAD_CAST "rowBufferPolicy", (const xmlChar *)lexical_cast<string>(rowBufferManagementPolicy).c_str());
	xmlNewChild(node, NULL, BAD_CAST "rowSize", (const xmlChar *)lexical_cast<string>(rowSize).c_str());
	xmlNewChild(node, NULL, BAD_CAST "columnSize", (const xmlChar *)lexical_cast<string>(columnSize).c_str());
	xmlNewChild(node, NULL, BAD_CAST "postedCAS", (const xmlChar *)(postedCAS ? "true" : "false"));
	xmlNewChild(node, NULL, BAD_CAST "autoRefreshPolicy", (const xmlChar *)lexical_cast<string>(refreshPolicy).c_str());
	xmlNewChild(node, NULL, BAD_CAST "commandOrderingAlgorithm", (const xmlChar *)lexical_cast<string>(commandOrderingAlgorithm).c_str());
	xmlNewChild(node, NULL, BAD_CAST "transactionOrderingAlgorithm", (const xmlChar *)lexical_cast<string>(transactionOrderingAlgorithm).c_str());
	xmlNewChild(node, NULL, BAD_CAST "perBankQueueDepth", (const xmlChar *)lexical_cast<string>(perBankQueueDepth).c_str());
	xmlNewChild(node, NULL, BAD_CAST "cacheLineSize", (const xmlChar *)lexical_cast<string>(cacheLineSize).c_str());
	xmlNewChild(node, NULL, BAD_CAST "transactionQueueDepth", (const xmlChar *)lexical_cast<string>(transactionQueueDepth).c_str());
	xmlNewChild(node, NULL, BAD_CAST "decodeWindow", (const xmlChar *)lexical_cast<string>(decodeWindow).c_str());
	xmlNewChild(node, NULL, BAD_CAST "readWriteGrouping", (const xmlChar *)(readWriteGrouping ? "true" : "false"));
	xmlNewChild(node, NULL, BAD_CAST "autoPrecharge", (const xmlChar *)(autoPrecharge ? "true" : "false"));

	// generate the cache settings section
	node = xmlNewChild(node, NULL, BAD_CAST "cache", NULL);
	xmlNewChild(node, NULL, BAD_CAST "associativity", (const xmlChar *)lexical_cast<string>(associativity).c_str());
	xmlNewChild(node, NULL, BAD_CAST "cacheSize", (const xmlChar *)lexical_cast<string>(cacheSize).c_str());
	xmlNewChild(node, NULL, BAD_CAST "blockSize", (const xmlChar *)lexical_cast<string>(blockSize).c_str());
	xmlNewChild(node, NULL, BAD_CAST "hitLatency", (const xmlChar *)lexical_cast<string>(hitLatency).c_str());
	xmlNewChild(node, NULL, BAD_CAST "replacementPolicy", (const xmlChar *)lexical_cast<string>(replacementPolicy).c_str());
	xmlNewChild(node, NULL, BAD_CAST "hitLatency", (const xmlChar *)lexical_cast<string>(nmruTrackingCount).c_str());
	xmlNewChild(node, NULL, BAD_CAST "usingCache", (const xmlChar *)(usingCache ? "true" : "false"));
	xmlNewChild(node, NULL, BAD_CAST "fixedCacheLatency", (const xmlChar *)(fixedCacheLatency ? "true" : "false"));

	// create the timing parameter section
	node = xmlNewChild(rootNode, NULL, BAD_CAST "timing", NULL);
	xmlNewChild(node, NULL, BAD_CAST "tBufferDelay", (const xmlChar *)lexical_cast<string>(tBufferDelay).c_str());
	xmlNewChild(node, NULL, BAD_CAST "tBurst", (const xmlChar *)lexical_cast<string>(tBurst).c_str());
	xmlNewChild(node, NULL, BAD_CAST "tCAS", (const xmlChar *)lexical_cast<string>(tCAS).c_str());
	xmlNewChild(node, NULL, BAD_CAST "tCMD", (const xmlChar *)lexical_cast<string>(tCMD).c_str());
	xmlNewChild(node, NULL, BAD_CAST "tCWD", (const xmlChar *)lexical_cast<string>(tCWD).c_str());
	xmlNewChild(node, NULL, BAD_CAST "tFAW", (const xmlChar *)lexical_cast<string>(tFAW).c_str());
	xmlNewChild(node, NULL, BAD_CAST "tRAS", (const xmlChar *)lexical_cast<string>(tRAS).c_str());
	xmlNewChild(node, NULL, BAD_CAST "tRC", (const xmlChar *)lexical_cast<string>(tRC).c_str());
	xmlNewChild(node, NULL, BAD_CAST "tRCD", (const xmlChar *)lexical_cast<string>(tRCD).c_str());
	xmlNewChild(node, NULL, BAD_CAST "tRFC", (const xmlChar *)lexical_cast<string>(tRFC).c_str());
	xmlNewChild(node, NULL, BAD_CAST "tRRD", (const xmlChar *)lexical_cast<string>(tRRD).c_str());
	xmlNewChild(node, NULL, BAD_CAST "tRP", (const xmlChar *)lexical_cast<string>(tRP).c_str());
	xmlNewChild(node, NULL, BAD_CAST "tRTP", (const xmlChar *)lexical_cast<string>(tRTP).c_str());
	xmlNewChild(node, NULL, BAD_CAST "tRTRS", (const xmlChar *)lexical_cast<string>(tRTRS).c_str());
	xmlNewChild(node, NULL, BAD_CAST "tWR", (const xmlChar *)lexical_cast<string>(tWR).c_str());
	xmlNewChild(node, NULL, BAD_CAST "tWTR", (const xmlChar *)lexical_cast<string>(tWTR).c_str());
	xmlNewChild(node, NULL, BAD_CAST "tAL", (const xmlChar *)lexical_cast<string>(tAL).c_str());
	xmlNewChild(node, NULL, BAD_CAST "tREFI", (const xmlChar *)lexical_cast<string>(tREFI).c_str());
	xmlNewChild(node, NULL, BAD_CAST "seniorityAgeLimit", (const xmlChar *)lexical_cast<string>(seniorityAgeLimit).c_str());


	// create the power parameter section
	node = xmlNewChild(rootNode, NULL, BAD_CAST "power", NULL);
	xmlNewChild(node, NULL, BAD_CAST "PdqRD", (const xmlChar *)lexical_cast<string>(PdqRD).c_str());
	xmlNewChild(node, NULL, BAD_CAST "PdqWR", (const xmlChar *)lexical_cast<string>(PdqWR).c_str());
	xmlNewChild(node, NULL, BAD_CAST "PdqRDoth", (const xmlChar *)lexical_cast<string>(PdqRDoth).c_str());
	xmlNewChild(node, NULL, BAD_CAST "PdqWRoth", (const xmlChar *)lexical_cast<string>(PdqWRoth).c_str());
	xmlNewChild(node, NULL, BAD_CAST "DQperDRAM", (const xmlChar *)lexical_cast<string>(DQperDRAM).c_str());
	xmlNewChild(node, NULL, BAD_CAST "DQSperDRAM", (const xmlChar *)lexical_cast<string>(DQSperDRAM).c_str());
	xmlNewChild(node, NULL, BAD_CAST "DMperDRAM", (const xmlChar *)lexical_cast<string>(DMperDRAM).c_str());
	xmlNewChild(node, NULL, BAD_CAST "frequencySpec", (const xmlChar *)lexical_cast<string>(frequencySpec).c_str());
	xmlNewChild(node, NULL, BAD_CAST "maxVCC", (const xmlChar *)lexical_cast<string>(maxVCC).c_str());
	xmlNewChild(node, NULL, BAD_CAST "systemVDD", (const xmlChar *)lexical_cast<string>(VDD).c_str());
	xmlNewChild(node, NULL, BAD_CAST "IDD0", (const xmlChar *)lexical_cast<string>(IDD0).c_str());
	xmlNewChild(node, NULL, BAD_CAST "IDD1", (const xmlChar *)lexical_cast<string>(IDD1).c_str());
	xmlNewChild(node, NULL, BAD_CAST "IDD2P", (const xmlChar *)lexical_cast<string>(IDD2P).c_str());
	xmlNewChild(node, NULL, BAD_CAST "IDD2N", (const xmlChar *)lexical_cast<string>(IDD2N).c_str());
	xmlNewChild(node, NULL, BAD_CAST "IDD3P", (const xmlChar *)lexical_cast<string>(IDD3P).c_str());
	xmlNewChild(node, NULL, BAD_CAST "IDD3N", (const xmlChar *)lexical_cast<string>(IDD3N).c_str());
	xmlNewChild(node, NULL, BAD_CAST "IDD4R", (const xmlChar *)lexical_cast<string>(IDD4R).c_str());
	xmlNewChild(node, NULL, BAD_CAST "IDD4W", (const xmlChar *)lexical_cast<string>(IDD4W).c_str());
	xmlNewChild(node, NULL, BAD_CAST "IDD5A", (const xmlChar *)lexical_cast<string>(IDD5).c_str());


	// create the simulation parameter section
	node = xmlNewChild(rootNode, NULL, BAD_CAST "simulationParameters", NULL);
	xmlNewChild(node, NULL, BAD_CAST "shortBurstRatio", (const xmlChar *)lexical_cast<string>(shortBurstRatio).c_str());
	xmlNewChild(node, NULL, BAD_CAST "readPercentage", (const xmlChar *)lexical_cast<string>(readPercentage).c_str());
	xmlNewChild(node, NULL, BAD_CAST "averageInterarrivalCycleCount", (const xmlChar *)lexical_cast<string>(averageInterarrivalCycleCount).c_str());
	xmlNodePtr node2 = xmlNewChild(node, NULL, BAD_CAST "outFile", (const xmlChar *)lexical_cast<string>(outFile).c_str());
	xmlNewProp(node2, BAD_CAST "type", (const xmlChar *)lexical_cast<string>(outFileType).c_str());
	xmlNewProp(node2, BAD_CAST "dbreporting", BAD_CAST "false");
	xmlNewChild(node, NULL, BAD_CAST "outFileDirectory", (const xmlChar *)lexical_cast<string>(outFileDir).c_str());
	xmlNewChild(node, NULL, BAD_CAST "requestCount", (const xmlChar *)lexical_cast<string>(requestCount).c_str());
	node2 = xmlNewChild(node, NULL, BAD_CAST "inputFile", (const xmlChar *)lexical_cast<string>(inFile).c_str());
	xmlNewProp(node2, BAD_CAST "type", (const xmlChar *)lexical_cast<string>(inFileType).c_str());
	xmlNewChild(node, NULL, BAD_CAST "cpuToMemoryClockRatio", (const xmlChar *)lexical_cast<string>(cpuToMemoryClockRatio).c_str());
	xmlNewChild(node, NULL, BAD_CAST "epoch", (const xmlChar *)lexical_cast<string>(epoch).c_str());
	
	int len;
	xmlChar *buffer;
	xmlDocDumpFormatMemoryEnc(doc, &buffer, &len, "UTF-8", 1);
	settingsOutputFile = (const char *)buffer;
	xmlFree(buffer);

	xmlFreeDoc(doc);

	xmlCleanupParser();

	return true;
}

//////////////////////////////////////////////////////////////////////////
/// @brief load settings from an xml file
/// @details loads and populates a Settings object based on a particular
/// settings file and updates the settings based upon the modifiers
/// sent to it
/// @returns true if no errors were encountered, false otherwise
//////////////////////////////////////////////////////////////////////////
bool Settings::loadSettingsFromFile(int argc, char **argv)
{

	opt::options_description desc("Basic options");
	string settingsFile;
	string extraSettings;
	desc.add_options()
		("help", "help message")
		("config-file",opt::value<string>(&settingsFile),"The configuration file")
		("modifiers", opt::value<string>(&extraSettings)->default_value(""), "Modifiers to the settings file");

	opt::variables_map vm;

	opt::store(opt::parse_command_line(argc, argv, desc), vm);
	opt::notify(vm);

	if (vm.count("help"))
	{
		cout << "Usage: " << argv[0] << "(--help | --config-file <configuration file> {--modifiers \"<modifiers string>}\")" << endl;
		cout << "Where modifiers strings look like (parameter value)+" << endl;
	}

	if (!vm.count("config-file"))
	{
		cout << "A configuration file MUST be specified (--config-file <filename>)" << endl;
		exit(-1);
	}

	//#define USEREADERFORMEMORY

	ifstream xmlFile(settingsFile.c_str(),ios::in|ios::ate);

	xmlTextReader *reader = NULL;

	xmlDocPtr doc = NULL;

	if (xmlFile.is_open())
	{		
		doc = xmlParseFile(settingsFile.c_str());
		if (doc == NULL)
		{
			cout << "Unable to open settings file \"" << settingsFile << "\"" << endl;
			exit(-2);
		}
#ifdef USEREADERFORMEMORY
		ifstream::pos_type entireXmlFileLength = 0;

		entireXmlFileLength = xmlFile.tellg();
		char *entireXmlFile = new char[entireXmlFileLength];

		xmlFile.seekg(0,ios::beg);
		xmlFile.read(entireXmlFile,entireXmlFileLength);

		xmlFile.close();

		entireXmlFile[entireXmlFileLength] = 0;

		reader = xmlReaderForMemory(
			entireXmlFile,entireXmlFileLength,
			NULL,NULL,
			XML_PARSE_RECOVER | XML_PARSE_DTDATTR | XML_PARSE_NOENT | XML_PARSE_DTDVALID);	
		delete[] entireXmlFile;
#else
		reader = xmlReaderForFile(
			settingsFile.c_str(),
			NULL,
			XML_PARSE_RECOVER | XML_PARSE_DTDATTR | XML_PARSE_NOENT | XML_PARSE_DTDVALID);
#endif	

	}

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
						xmlChar *attr = xmlTextReaderGetAttribute(reader, (xmlChar *)"type");
						string attrS = (const char *)attr;
#ifndef NDEBUG
						bool result =
#endif
							setKeyValue(nodeName,attrS);
						assert(result);
						xmlFree(attr);
					}
					else if (nodeName == "inputFile")
					{
						xmlChar *attr = xmlTextReaderGetAttribute(reader, (xmlChar *)"type");
						string type = (const char *)attr;

						setKeyValue("inputType",type);

						if (type == "random")
						{							
							if (xmlTextReaderRead(reader) == 1)
							{
								if (xmlTextReaderNodeType(reader) == XML_TEXT_NODE)
								{	
									nodeValue = (const char *)xmlTextReaderConstValue(reader);
#ifndef NDEBUG
									bool result =
#endif
										setKeyValue(type,nodeValue);		
									assert(result);
								}								
							}							
						}
						xmlFree(attr);
					}
					else if (nodeName == "outFile")
					{
						xmlChar *attr = xmlTextReaderGetAttribute(reader, (xmlChar *)"type");

						if (attr)
						{
							const string type = (const char *)attr;
#ifndef NDEBUG
							bool result =
#endif
								setKeyValue("outFileType",type);
							assert(result);
						}

						xmlFree(attr);					
					}
					else if (nodeName == "usingCache")
					{
						xmlChar *attr = xmlTextReaderGetAttribute(reader, (xmlChar *)"fixedLatency");
						string type = (const char *)attr;

						if (attr)
						{
							const string type = (const char *)attr;
#ifndef NDEBUG
							bool result =
#endif
								setKeyValue("fixedLatency",type);
							assert(result);
						}

						xmlFree(attr);
					}
				}
				break;
			case XML_TEXT_NODE:				
				if (xmlTextReaderConstName(reader))
				{
					nodeValue = (const char *)xmlTextReaderConstValue(reader);
#ifndef NDEBUG
					bool result =
#endif
						setKeyValue(nodeName, nodeValue);
					assert(result);

				}
			case XML_CDATA_SECTION_NODE:
				break;

			default:
				//cerr << "Unknown XML node type" << endl;
				break;
			}
		}

		if ((ret == -1) || (xmlTextReaderIsValid(reader) != 1))
		{
			cerr << "There was an error reading/parsing " << settingsFile << "." << endl;
			exit(-2);
		}

		// close the reader
		xmlFreeTextReader(reader);
		xmlMemoryDump();
	}

	// then update with the extra settings
	if ((extraSettings).length() > 0)
	{
		std::vector<string> params;
		boost::algorithm::trim(extraSettings);
		split(params, extraSettings, is_any_of(" "), token_compress_on);

		if (!params.empty())
		{
			if (params.size() % 2 != 0)
			{
				cerr << "Invalid memory override format, should be {<parameter name> <value>}+" << endl;
				cerr << params.size() << endl;
				exit(-14);
			}

			for (vector<string>::size_type i = 0; i < params.size(); i += 2)
			{
				if (!setKeyValue(params[i], params[i+1]) || !updateKeyValue(params[i],params[i+1], doc))
					cerr << "warn: Unrecognized key/value pair (" << params[i] << "," << params[i+1] << ")" << endl;
				else
					cerr << "note: setting " << params[i] << "=" << params[i+1] << endl;
			}
		}
	}

	// dump the resulting document
	int len;
	xmlChar *buffer;
	xmlDocDumpMemory(doc, &buffer,&len);
	settingsOutputFile.append((const char *)buffer);
	xmlFree(buffer);

	// free the document
	xmlFreeDoc(doc);

	xmlCleanupParser();

	return true;
}
