#ifndef POWERCONFIG_H
#define POWERCONFIG_H
#pragma once

#include <map>
#include <string>

#include "globals.h"
#include "dramChannel.h"
#include "enumTypes.h"
#include "dramSettings.h"

class powerConfig
{
protected:
	float VDD;
	int IDD0; // Operating Current: One Bank Active-Precharge in mA
	int IDD2P; // Precharge Power-Down Current (CKE=0), in mA
	int IDD2N; // Precharge Standby Current (CKE=1), in mA
	int IDD3P; // Active Power-Down Current (CKE=0), in mA
	int IDD3N; // Active Standby Current (CKE=1), in mA
	int IDD4R; // Operating Burst Read Current in mA
	int IDD4W; // Operating Burst Write Current in mA
	int IDD5; // Burst Refresh Current in mA

	tick_t allBanksPrecharged;
	tick_t RDsch; // number of clock cycles spent sending data
	tick_t WRsch; // number of clock cycles spent receiving data
	

public:
	powerConfig();
	powerConfig(dramSettings *settings);
	void recordCommand(const command *, const dramChannel &channel, const dramTimingSpecification &timing);
	void calculatePower(const std::vector<dramChannel> &channels);
	
public:
	~powerConfig();
};
#endif
