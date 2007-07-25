// VarioSens Lib.cpp : Defines the initialization routines for the DLL.
//
#include "stdafx.h"
#include "VarioSens Lib.h"
#include "C1lib.h"
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

#define MAXTRIES 1

#define DEFAULTARRAYSIZE 1

//////////////////////////////////////////////////////////////////////////
// setVarioSensSettings
//////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) int setVarioSensSettings(float lowTemp,
														  float hiTemp,
														  int interval,
														  int mode,
														  int batCheckInt)
{
	int errorCode = 0;

	if (!C1_open_comm()) 
	{
		C1_close_comm();
		errorCode = -1;
	}
	else if (!C1_enable())
	{
		C1_close_comm();
		errorCode = -2;
	}
	else
	{
		tag_15693 myVarioSensTag = new_15693();

		variosens_log varioSensLog = new_variosens_log();

		int failures = MAXTRIES;

		while (failures > 0)
		{
			// make sure it isn't doing a spin-wait
			Sleep(10);

			if (get_15693(&myVarioSensTag, NULL) && myVarioSensTag.tag_type == VARIOSENS)
			{
				if (!VS_init(&myVarioSensTag))
				{
					errorCode = -3;
					--failures;
				}
				else if (!VS_getLogState(&myVarioSensTag, &varioSensLog))
				{
					errorCode = -4;
					--failures;
				}				
				else
				{
					varioSensLog.stndByTime = 0;
					varioSensLog.logIntval = (unsigned short)interval;

					if (!VS_setLogTimer(&myVarioSensTag,&varioSensLog))
					{
						errorCode = -5;
						--failures;
					}
					else
					{
						varioSensLog.upperTemp = hiTemp;
						varioSensLog.lowerTemp = lowTemp;
						varioSensLog.logMode = (unsigned char)mode;

						if (!VS_setLogMode(&myVarioSensTag,&varioSensLog))
						{
							errorCode = -6;
							--failures;
						}
						else
						{
							CTime theTime(CTime::GetCurrentTime());
							time_t osBinaryTime = (time_t)theTime.GetTime(); // time_t defined in <time.h>
							SYSTEMTIME timeDest;
							theTime.GetAsSystemTime(timeDest);
							varioSensLog.UTCStartTime = osBinaryTime;

							if (!VS_startLog(&myVarioSensTag,osBinaryTime))
							{
								errorCode = -7;
								--failures;
							}
							else
							{								
								errorCode = 0;
								break;
							}
						}
					}
				}
			}
			else
			{
				errorCode = -3;
			}

		}
	}

	C1_disable();

	C1_close_comm();	

	return errorCode;
}

