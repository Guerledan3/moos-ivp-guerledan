/************************************************************/
/*    NAME: Anthony Welte                                   */
/*    ORGN: ENSTA Bretagne                                  */
/*    FILE: CustomLogger.cpp                                */
/*    DATE:                                                 */
/************************************************************/

#include "MBUtils.h"
#include "ACTable.h"
#include "CustomLogger.h"

#include <sstream>
#include <algorithm>
#include <iterator>
#include <time.h>

using namespace std;

//---------------------------------------------------------
// Constructor

CustomLogger::CustomLogger()
{
  m_path = "/home/";
}

CustomLogger::~CustomLogger()
{
  for(auto it = m_logfiles.begin(); it != m_logfiles.end(); ++it){
    it->second.close();
  }
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool CustomLogger::OnNewMail(MOOSMSG_LIST &NewMail)
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

    m_current[key] = msg;
    if(msg.IsString()){
      m_lastlogged[key] = msg.GetString();
      if(m_varsize[key] < msg.GetString().size()+1)
        m_varsize[key] = msg.GetString().size()+1;
    }
    else
      m_lastlogged[key] = doubleToString(msg.GetDouble(), 6);
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool CustomLogger::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool CustomLogger::Iterate()
{
  AppCastingMOOSApp::Iterate();
  
  for(auto file = m_logfiles.begin(); file != m_logfiles.end(); ++file){
    std::map<double, std::stringstream> phrases;
    // Create the phrases for each time
    for(auto varname = m_watched[file->first].begin(); varname != m_watched[file->first].end(); ++varname){
      if(m_current.count(*varname) == 1 && phrases.count(floor((m_current[*varname].GetTime()-GetAppStartTime())*1000.0)/1000.0) == 0){
        phrases[floor((m_current[*varname].GetTime()-GetAppStartTime())*1000.0)/1000.0] << setw(9) << setprecision(3) << std::fixed << floor((m_current[*varname].GetTime()-GetAppStartTime())*1000.0)/1000.0 << " ";
      }
    }

    // Fill the phrases with the appropriate variable or an X
    for(auto varname = m_watched[file->first].begin(); varname != m_watched[file->first].end(); ++varname){
      unsigned width = m_varsize[*varname]; // Size of the column (TODO dynamic size depending of the content)

      if(m_current.count(*varname) == 1){
        if(m_current[*varname].IsDouble()){
          phrases[floor((m_current[*varname].GetTime()-GetAppStartTime())*1000.0)/1000.0] << setw(width) << setprecision(6) << std::fixed << m_current[*varname].GetDouble();
        }
        else{
          phrases[floor((m_current[*varname].GetTime()-GetAppStartTime())*1000.0)/1000.0] << setw(width) << m_current[*varname].GetString();
        }
        // Add X to all other phrases
        for(auto phr = phrases.begin(); phr != phrases.end(); ++phr){
          if(phr->first != floor((m_current[*varname].GetTime()-GetAppStartTime())*1000.0)/1000.0){
            phr->second << setw(width) << "X";
          }
        }
      }
      else{ // Add X if the current variable has not been updated
        for(auto phr = phrases.begin(); phr != phrases.end(); phr++){
          phr->second << setw(width) << "X";
        }
      }
    }

    // Save the phrases in the log file
    for(auto phr = phrases.begin(); phr != phrases.end(); phr++){
      file->second << phr->second.rdbuf() << endl;
    }
  }

  m_current.clear();

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool CustomLogger::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  sParams.reverse();
  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = toupper(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if(param == "PATH") {
      m_path = line;
      handled = true;
    }
    else if(param == "FILE") {
      string name = tokStringParse(line, "name", ',', '=');
      string watch = tokStringParse(line, "watch", ',', '=');

      if(m_logfiles.count(name) != 0){ // If the file name causes conflict
        reportConfigWarning("Prefix: " + name + " already used.");
      }
      else{ // The name is not already used
        m_logfiles[name] = ofstream(MakeLogName(m_path, name) + ".txt", ios::out | ios::trunc);
        m_logfiles[name] << setw(10) << "TIME ";
        m_watched[name] = parseString(watch, '/');

        for (unsigned i = 0; i < m_watched[name].size(); ++i){
          m_watched[name][i] = stripBlankEnds(m_watched[name][i]);
          m_register[m_watched[name][i]].push_back(name);
          m_logfiles[name] << setw(15) << m_watched[name][i];
          m_varsize[m_watched[name][i]] = 15;
        }
        m_logfiles[name] << endl;
      }

      handled = true;
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }
  
  registerVariables();	
  return(true);
}

string CustomLogger::MakeLogName(string path, string name)
{
  struct tm *Now;
  time_t aclock;
  time( &aclock );
  
  
  Now = localtime( &aclock );

  std::string  sTmp = MOOSFormat( "%s%d_%d_%d__%.2d_%.2d_%.2d_%s",
          path.c_str(),
          Now->tm_year+1900,
          Now->tm_mon+1,
          Now->tm_mday,
          Now->tm_hour,
          Now->tm_min,
          Now->tm_sec,
          name.c_str());

  return sTmp;
}

//---------------------------------------------------------
// Procedure: registerVariables

void CustomLogger::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();

  for (auto it = m_register.begin(); it != m_register.end(); ++it)
  {
    Register(it->first, 0);
  }
}


//------------------------------------------------------------
// Procedure: buildReport()

bool CustomLogger::buildReport() 
{
  for(auto file = m_watched.begin(); file != m_watched.end(); ++file){
    m_msgs << "============================================" << endl;
    m_msgs << "File: " << file->first << endl;
    m_msgs << "============================================" << endl;
    ACTable actab(file->second.size());

    // Create the header
    string head = "";
    for(auto varname = file->second.begin(); varname != file->second.end()-1; ++varname){
      head += *varname + " | ";
    }
    head += *(file->second.end()-1);
  
    actab << head;
    actab.addHeaderLines();
    for(auto varname = file->second.begin(); varname != file->second.end(); varname++){
      actab << m_lastlogged[*varname];
    }
    m_msgs << actab.getFormattedString();

    m_msgs << endl;
  }

  return(true);
}




