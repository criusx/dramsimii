#ifndef POWERSCRIPTS_HH
#define POWERSCRIPTS_HH

#include "processStats.hh"
#include "PowerParameters.hh"
#include "globals.hh"

class PowerScripts
{
protected:
	std::list<string> filesGenerated;

	unsigned writing;
	vector<vector<float> > values;
	vector<float> valueBuffer;
	
	vector<pair<float, float> > energyValues;
	pair<float, float> energyValueBuffer;

	unsigned scaleFactor;
	unsigned scaleIndex;
	unsigned channelCount;
	unsigned epochCount;

	bool foundEpoch, foundCommandLine;

	double epochTime;

	list<pair<string, string> > powerParams;

	vector<string> commandLine;

	string rawCommandLine;

	bf::path givenfilename;

	pair<double, double> totalEnergy;

	PowerParameters powerParameters;	

	WeightedAverage<double> averageInUseTime;

	bool usingCache;


public:
	PowerScripts(list<pair<string, string> > &_powerParams):
	  writing(0),
		  scaleFactor(1),
		  scaleIndex(0),
		  channelCount(0),
		  epochCount(0),
		  foundEpoch(false),
		  foundCommandLine(false),
		  epochTime(0.0),
		usingCache(false)
	  {  powerParams = _powerParams; }

	  void generateGraphs(const bf::path &outputDir);

	  void generateJointGraphs(const bf::path &outputDir, PowerScripts &alternatePower);

	  string getRawCommandLine() const { return rawCommandLine; }

	  bool working() const
	  {
		  return foundCommandLine && foundEpoch;
	  }

	  bool processStatsForFile(const string &file);

	  pair<double, double> getTotalEnergy() const { return totalEnergy; }

	  double getAverageInUseTime() const { return averageInUseTime.average();}

	  const vector<vector<float> > &getValues() const { return values; }

	  const vector<pair<float,float> > &getEnergyValues() const { return energyValues; }

	  double getRunTime() const { return epochTime * (double)epochCount; }

	  void evenRunTime(const double);

	  bool isUsingCache() const { return usingCache; }

	  const PowerParameters &getPowerParameters() const { return powerParameters; }

protected:
	void processLine(char *newLine);

	void pushStats();

	void compressStats();

	void powerGraph(const bf::path &outFilename, opstream &p, bool isThumbnail) const;

	void bigPowerGraph(const bf::path &outFilename, opstream &p, const vector<vector<float> > &alternateValues, bool isThumbnail) const;

	void bigPowerGraph2(const bf::path &outFilename, opstream &p, const vector<vector<float> > &alternateValues, bool isThumbnail) const;

	void comparativePowerGraph(const bf::path &outFilename, opstream &p, const vector<vector<float> > &alternateValues, bool isThumbnail) const;

	void energyGraph(const bf::path &outFilename, opstream &p, bool isThumbnail) const;

	void bigEnergyGraph(const bf::path &outFilename, opstream &p, bool isThumbnail) const;

	void cumulativeEnergyGraph(const bf::path &outFilename, opstream &p, const vector<pair<float, float> > &alternateValues, bool isThumbnail) const;

	// the scripts to generate the graphs
	static const string totalPowerScript;
	static const string averagePowerScript;
	static const string energyScript;
	static const string energy2Script;
	static const string cumulPowerScript;
	static const string bigPowerScript;
	static const string comparativePowerScript;
	static const string bigEnergyScript;
};

#endif