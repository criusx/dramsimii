#ifndef POWERCONFIG_H
#define POWERCONFIG_H
#pragma once

#include <map>
#include <string>

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


public:
	powerConfig();
	powerConfig(std::map<file_io_token_t,std::string> &parameter);
public:
	~powerConfig();
};
#endif
