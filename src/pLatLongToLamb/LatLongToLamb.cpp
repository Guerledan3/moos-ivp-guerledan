/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: LatLongToLamb.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include <cmath>
#include "MBUtils.h"
#include "ACTable.h"
#include "LatLongToLamb.h"
#include <proj_api.h>

using namespace std;

double m_lat;
double m_lon;
double m_time;
bool m_newlat = false;
bool m_newlon = false;

double ref_lat = 0.0;
double ref_lon = 0.0;
double std_par1 = 0.0;
double std_par2 = 0.0;
projPJ m_Platlon;
projPJ m_Plambert;

//---------------------------------------------------------
// Constructor

LatLongToLamb::LatLongToLamb()
{
  m_Platlon = pj_init_plus("+proj=longlat +datum=WGS84 +no_defs");
  m_Plambert = pj_init_plus("+proj=lcc +lat_1=49 +lat_2=44 +lat_0=46.5 +lon_0=3 +x_0=700000 +y_0=6600000 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs");
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool LatLongToLamb::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

     if(key == "NAV_LAT"){
      m_lat = msg.GetDouble();
      m_time = msg.GetTime();
      m_newlat = true;
     }
     else if (key == "NAV_LONG")
     {
      m_lon = msg.GetDouble();
      m_time = msg.GetTime();
      m_newlon = true;
     }
     else if(key != "APPCAST_REQ") // handle by AppCastingMOOSApp
       reportRunWarning("Unhandled Mail: " + key);

     if(m_newlon & m_newlon){
      double x = -4.34*DEG_TO_RAD;
      double y = 48.42*DEG_TO_RAD;

      pj_transform(m_Platlon, m_Plambert, 1, 1, &x, &y, NULL);
      printf("%f %f\n", x, y);

      Notify("LAMBERT_X", x);
      Notify("LAMBERT_Y", y);
     }
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool LatLongToLamb::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool LatLongToLamb::Iterate()
{
  AppCastingMOOSApp::Iterate();
  // Do your thing here!
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool LatLongToLamb::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = toupper(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if(param == "FOO") {
      handled = true;
    }
    else if(param == "BAR") {
      handled = true;
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void LatLongToLamb::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  // Register("FOOBAR", 0);
  Register("NAV_LAT", 0);
  Register("NAV_LONG", 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool LatLongToLamb::buildReport() 
{
  m_msgs << "============================================ \n";
  m_msgs << "File:                                        \n";
  m_msgs << "============================================ \n";

  ACTable actab(4);
  actab << "Alpha | Bravo | Charlie | Delta";
  actab.addHeaderLines();
  actab << "one" << "two" << "three" << "four";
  m_msgs << actab.getFormattedString();

  return(true);
}




