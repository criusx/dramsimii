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