//////////////////////////////////////////////////////////////////////////
// getVarioSensTagID 
//////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) LPWSTR getVarioSensTagID()
{
	if (!C1_open_comm()) 
	{
		C1_close_comm();
		return NULL;
	}
	else if (!C1_enable())
	{
		C1_close_comm();
		return NULL;
	}
	else
	{
		tag_15693 myVarioSensTag = new_15693();

		int failures = MAXTRIES + 50;

		while (failures > 0)
		{
			// make sure it isn't doing a spin-wait
			Sleep(50);

			if (get_15693(&myVarioSensTag, NULL) &&
				myVarioSensTag.tag_type == VARIOSENS)
			{
				LPWSTR tagID = new WCHAR[17];

				for (int j = 0; j < 8; j++)
				{
					wsprintf(&(tagID[2*j]),L"%02X",myVarioSensTag.tag_id[j]);
				}
				return tagID;
			}
			failures--;
		}
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// getVarioSensSettings and associated callback
//////////////////////////////////////////////////////////////////////////
typedef void (*ARRAYCB2)(float upper,
						 float lower,
						 int period,
						 int logMode,
						 int batteryCheckInterval);

extern "C" __declspec(dllexport) int getVarioSensSettings(ARRAYCB2 callbackFunc)
{

	static float lowerTempLimit = 0;
	static float upperTempLimit = 0;
	static unsigned short recordPeriod = 0;
	static int errorCode = 0; // the equivalent of a return code
	static int logMode = 0;
	static int batteryCheckInterval = 0;

	if (!C1_open_comm()) 
	{
		C1_close_comm();
		errorCode = -1;
	}
	else if (!C1_enable())
	{
		C1_close_comm();
		errorCode = -2;
	}
	else
	{
		tag_15693 myVarioSensTag = new_15693();

		variosens_log varioSensLog = new_variosens_log();

		int failures = MAXTRIES;

		while (failures > 0)
		{
			// make sure it isn't doing a spin-wait
			Sleep(10);

			if (get_15693(&myVarioSensTag, NULL) &&
				myVarioSensTag.tag_type == VARIOSENS)
			{
#ifdef USINGPASSIVE
				if(!VS_set_passive(&my_tag_15693))
				{
					errorMessage = "ERROR - Failed to set passive";
					break;
				}
#endif

				if (!VS_getLogState(&myVarioSensTag, &varioSensLog))
				{
					errorCode = -4;
					--failures;
				}				
				else
				{
					// no more read failures at this point
					lowerTempLimit = varioSensLog.lowerTemp;
					upperTempLimit = varioSensLog.upperTemp;
					recordPeriod = varioSensLog.logIntval; 
					batteryCheckInterval = varioSensLog.batCheck;
					logMode = varioSensLog.logMode;

					errorCode = 0;
					
					(*callbackFunc)(upperTempLimit,lowerTempLimit,recordPeriod,logMode,batteryCheckInterval);					
					break;
				}
			}
			else
			{
				/*if (myVarioSensTag.tag_type != VARIOSENS)
				{
				errorCode = -7;
				break;
				}*/
				//else
				//{
				errorCode = -3;
				//}
				//--failures;
			}

		}
	}

	C1_disable();

	C1_close_comm();	

	return errorCode;
}

//////////////////////////////////////////////////////////////////////////
// getVarioSensLog and associated callback
//////////////////////////////////////////////////////////////////////////
typedef void (*ARRAYCB)(float upperTempLimit,
						float lowerTempLimit, 
						int len,
						unsigned short recordPeriod, 
						unsigned int dateTime[],
						unsigned char logMode[],						
						float temperatures[]);


extern "C" __declspec(dllexport) int getVarioSensLog(ARRAYCB callbackFunc)
{
	// default to having something to marshal
	int len = DEFAULTARRAYSIZE;
	unsigned char *logMode = new unsigned char[DEFAULTARRAYSIZE];
	unsigned int *dateTime = new unsigned int[DEFAULTARRAYSIZE];
	float *temperatures = new float[DEFAULTARRAYSIZE];	

	static float lowerTempLimit = 0;
	static float upperTempLimit = 0;
	static unsigned short recordPeriod = 0;
	int errorCode = -1; // the equivalent of a return code	 

	if (!C1_open_comm()) 
	{
		C1_close_comm();
		errorCode = -1;
	}
	else if (!C1_enable())
	{
		C1_close_comm();
		errorCode = -2;
	}
	else
	{
		tag_15693 myVarioSensTag = new_15693();

		variosens_log varioSensLog = new_variosens_log();

		int failures = MAXTRIES;

		while (failures > 0)
		{
			// make sure it isn't doing a spin-wait
			Sleep(10);

			if (get_15693(&myVarioSensTag, NULL) &&
				myVarioSensTag.tag_type == VARIOSENS)
			{
#ifdef USINGPASSIVE
				if(!VS_set_passive(&my_tag_15693))
				{
					errorMessage = "ERROR - Failed to set passive";
					break;
				}
#endif

				if (!VS_getLogState(&myVarioSensTag, &varioSensLog))
				{
					errorCode = -4;
					--failures;
				}
				else if(!VS_getLogData(&myVarioSensTag, &varioSensLog))
				{
					errorCode = -5;
					--failures;
				}
				else
				{
					// no more read failures at this point
					lowerTempLimit = varioSensLog.lowerTemp;
					upperTempLimit = varioSensLog.upperTemp;
					recordPeriod = varioSensLog.logIntval; 


					if (varioSensLog.numDownloadMeas == 0) 
					{
						errorCode = -6;	
						break;
					}
					else
					{
						errorCode = 0;

						delete[] logMode;
						delete[] dateTime;
						delete[] temperatures;

						len = varioSensLog.numDownloadMeas;

						logMode = new unsigned char[varioSensLog.numDownloadMeas];
						dateTime = new unsigned int[varioSensLog.numDownloadMeas];
						temperatures = new float[varioSensLog.numDownloadMeas];

						for (unsigned int i = 0; i < varioSensLog.numDownloadMeas; i++) 
						{
							dateTime[i] = varioSensLog.vltionData[i].vltionTime;

							logMode[i] = varioSensLog.vltionData[i].logMode;

							switch(varioSensLog.vltionData[i].logMode)
							{
							case 0:
								temperatures[i] = varioSensLog.vltionData[i].temperature[0];
								break;
							case 1:
								// FIXME: this logs to all three temperature slots
								temperatures[i] = varioSensLog.vltionData[i].temperature[0];
								break;
							case 2:
								temperatures[i] = varioSensLog.vltionData[i].temperature[0];
								break;
							case 3:						
								// FIXME: should log extremum, involves all three values
								/*tmp.Format(L"First Violation = %d/%d/%d........%d:%02d.........%.2f C\r\n",theTime.GetMonth(), theTime.GetDay(), theTime.GetYear(), theTime.GetHour(), theTime.GetMinute(), my_log.vltionData[0].temperature[0]);
								gStrStatus += tmp;
								tmp.Format(L"Extremum Temp = %d/%d/%d.........%d:%02d.........%.2f C\r\n",theTime.GetMonth(), theTime.GetDay(), theTime.GetYear(), theTime.GetHour(), theTime.GetMinute(), my_log.vltionData[1].temperature[0]);
								gStrStatus += tmp;
								tmp.Format(L"First Temp inside Limits = %d/%d/%d.........%d:%02d.........%.2f C\r\n",theTime.GetMonth(), theTime.GetDay(), theTime.GetYear(), theTime.GetHour(), theTime.GetMinute(), my_log.vltionData[2].temperature[0]);
								gStrStatus += tmp;*/
								break;
							}
						}
						(*callbackFunc)(upperTempLimit,lowerTempLimit,len,recordPeriod,dateTime,logMode,temperatures);
						break;
					}
				}
			}
			else
			{
				if (myVarioSensTag.tag_type != VARIOSENS)
				{
					errorCode = -7;
					break;
				}
				else
				{
					errorCode = -3;
				}
				//--failures;
			}

		}
	}

	C1_disable();

	C1_close_comm();	

	return errorCode;
}