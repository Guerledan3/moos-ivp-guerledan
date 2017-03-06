/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: LogLine.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <time.h>
#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include "MBUtils.h"
#include "ACTable.h"
#include "LogLine.h"

using namespace std;

//---------------------------------------------------------
// Constructor

LogLine::LogLine()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool LogLine::OnNewMail(MOOSMSG_LIST &NewMail)
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
     if(key == "LOGLINE"){
      string str = msg.GetString();
      if(str == "stop"){
        recording = false;
        // Save everything in a file
        saveDataInterp();
        saveData();
        // Empty saved data
        clearData();
        
      }
      else if (str == "restart")
      {
        recording = true;
        num_line = 1;
        time_restart = msg.GetTime();
      }
      else if(str == "pause"){
        recording = false;
        time_pause = msg.GetTime();
      }
      else{
        recording = true;
        num_line = 0;
        time_start = msg.GetTime();
        MOOSValFromString(current_line, str, "init");
      }
     }
     else if(recording && key == "GPS_NAV_LAMB"){
      string str = msg.GetString();
      double time;
      Position pos;
      MOOSValFromString(time, str, "TIME");
      MOOSValFromString(pos.X, str, "X");
      MOOSValFromString(pos.Y, str, "Y");
      MOOSValFromString(pos.Z, str, "Z");
      gps_nav[time] = pos;
     }
     else if(recording && key == "INS_NAV_LAMB"){
      string str = msg.GetString();
      double time;
      Position pos;
      MOOSValFromString(time, str, "TIME");
      MOOSValFromString(pos.X, str, "X");
      MOOSValFromString(pos.Y, str, "Y");
      MOOSValFromString(pos.Z, str, "Z");
      ins_nav[time] = pos;
     }
     else if(recording && key == "INS_EULER"){
      string str = msg.GetString();
      double time;
      Euler eul;
      MOOSValFromString(time, str, "TIME");
      MOOSValFromString(eul.roll, str, "ROLL");
      MOOSValFromString(eul.pitch, str, "PICH");
      MOOSValFromString(eul.yaw, str, "YAW");
      ins_euler[time] = eul;
     }
     else if(recording && key == "INS_EEULER"){
      string str = msg.GetString();
      double time;
      Euler eeul;
      MOOSValFromString(time, str, "TIME");
      MOOSValFromString(eeul.roll, str, "EROLL");
      MOOSValFromString(eeul.pitch, str, "EPICH");
      MOOSValFromString(eeul.yaw, str, "EYAW");
      ins_eeuler[time] = eeul;
     }
     else if(recording && key == "SOUNDER_RANGE"){
      string str = msg.GetString();
      double time;
      double sounder;
      MOOSValFromString(time, str, "TIME");
      MOOSValFromString(sounder, str, "RANGE");
      sounder_range[time] = sounder;
     }
     else if(recording && key == "GPS_SIG"){
      gps_rtk[MOOS::Time()] = (int)msg.GetDouble();
     }
     else if(key != "APPCAST_REQ") // handle by AppCastingMOOSApp
       reportRunWarning("Unhandled Mail: " + key);
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool LogLine::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool LogLine::Iterate()
{
  AppCastingMOOSApp::Iterate();
  // Do your thing here!
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool LogLine::OnStartUp()
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

void LogLine::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  // Register("FOOBAR", 0);
  Register("GPS_NAV_LAMB", 0);
  Register("INS_NAV_LAMB", 0);
  Register("INS_EULER", 0);
  Register("INS_EEULER", 0);
  Register("SOUNDER_RANGE", 0);
  Register("GPS_SIG", 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool LogLine::buildReport() 
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

void LogLine::saveData(){
  // Time
  time_t rawtime;
  struct tm * timeinfo;

  time(&rawtime);
  timeinfo = localtime(&rawtime);

  ofstream save_file;
  ofstream archive_file;

  stringstream filename_save;
  filename_save << "/home/Lines/F" << current_line << ".txt";
  save_file.open(filename_save.str().c_str(), ios::trunc);

  stringstream filename_archive;
  filename_archive << "_Time_" << asctime(timeinfo) << ".txt";
  archive_file.open(filename_archive.str().c_str(), ios::trunc);

  for(std::map<double, double>::iterator it = sounder_range.begin(); it != sounder_range.end(); ++it){
    double t = it->first;
    double s_range = it->second;

    int line_n = 0;
    if(t>time_restart)
      line_n = 1;

    // Find closest Euler Angles
    Euler eul;
    double min_dt = 1000.0;
    for(std::map<double, Euler>::iterator it_euler = ins_euler.begin(); it_euler != ins_euler.end(); ++it_euler){
      if(abs(t-it_euler->first) < min_dt){
        min_dt = abs(t-it_euler->first);
        eul = it_euler->second;
      }
    }

    // Find closest Euler Standart Derivation
    Euler eeul;
    min_dt = 1000.0;
    for(std::map<double, Euler>::iterator it_eeuler = ins_eeuler.begin(); it_eeuler != ins_eeuler.end(); ++it_eeuler){
      if(abs(t-it_eeuler->first) < min_dt){
        min_dt = abs(t-it_eeuler->first);
        eeul = it_eeuler->second;
      }
    }

    // Find closest Position
    Position p;
    min_dt = 1000.0;
    for(std::map<double, Position>::iterator it_pos = ins_nav.begin(); it_pos != ins_nav.end(); ++it_pos){
      if(abs(t-it_pos->first) < min_dt){
        min_dt = abs(t-it_pos->first);
        p = it_pos->second;
      }
    }

    // Find closest RTK
    int rtk;
    min_dt = 1000.0;
    for(std::map<double, int>::iterator it_rtk = gps_rtk.begin(); it_rtk != gps_rtk.end(); ++it_rtk){
      if(abs(t-it_rtk->first) < min_dt){
        min_dt = abs(t-it_rtk->first);
        rtk = it_rtk->second;
      }
    }

    save_file << p.X << " " << p.Y << " " << p.Z << " ";
    save_file << eul.roll << " " << eul.pitch << " " << eul.yaw << " ";
    save_file << rtk << " ";
    save_file << eeul.roll << " " << eeul.pitch << " " << eeul.yaw << " ";
    save_file << s_range << " " << line_n << "\n";


    archive_file << p.X << " " << p.Y << " " << p.Z << " ";
    archive_file << eul.roll << " " << eul.pitch << " " << eul.yaw << " ";
    archive_file << rtk << " ";
    archive_file << eeul.roll << " " << eeul.pitch << " " << eeul.yaw << " ";
    archive_file << s_range << " " << line_n << "\n";
  }

  save_file.close(); 
  archive_file.close();

}

void LogLine::saveDataInterp(){
  // Time
  time_t rawtime;
  struct tm * timeinfo;

  time(&rawtime);
  timeinfo = localtime(&rawtime);

  ofstream save_file;
  ofstream archive_file;

  stringstream filename_save;
  filename_save << "/home/Lines/F" << current_line << ".txt";
  save_file.open(filename_save.str().c_str(), ios::trunc);

  stringstream filename_archive;
  filename_archive << "_Time_" << asctime(timeinfo) << ".txt";
  archive_file.open(filename_archive.str().c_str(), ios::trunc);

  for(std::map<double, double>::iterator it = sounder_range.begin(); it != sounder_range.end(); ++it){
    double t = it->first;
    double s_range = it->second;

    int line_n = 0;
    if(t>time_restart)
      line_n = 1;

    // Find closest Euler Angles
    Euler eul;
    double min_dt = 1000.0;
    for(std::map<double, Euler>::iterator it_euler = ins_euler.begin(); it_euler != ins_euler.end(); ++it_euler){
      if(abs(t-it_euler->first) < min_dt){
        min_dt = abs(t-it_euler->first);
        eul = it_euler->second;
      }
    }

    // Find closest Euler Standart Derivation
    Euler eeul;
    min_dt = 1000.0;
    for(std::map<double, Euler>::iterator it_eeuler = ins_eeuler.begin(); it_eeuler != ins_eeuler.end(); ++it_eeuler){
      if(abs(t-it_eeuler->first) < min_dt){
        min_dt = abs(t-it_eeuler->first);
        eeul = it_eeuler->second;
      }
    }

    // Find closest Position
    Position p;
    min_dt = 1000.0;
    for(std::map<double, Position>::iterator it_pos = ins_nav.begin(); it_pos != ins_nav.end(); ++it_pos){
      if(abs(t-it_pos->first) < min_dt){
        min_dt = abs(t-it_pos->first);
        p = it_pos->second;
      }
    }

    // Find closest RTK
    int rtk;
    min_dt = 1000.0;
    for(std::map<double, int>::iterator it_rtk = gps_rtk.begin(); it_rtk != gps_rtk.end(); ++it_rtk){
      if(abs(t-it_rtk->first) < min_dt){
        min_dt = abs(t-it_rtk->first);
        rtk = it_rtk->second;
      }
    }

    save_file << p.X << " " << p.Y << " " << p.Z << " ";
    save_file << eul.roll << " " << eul.pitch << " " << eul.yaw << " ";
    save_file << rtk << " ";
    save_file << eeul.roll << " " << eeul.pitch << " " << eeul.yaw << " ";
    save_file << s_range << " " << line_n << "\n";


    archive_file << p.X << " " << p.Y << " " << p.Z << " ";
    archive_file << eul.roll << " " << eul.pitch << " " << eul.yaw << " ";
    archive_file << rtk << " ";
    archive_file << eeul.roll << " " << eeul.pitch << " " << eeul.yaw << " ";
    archive_file << s_range << " " << line_n << "\n";
  }

  save_file.close(); 
  archive_file.close();

}

void LogLine::clearData(){
  gps_nav.clear();
  gps_rtk.clear();
  ins_nav.clear();
  ins_euler.clear();
  ins_eeuler.clear();
  sounder_range.clear();
}