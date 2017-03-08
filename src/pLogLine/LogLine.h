/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: LogLine.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef LogLine_HEADER
#define LogLine_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

struct Position{
	double X;
	double Y;
	double Z;
};

struct Euler{
	double roll;
	double pitch;
	double yaw;
};

class LogLine : public AppCastingMOOSApp
{
 public:
   LogLine();
   ~LogLine() {};

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

 private: // State variables
 	std::map<double, Position> gps_nav;
 	std::map<double, int> gps_rtk;
 	std::map<double, Position> ins_nav;
 	std::map<double, Euler> ins_euler;
 	std::map<double, Euler> ins_eeuler;
 	std::map<double, double> sounder_range;
 	double time_start;
 	double time_pause;
 	double time_restart;
 	bool recording;
 	int current_line;
 	int num_line;

<<<<<<< HEAD
	std::string last_msg;

 	std::string save_prefix;
 	std::string archive_prefix;

 	std::string saveData();
 	std::string saveDataInterp();
=======
 	void saveData();
 	void saveDataInterp();
>>>>>>> b2535f1d9f6634a86238c984a12ed1349f448020
	void clearData();
};

#endif 
