// VarioSens Lib.cpp : Defines the initialization routines for the DLL.
//
#include "stdafx.h"
#include "C1lib.h"
#include <stdio.h>

#define DEFAULTARRAYSIZE 1


#define TIMEOUT 7

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
		return -1;
	}
	else if (!C1_enable())
	{
		C1_close_comm();
		return -1;
	}
	else
	{
		tag_15693 myVarioSensTag = new_15693();

		variosens_log varioSensLog = new_variosens_log();

		__time64_t startTime = CTime::GetCurrentTime().GetTime();

		while (CTime::GetCurrentTime().GetTime() - startTime < TIMEOUT)
		{
			// make sure it isn't doing a spin-wait
			Sleep(10);

			if (get_15693(&myVarioSensTag, NULL) && myVarioSensTag.tag_type == VARIOSENS)
			{
				if (!VS_init(&myVarioSensTag))
				{
					errorCode = -3;					
				}
				else if (!VS_getLogState(&myVarioSensTag, &varioSensLog))
				{
					errorCode = -4;
				}				
				else
				{
					varioSensLog.stndByTime = 0;
					varioSensLog.logIntval = (unsigned short)interval;

					if (!VS_setLogTimer(&myVarioSensTag,&varioSensLog))
					{
						errorCode = -5;
					}
					else
					{
						varioSensLog.upperTemp = hiTemp;
						varioSensLog.lowerTemp = lowTemp;
						varioSensLog.logMode = (unsigned char)mode;

						if (!VS_setLogMode(&myVarioSensTag,&varioSensLog))
						{
							errorCode = -6;
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
	LPWSTR tagID = NULL;

	if (!C1_open_comm() || !C1_enable()) 
	{
		return NULL;
	}
	else
	{
		tag_15693 myVarioSensTag = new_15693();

		__time64_t startTime = CTime::GetCurrentTime().GetTime();

		while (CTime::GetCurrentTime().GetTime() - startTime < TIMEOUT)
		{
			// make sure it isn't doing a spin-wait
			Sleep(50);

			if (get_15693(&myVarioSensTag, NULL) &&
				myVarioSensTag.tag_type == VARIOSENS)
			{
				tagID = new WCHAR[17];

				for (int j = 0; j < 8; j++)
				{
					wsprintf(&(tagID[2*j]),L"%02X",myVarioSensTag.tag_id[j]);
				}

				break;
			}
		}
	}
	C1_disable();

	C1_close_comm();	

	return tagID;
}

//////////////////////////////////////////////////////////////////////////
// getOneTagID 
//////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) LPWSTR getOneTagID()
{
	LPWSTR tagID = NULL;

	if (!C1_open_comm() || !C1_enable()) 
	{
		return NULL;
	}
	else
	{
		tag_15693 myTag = new_15693();

		__time64_t startTime = CTime::GetCurrentTime().GetTime();

		while (CTime::GetCurrentTime().GetTime() - startTime < TIMEOUT)
		{
			// make sure it isn't doing a spin-wait
			Sleep(50);

			if (get_15693(&myTag, NULL))
			{
				tagID = new WCHAR[17];

				for (int j = 0; j < 8; j++)
				{
					wsprintf(&(tagID[2*j]),L"%02X",myTag.tag_id[j]);
				}

				break;
			}
		}
	}
	C1_disable();

	C1_close_comm();	

	return tagID;
}

//////////////////////////////////////////////////////////////////////////
// getVarioSensSettings and associated callback
//////////////////////////////////////////////////////////////////////////
typedef void (*ARRAYCB2)(float upper,
						 float lower,
						 int period,
						 int logMode,
						 int batteryCheckInterval);

////
extern "C" __declspec(dllexport) int getVarioSensSettings(ARRAYCB2 callbackFunc)
{

	static float lowerTempLimit = 0;
	static float upperTempLimit = 0;
	static unsigned short recordPeriod = 0;
	static int errorCode = 0; // the equivalent of a return code
	static int logMode = 0;
	static int batteryCheckInterval = 0;

	if (!C1_open_comm() || !C1_enable()) 
	{
		errorCode = -1;
	}
	else
	{
		tag_15693 myVarioSensTag = new_15693();

		variosens_log varioSensLog = new_variosens_log();

		__time64_t startTime = CTime::GetCurrentTime().GetTime();

		while (CTime::GetCurrentTime().GetTime() - startTime < TIMEOUT)
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
						float temperatures[],
						LPWSTR tagID);

extern "C" __declspec(dllexport) int getVarioSensLog(ARRAYCB callbackFunc, bool reset, int periodicity)
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
		return -1;
	}
	else if (!C1_enable()) 
	{
		C1_close_comm();
		return -2;
	}
	else
	{
		tag_15693 myVarioSensTag = new_15693();

		variosens_log varioSensLog = new_variosens_log();		

		for  (__time64_t startTime = CTime::GetCurrentTime().GetTime();
			CTime::GetCurrentTime().GetTime() - startTime < TIMEOUT;)
		{
			// make sure it isn't doing a spin-wait
			Sleep(10);

			if (get_15693(&myVarioSensTag, NULL)) 
			{
				if (myVarioSensTag.tag_type == VARIOSENS)
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
					}
					else if(!VS_getLogData(&myVarioSensTag, &varioSensLog))
					{
						errorCode = -5;
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
							temperatures = new float[varioSensLog.numDownloadMeas * 3];

							for (unsigned int i = 0; i < varioSensLog.numDownloadMeas; i++) 
							{
								dateTime[i] = varioSensLog.vltionData[i].vltionTime;

								logMode[i] = varioSensLog.vltionData[i].logMode;

								switch(varioSensLog.vltionData[i].logMode)
								{
									// Logging internal and external measurement values
								case 0:
									// All values out of the limits
								case 2:
									temperatures[i] = varioSensLog.vltionData[i].temperature[0];
									break;
									// Logging all values (3 measurement values per block) 
								case 1:
									// FIXME: this logs to all three temperature slots
									temperatures[3 * i + 0] = varioSensLog.vltionData[i].temperature[0];
									temperatures[3 * i + 1] = varioSensLog.vltionData[i].temperature[1];
									temperatures[3 * i + 2] = varioSensLog.vltionData[i].temperature[2];
									break;

									// Logging extremum out of the limits 
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
						}
						// then get the tag ID
						LPWSTR tagID = new WCHAR[17];

						for (int j = 0; j < 8; j++)
						{
							wsprintf(&(tagID[2*j]),L"%02x",myVarioSensTag.tag_id[j]);
						}

						(*callbackFunc)(upperTempLimit,lowerTempLimit,varioSensLog.numDownloadMeas,recordPeriod,dateTime,logMode,temperatures,tagID);
						break;
					}
				}
				else
				{
					errorCode = -7;
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

	if (reset && ((errorCode == 0) || (errorCode == -6)))
	{
		errorCode = setVarioSensSettings(29.0f, 29.0f,60,1, 3600);
	}

	return errorCode;
}


///////////////////////////////////////////////////////////////////////////////
//// resetVS
//// returns
//// 0 - success
//// -1 - could not open or enable reader
//// -2 - could not init the VS card
//// -3 - could not find a VS tag
///////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) int resetVS()
{
	int errorCode = 0;

	if (!C1_open_comm())
	{
		C1_close_comm();
		return -1;
	}
	else if (!C1_enable())
	{
		C1_close_comm();
		return -1;
	}
	else
	{
		tag_15693 myVarioSensTag = new_15693();
		
		__time64_t startTime = CTime::GetCurrentTime().GetTime();

		while (CTime::GetCurrentTime().GetTime() - startTime < TIMEOUT)
		{
			// make sure it isn't doing a spin-wait
			Sleep(10);

			if (get_15693(&myVarioSensTag, NULL) &&
				myVarioSensTag.tag_type == VARIOSENS)
			{
				// disable logging
				if (VS_init(&myVarioSensTag))
				{
					errorCode = 0;
					break;
				}
				else
				{
					errorCode = -2;
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