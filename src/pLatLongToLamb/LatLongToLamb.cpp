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

     if(key == "INS_NAV"){
      string str = msg.GetString();
      string time;
      double lat, lon, ele;
      MOOSValFromString(time, str, "TIME");
      MOOSValFromString(lat, str, "LAT");
      MOOSValFromString(lon, str, "LON");
      MOOSValFromString(ele, str, "ALT");
      lat *= DEG_TO_RAD;
      lon *= DEG_TO_RAD;
      pj_transform(m_Platlon, m_Plambert, 1, 1, &lon, &lat, NULL);
      string msg_str;
      msg_str += "TIME=" + time + ",";
      msg_str += "X=" + doubleToString(lon, 6) + ",";
      msg_str += "Y=" + doubleToString(lat, 6) + ",";
      msg_str += "Z=" + doubleToString(ele, 6);
      Notify("INS_NAV_LAMB", msg_str);
     }
     else if (key == "GPS_NAV")
     {
      string str = msg.GetString();
      string time;
      double lat, lon, ele;
      MOOSValFromString(time, str, "TIME");
      MOOSValFromString(lat, str, "LAT");
      MOOSValFromString(lon, str, "LON");
      MOOSValFromString(ele, str, "ALT");
      lat *= DEG_TO_RAD;
      lon *= DEG_TO_RAD;
      pj_transform(m_Platlon, m_Plambert, 1, 1, &lon, &lat, NULL);
      string msg_str;
      msg_str += "TIME=" + time + ",";
      msg_str += "X=" + doubleToString(lon, 6) + ",";
      msg_str += "Y=" + doubleToString(lat, 6) + ",";
      msg_str += "Z=" + doubleToString(ele, 6);
      Notify("GPS_NAV_LAMB", msg_str);
<<<<<<< HEAD
      Notify("NAV_X", lon-253559.1325078258);
      Notify("NAV_Y", lat-6805674.85861476);
=======
>>>>>>> b2535f1d9f6634a86238c984a12ed1349f448020
     }
     else if(key != "APPCAST_REQ") // handle by AppCastingMOOSApp
       reportRunWarning("Unhandled Mail: " + key);
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
  Register("INS_NAV", 0);
  Register("GPS_NAV", 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool LatLongToLamb::buildReport() 
{
  return(true);
}




