#include "processStats.hh"
#include "globals.hh"

bf::path executableDirectory;

bool generatePngFiles = false;

bool userStop = false;

bool cypressResults = false;

unordered_map<string, string> decoder = setupDecoder();

bool doneEntering = false;

list<string> fileList;

mutex fileListMutex;


//////////////////////////////////////////////////////////////////////////
/// @brief setup the output directory
//////////////////////////////////////////////////////////////////////////
void prepareOutputDir(const bf::path &outputDir, const string &filename,
					  const string &commandLine, list<pair<string,
					  string> > &graphs)
{
	bf::path templateFile = executableDirectory / "template.html";

	if (!fileExists(templateFile.native_directory_string()))
	{
		cerr << "cannot find template:"
			<< templateFile.native_directory_string();
		return;
	}
	string basename;
	string::size_type position = filename.find("-stats");
	if (position != string::npos)
	{
		basename = filename.substr(0, position);
	}
	else if ((position = filename.find("-power")) != string::npos)
	{
		basename = filename.substr(0, position);
	}
	else
		return;

	bf::path printFile = outputDir / ("index.html");

	ifstream instream;

	bool alreadyExists;

	if (alreadyExists = fileExists(printFile.native_directory_string()))
	{
		instream.open(printFile.native_directory_string().c_str(), ios::in
			| ios::ate);
	}
	else
	{
		instream.open(templateFile.directory_string().c_str(), ios::in
			| ios::ate);
	}

	ifstream::pos_type entireFileLength = instream.tellg();
	char *entireFile = new char[entireFileLength];

	instream.seekg(0, ios::beg);
	instream.read(entireFile, entireFileLength);
	instream.close();

	string outputContent(entireFile);
	delete[] entireFile;

	bool changesMade = false;

	if (!alreadyExists)
	{
		string find("@@@");
		// update the title 
		outputContent = outputContent.replace(outputContent.find(find),
			find.length(), commandLine);
		changesMade = true;
	}

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

	if (changesMade)
	{
		cerr << endl << printFile.native_directory_string() << endl;
		std::ofstream out(printFile.native_directory_string().c_str());
		out << outputContent;
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

