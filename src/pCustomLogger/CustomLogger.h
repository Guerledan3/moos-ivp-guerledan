/************************************************************/
/*    NAME: Anthony Welte                                   */
/*    ORGN: ENSTA Bretagne                                  */
/*    FILE: CustomLogger.h                                  */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef CustomLogger_HEADER
#define CustomLogger_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

#include <string>
#include <vector>
#include <fstream>

class CustomLogger : public AppCastingMOOSApp
{
 public:
   CustomLogger();
   ~CustomLogger();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();

 protected:
   void registerVariables();
   std::string MakeLogName(std::string path, std::string name);

 private: // Configuration variables
   std::string m_path;
   std::map<std::string, std::vector<std::string>> m_watched;  // For each files     which variables are watched
   std::map<std::string, std::vector<std::string>> m_register; // For each variables which files     are registered

   std::map<std::string, CMOOSMsg> m_current;
   std::map<std::string, std::ofstream> m_logfiles;

   std::map<std::string, std::string> m_lastlogged; // Last value (converted to string) that was logged (used for debugging)
   std::map<std::string, unsigned> m_varsize;

 private: // State variables
};

#endif 
