#include "processStats.hh"
#include "globals.hh"

bool fileExists(const string& fileName)
{
	bf::path newPath(fileName.c_str());
	return bf::exists(newPath);
}

bool ensureDirectoryExists(const bf::path &outputDir)
{
	if (!bf::exists(outputDir))
	{
		if (!bf::create_directories(outputDir))
		{
			cerr << "Could not create dir " << outputDir.leaf();
			return false;
		}
	}
	else
	{
		if (!bf::is_directory(outputDir))
		{
			cerr << "Directory " << outputDir.leaf()
				<< " exists, but is not a directory." << endl;
			return false;
		}
	}

	return true;
}

template<class T>
T toNumeric(const std::string& s, std::ios_base& (*f)(std::ios_base&))
{
	std::istringstream iss(s);
	T t;
	iss >> f >> t;
	return t;
}


#define LINE_LENGTH 178


vector<string> getCommandLine(const string &inputLine)
{
	string commandLine(inputLine);
	trim(commandLine);
	vector<string> individualWords;
	split(individualWords,inputLine, is_any_of(" "));

	string currentLine;
	vector<string> splitLine;

	for (vector<string>::const_iterator i = individualWords.begin(), end = individualWords.end();
		i < end; ++i)
	{
		if (currentLine.length() + i->length() > LINE_LENGTH)
		{
			splitLine.push_back(currentLine);
			currentLine = *i;
		}
		else
		{
			currentLine.append(" ");
			currentLine.append(*i);
		}
	}

	if (currentLine.length() > 0)
		splitLine.push_back(currentLine);

	return splitLine;
}

void printTitle(const char *title, const vector<string> &commandLine, std::ostream &p, const unsigned numPlots)
{
	if (numPlots > 0)
		p << "set multiplot layout " << numPlots << ", 1 ";
	else
		p << "set ";

	p << "title \"{/*1.5 " << title << "}\\n";

	for (vector<string>::const_iterator i = commandLine.begin(), end = commandLine.end();
		i < end; ++i)
	{
		p << "{/*0.75 " << *i << "}\\n";
	}
	p << "\"";
	
	if (numPlots == 0)
		p << " offset character 0, -3, 0 norotate";
	
	p << endl;
}