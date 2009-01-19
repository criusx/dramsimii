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
#include <boost/algorithm/string.hpp>
#include <boost/program_options/option.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
#include <libxml/xmlreader.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "globals.h"
#include "Settings.h"

using namespace std;
using namespace DRAMsimII;
using boost::is_any_of;
using boost::token_compress_on;
using boost::split;



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


Settings::Settings(int argc, char **argv):
systemType(BASELINE_CONFIG)
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

		doc = xmlParseFile(settingsFile.c_str());
		if (doc == NULL)
		{
			cout << "Unable to open settings file \"" << settingsFile << "\"" << endl;
			exit(-2);
		}
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
		}/*
		else
		{
			if (entireXmlFileLength > 0)
				settingsOutputFile.append(entireXmlFile, entireXmlFileLength);
			else
			{
				cerr << "Could not create output for settings file" << endl;
				exit(-2);
			}
		}*/
		// close the reader
		xmlFreeTextReader(reader);
		xmlMemoryDump();
	}

	// then update with the extra settings
	if ((extraSettings).length() > 0)
	{
		std::vector<string> params;
		split(params, extraSettings, is_any_of(" "), token_compress_on);

		if (params.size() > 0)
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

	/* dump the resulting document */
	//xmlDocDump(stdout, doc);
	int len;
	xmlChar *buffer;
	xmlDocDumpMemory(doc, &buffer,&len);
	settingsOutputFile.append((const char *)buffer);
	xmlFree(buffer);


	/* free the document */
	xmlFreeDoc(doc); 


	boost::mt19937 generator(std::time(0));
	// Define a uniform random number distribution which produces "double"
	// values between 0 and 1 (0 inclusive, 1 exclusive).
	boost::uniform_int<> uni_dist(0,INT_MAX);
	boost::variate_generator<boost::mt19937&, boost::uniform_int<> > uni(generator, uni_dist);	
	sessionID = toString(uni());
}
