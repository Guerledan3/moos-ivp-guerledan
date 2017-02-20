/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: InsSBG.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef InsSBG_HEADER
#define InsSBG_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include <sbgEComLib/sbgEComLib.h>
#include <string>

using namespace std;

class InsSBG : public AppCastingMOOSApp
{
 public:
   InsSBG();
   ~InsSBG();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();

 protected:
   void registerVariables();

 private: // Configuration variables
	string UART_PORT;
  int UART_BAUD_RATE;

  SbgEComHandle			  comHandle;
	SbgInterface			  sbgInterface;
	SbgEComDeviceInfo		deviceInfo;
  SbgErrorCode        errorCode;

 private: // State variables
 	float roll, pitch, yaw;
  float latitude, longitude, altitude;

 	bool new_ekf, new_nav, new_time;
  float year, month, day, hour, minute, second, nanoSecond, gpsTimeOfWeek;

 private:
  int newPose(string pose);
 	static SbgErrorCode onLogReceived(SbgEComHandle *pHandle, SbgEComCmdId logCmd, const SbgBinaryLogData *pLogData, void *pUserArg);
  bool Notify_attitude(float *roll, float *pitch, float *yaw);
  bool Notify_pose(float *lat, float *lon, float *altitude);
};

#endif 
