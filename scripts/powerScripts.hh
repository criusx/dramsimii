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

	vector<unsigned> rasCounts;

	unsigned rasCountBuffer;

	vector<unsigned> readCounts;

	unsigned readCountBuffer;

	vector<vector<double> > values;

	vector<double> valueBuffer;
	
	vector<pair<double, double> > energyValues;

	pair<double, double> energyValueBuffer;

	unsigned scaleFactor;
	
	unsigned scaleIndex;
	
	unsigned channelCount;
	
	unsigned epochCount;

	bool foundEpoch, foundCommandLine;

	double epochTime;

	double baseEpochTime;

	list<pair<string, string> > powerParams;

	vector<string> commandLine;

	string rawCommandLine;

	bf::path givenfilename;

	pair<double, double> totalEnergy;

	PowerParameters powerParameters;	

	WeightedAverage<double> averageInUseTime;

	bool usingCache;

	double runTime;

	SimpleAverage<uint64_t> averageDramPower;

	SimpleAverage<uint64_t> averageCachePower;


public:
	PowerScripts(list<pair<string, string> > &_powerParams):
	  writing(0),
		  rasCountBuffer(0),
		  readCountBuffer(0),
		  scaleFactor(1),
		  scaleIndex(0),
		  channelCount(0),
		  epochCount(0),
		  foundEpoch(false),
		  foundCommandLine(false),
		  epochTime(0.0),
		  baseEpochTime(0.0),
		  usingCache(false),
		  runTime(0.0),
		  averageDramPower(),
		  averageCachePower()
	  {  powerParams = _powerParams; }

	  double getAverageActStbyPower() const
	  {
		  double count = (double)values.size();
		  double total = 0.0;
		  for (vector<vector<double> >::const_iterator i = values.begin(), end = values.end();
			  i < end; i++)
		  {
			  total += (*i)[0];
		  }

		  return total / count;
	  }

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

	  double getEpochTime() const { return epochTime; }

	  const vector<vector<double> > &getValues() const { return values; }

	  const vector<pair<double,double> > &getEnergyValues() const { return energyValues; }

	  double getRunTime() const { return runTime; }

	  void evenRunTime(const double);

	  bool isUsingCache() const { return usingCache; }

	  const PowerParameters &getPowerParameters() const { return powerParameters; }

	  const vector<unsigned> &getRasCounts() const { return rasCounts; }

	  const vector<unsigned> &getReadCounts() const { return readCounts; }

	  double getDramPower() const { return averageDramPower.average(); }

	  double getCachePower() const { return averageCachePower.average(); }

protected:
	void processLine(char *newLine);

	void pushStats();

	void compressStats();

	void powerGraph(const bf::path &outFilename, opstream &p, bool isThumbnail) const;

	void energyGraph(const bf::path &outFilename, opstream &p, bool isThumbnail) const;

	void bigEnergyGraph(const bf::path &outFilename, opstream &p, bool isThumbnail) const;

	void bigPowerGraph(const bf::path &outFilename, opstream &p, const vector<vector<double> > &alternateValues, const double alternateRuntime, bool isThumbnail) const;

	void bigPowerGraph2(const bf::path &outFilename, opstream &p, const vector<vector<double> > &alternateValues, const double alternateRuntime, bool isThumbnail) const;

	void comparativePowerGraph(const bf::path &outFilename, opstream &p, const vector<vector<double> > &alternateValues, const double alternateEpochTime, bool isThumbnail) const;

	void cumulativeEnergyGraph(const bf::path &outFilename, opstream &p, const vector<pair<double, double> > &alternateValues, const double alternateEpochTime, bool isThumbnail) const;

	void rasCountReadCountGraph(const bf::path &outFilename, opstream &p, const PowerScripts &theoreticalPower, bool isThumbnail) const;

	// the scripts to generate the graphs
	static const string totalPowerScript;
	static const string averagePowerScript;
	static const string energyScript;
	static const string energy2Script;
	static const string cumulPowerScript;
	static const string bigPowerScript;
	static const string comparativePowerScript;
	static const string bigEnergyScript;
	static const string rasCountReadCountGraphScript;
};

#endif