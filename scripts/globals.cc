#include "processStats.hh"
#include "globals.hh"

#include <boost/interprocess/sync/file_lock.hpp>

using boost::ireplace_first;

bf::path executableDirectory;

bool generatePngFiles = false;

bool userStop = false;

bool cypressResults = false;

unordered_map<string, string> decoder = setupDecoder();

bool doneEntering = false;

list<string> fileList;

mutex fileListMutex;

bool separateOutputDir = false;

using boost::algorithm::replace_first;

//////////////////////////////////////////////////////////////////////////
/// @brief setup the output directory
//////////////////////////////////////////////////////////////////////////
void prepareOutputDir(const bf::path &outputDir, const string &filename,
					  const vector<string> &commandLine, list<pair<string,
					  string> > &graphs)
{	
	bf::path printFile = outputDir  / ("index.html");

	if (!fileExists(printFile.native_directory_string()))
	{
		bf::path templateFile = executableDirectory / "template.html";

		if (!fileExists(templateFile.native_directory_string()))
		{
			cerr << "cannot find template:"
				<< templateFile.native_directory_string();
			return;
		}

#pragma omp critical
		bf::copy_file(templateFile, printFile);
	}

	boost::interprocess::file_lock flock(printFile.native_directory_string().c_str());

	ifstream instream(printFile.native_directory_string().c_str(), ios::in | ios::ate);

	ifstream::pos_type entireFileLength = instream.tellg();
	char *entireFile = new char[entireFileLength];

	instream.seekg(0, ios::beg);
	instream.read(entireFile, entireFileLength);
	instream.close();

	string outputContent(entireFile);
	delete[] entireFile;

	bool changesMade = false;

	// update the title if necessary
	const string titleString = "@@@";
	if (outputContent.find(titleString) != string::npos)
	{
		string newTitle;
		for (vector<string>::const_iterator i = commandLine.begin(), end = commandLine.end();
			i < end; ++i)
		{
			newTitle.append(*i);
		}
		//cerr << "@@@" << newTitle << endl;
		replace_first(outputContent, titleString, newTitle);
	}
	
	// append all the images that are to go into this summary
	for (list<pair<string, string> >::const_iterator i = graphs.begin(), end =
		graphs.end(); i != end; ++i)
	{
		string currentImageLink = "<h2>" + i->second
			+ "</h2><a rel=\"lightbox\" href=\"" + i->first + "."
			+ processedExtension + "\"><img class=\"fancyzoom\" src=\"" + i->first
			+ "-thumb." + thumbnailExtension + +"\" alt=\"\" /></a>";

		if (outputContent.find(currentImageLink) == string::npos)
		{
			changesMade = true;
			outputContent = outputContent.insert(outputContent.find("</div>"),
				currentImageLink);
		}
	}
#pragma omp critical
	if (changesMade)
	{
		cerr << "Updating results file: " << printFile.native_directory_string() << endl;
		std::ofstream out(printFile.native_directory_string().c_str());
		out.write(outputContent.c_str(), outputContent.length());
		out.close();
	}

	bf::path htaccessOut = outputDir / ".htaccess";
	if (!fileExists(htaccessOut.string()))
	{
		bf::path htaccessFile = executableDirectory / ".htaccess";
		bf::copy_file(htaccessFile, htaccessOut);
	}
}

void sigproc(int i)
{
	userStop = true;
}

