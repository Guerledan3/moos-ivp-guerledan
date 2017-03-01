/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: LatLongToLamb.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef LatLongToLamb_HEADER
#define LatLongToLamb_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class LatLongToLamb : public AppCastingMOOSApp
{
 public:
   LatLongToLamb();
   ~LatLongToLamb() {};

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
};

#endif 
