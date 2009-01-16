// Copyright (C) 2008 University of Maryland.
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
#include <libxml/xmlreader.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <iostream>
#include <fstream>
#include <string>
#define USEXML
#include "globals.h"
#include "Settings.h"

using namespace std;
using namespace DRAMsimII;

Settings::Settings(const int argc, const char **argv):
systemType(BASELINE_CONFIG)
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

	//#define USEREADERFORMEMORY


	ifstream xmlFile(settingsFile.c_str(),ios::in|ios::ate);

	xmlTextReader *reader = NULL;

	char *entireXmlFile = NULL;
	ifstream::pos_type entireXmlFileLength = 0;

	if (xmlFile.is_open())
	{
		entireXmlFileLength = xmlFile.tellg();
		entireXmlFile = new char[entireXmlFileLength];

		xmlFile.seekg(0,ios::beg);
		xmlFile.read(entireXmlFile,entireXmlFileLength);

		xmlFile.close();

		entireXmlFile[entireXmlFileLength] = 0;

		//xmlDocPtr doc = xmlReadFile(settingsFile.c_str(),NULL, XML_PARSE_DTDVALID);
#ifdef USEREADERFORMEMORY
		reader = xmlReaderForMemory(
			entireXmlFile,entireXmlFileLength,
			NULL,NULL,
			XML_PARSE_RECOVER | XML_PARSE_DTDATTR | XML_PARSE_NOENT | XML_PARSE_DTDVALID);	
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
						bool result = setKeyValue(nodeName,attrS);
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
									bool result = setKeyValue(type,nodeValue);		
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
							bool result = setKeyValue("outFileType",type);
							assert(result);
						}

						xmlFree(attr);

						attr = xmlTextReaderGetAttribute(reader, (xmlChar *)"dbreporting");

						if (attr)
						{
							const string type = (const char *)attr;
							bool result = setKeyValue("dbreporting",type);
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

					bool result = setKeyValue(nodeName, nodeValue);
					assert(result);

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
			if (entireXmlFileLength > 0)
				settingsOutputFile.append(entireXmlFile, entireXmlFileLength);
			else
			{
				cerr << "Could not create output for settings file" << endl;
				exit(-2);
			}
		}
		// close the reader
		xmlFreeTextReader(reader);
		xmlMemoryDump();
	}

	boost::mt19937 generator(std::time(0));
	// Define a uniform random number distribution which produces "double"
	// values between 0 and 1 (0 inclusive, 1 exclusive).
	boost::uniform_int<> uni_dist(0,INT_MAX);
	boost::variate_generator<boost::mt19937&, boost::uniform_int<> > uni(generator, uni_dist);	
	sessionID = toString(uni());

}
