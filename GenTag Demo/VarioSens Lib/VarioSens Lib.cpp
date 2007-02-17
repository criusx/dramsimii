// VarioSens Lib.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "VarioSens Lib.h"
#include "c1lib.h"
#include <string>
#include <sstream>

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

BOOL APIENTRY DllMain( HANDLE hModule,
					  DWORD ul_reason_for_call,
					  LPVOID lpReserved)
{
	return TRUE;
}

extern "C" __declspec(dllexport) int WINAPI multiply(int a,int b)
{
	return a * b;
}
#if 0
extern "C" __declspec(dllexport) int WINAPI getViolations()
{
	
}
#endif

typedef void (*ARRAYCB)(int errorCode,
						float upperTempLimit, float lowerTempLimit, int recordPeriod, 
						int len,
						unsigned char logMode[],
						unsigned int dateTime[],
						float temperatures[]);

#define MAXTRIES 16

extern "C" __declspec(dllexport) void getVarioSensLog(ARRAYCB callbackFunc)
{
	// default to having something to marshal
	int len = 1;
	unsigned char *logMode = new unsigned char[1];
	unsigned int *dateTime = new unsigned int[1];	
	float *temperatures = new float[1];	

	static float lowerTempLimit = 0;
	static float upperTempLimit = 0;
	static unsigned short recordPeriod = 0;
	static int errorCode = 0; // the equivalent of a return code	 

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
			//Sleep(10);

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

						unsigned char *logMode = new unsigned char[varioSensLog.numDownloadMeas];
						unsigned int *dateTime = new unsigned int[varioSensLog.numDownloadMeas];
						float *temperatures = new float[varioSensLog.numDownloadMeas];

						for (unsigned int i=0; i<varioSensLog.numDownloadMeas; i++) 
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
								temperatures[i] = varioSensLog.vltionData[i].temperature[1];
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
					}
					break;
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
				--failures;
			}
			
		}
	}

	C1_disable();

	C1_close_comm();	

	(*callbackFunc)(errorCode, upperTempLimit,lowerTempLimit,recordPeriod,len,logMode,dateTime,temperatures);
}