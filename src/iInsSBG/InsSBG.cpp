/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: InsSBG.cpp                                        */
/*    DATE:                                                 */
/************************************************************/



#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "InsSBG.h"

using namespace std;

//---------------------------------------------------------
// Constructor

InsSBG::InsSBG()
{
  yaw = 0.0;
  pitch = 0.0;
  roll = 0.0;

  e_yaw = 0.0;
  e_pitch = 0.0;
  e_roll = 0.0;  

  longitude = 0.0;
  latitude = 0.0;
  altitude = 0.0;

  new_ekf = 0;
  new_nav = 0;
  new_std = 0;
}

InsSBG::~InsSBG()
{
  sbgEComClose(&comHandle);
  sbgInterfaceSerialDestroy(&sbgInterface);
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool InsSBG::OnNewMail(MOOSMSG_LIST &NewMail)
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

     if(key == "INS_CMD"){
        string str = msg.GetString();
        string cmd = biteString(str, ':');
        if(toupper(cmd)=="NEW_POSE"){
          // INS_CMD= "NEW_POSE:LATITUDE=40.0,LONGITUDE=1.0"
          newPose(str);
        }
     }
     else if(key != "APPCAST_REQ") // handle by AppCastingMOOSApp
       reportRunWarning("Unhandled Mail: " + key);
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool InsSBG::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool InsSBG::Iterate()
{
  AppCastingMOOSApp::Iterate();

  int errorCode = sbgEComHandle(&comHandle);
  // if (errorCode != SBG_NOT_READY){ // ToDo : doesn't work
    reportConfigWarning("Test");
    if(new_ekf){
      new_ekf = false;
      // Notify("NAV_INS_ROLL", roll);
      // Notify("NAV_INS_PITCH", pitch);
      Notify("NAV_HEADING", yaw*180.0/3.1415);
      Notify_attitude( &roll, &pitch, &yaw);
    }
    if(new_nav){
      new_nav = false;
      // Notify("NAV_INS_LAT", latitude);  // to be remplaced by NAV_INS_*
      // Notify("NAV_INS_LONG", longitude);
      // Notify("NAV_INS_DEPTH", -altitude);
      Notify_pose(&latitude, &longitude, &altitude);
    }
    if(new_std){
      new_std = false;
      Notify_attitude_std(&e_roll, &e_pitch, &e_yaw);
    }
    if(new_time){
      new_time = false;
      Notify("INS_year", year);
      Notify("INS_month", month);
      Notify("INS_day", day);
      Notify("INS_hour", hour);
      Notify("INS_minute", minute);
      Notify("INS_second", second);
      Notify("INS_nanoSecond", nanoSecond);
      Notify("INS_gpsTimeOfWeek", gpsTimeOfWeek);
    }
  // }
  // else{
      // reportRunWarning("SBG NOT READY");
  // }

  //AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool InsSBG::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  // SBG param variables
  SbgEComMagCalibResults  magCalibResults;

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
    if(param == "UART_PORT"){
      UART_PORT = value;
      handled = true;
    }
    else if(param == "UART_BAUD_RATE"){
      UART_BAUD_RATE = atoi(value.c_str());
      handled = true;
    }
    else if(param == "MAG_CALIB_OFFSET_0"){
      magCalibResults.offset[0] = atof(value.c_str());
      handled = true;
    }
    else if(param == "MAG_CALIB_OFFSET_1"){
      magCalibResults.offset[1] = atof(value.c_str());
      handled = true;
    }
    else if (param == "MAG_CALIB_OFFSET_2"){
      magCalibResults.offset[2] = atof(value.c_str());
      handled = true;     
    }
    else if (param == "MAG_CALIB_MATRIX_00"){
      magCalibResults.matrix[0] = atof(value.c_str());
      handled = true;
    }
    else if (param == "MAG_CALIB_MATRIX_01"){
      magCalibResults.matrix[1] = atof(value.c_str());
      handled = true;
    }
    else if (param == "MAG_CALIB_MATRIX_02"){
      magCalibResults.matrix[2] = atof(value.c_str());
      handled = true;
    }
    else if (param == "MAG_CALIB_MATRIX_11"){
      magCalibResults.matrix[3] = atof(value.c_str());
      handled = true;
    }
    else if (param == "MAG_CALIB_MATRIX_12"){
      magCalibResults.matrix[4] = atof(value.c_str());
      handled = true;
    }
    else if (param == "MAG_CALIB_MATRIX_22"){
      magCalibResults.matrix[5] = atof(value.c_str());
      handled = true;
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);
  }

  // ********************* Initialize the SBG  *********************
  errorCode = sbgInterfaceSerialCreate(&sbgInterface, UART_PORT.c_str(), UART_BAUD_RATE);
  if (errorCode != SBG_NO_ERROR){reportConfigWarning("sbgInterfaceSerialCreate Error");}

  errorCode = sbgEComInit(&comHandle, &sbgInterface); // Init the SBG
  if (errorCode != SBG_NO_ERROR){reportConfigWarning("sbgEComInit Error");}

  errorCode = sbgEComCmdGetInfo(&comHandle, &deviceInfo); // Get device info
  if (errorCode != SBG_NO_ERROR){reportConfigWarning("sbgEComCmdGetInfo Error");}

  // ****************************** SBG Config ******************************
  // sbgEComCmdOutputSetConf(&comHandle, SBG_ECOM_OUTPUT_PORT_?, SBG_ECOM_CLASS_LOG_?, ?,  ? );
  // errorCode = sbgEComCmdOutputSetConf(&comHandle, SBG_ECOM_OUTPUT_PORT_A, SBG_ECOM_CLASS_LOG_ECOM_0, SBG_ECOM_LOG_IMU_DATA, SBG_ECOM_OUTPUT_MODE_DIV_8);
  // if (errorCode != SBG_NO_ERROR){reportConfigWarning("sbgEComCmdOutputSetConf EKF Error");}

  errorCode = sbgEComCmdOutputSetConf(&comHandle, SBG_ECOM_OUTPUT_PORT_A, SBG_ECOM_CLASS_LOG_ECOM_0, SBG_ECOM_LOG_EKF_EULER, SBG_ECOM_OUTPUT_MODE_DIV_8);
  if (errorCode != SBG_NO_ERROR){reportConfigWarning("sbgEComCmdOutputSetConf EKF Error");}

  errorCode = sbgEComCmdOutputSetConf(&comHandle, SBG_ECOM_OUTPUT_PORT_A, SBG_ECOM_CLASS_LOG_ECOM_0, SBG_ECOM_LOG_EKF_NAV, SBG_ECOM_OUTPUT_MODE_DIV_8);
  if (errorCode != SBG_NO_ERROR){reportConfigWarning("sbgEComCmdOutputSetConf EKF Error");}

  // GNSS : input trame selection (UBX or NMEA)
   SbgEComModelInfo pModelInfo;
  // pModelInfo.id = 104; //104 for UBX, 102 for NMEA
  // pModelInfo.revision = 1.0;
   
   // sbgEComCmdGenericGetModelInfo(&comHandle, SBG_ECOM_CMD_GNSS_1_MODEL_INFO, &pModelInfo);
   // if (errorCode != SBG_NO_ERROR){reportConfigWarning("sbgEComCmdGenericGetModelInfo Error");}
   // char id[100], revision[100];
   // sprintf(&id[0], "%i", pModelInfo.id);
   // sprintf(&revision[0], "%i", pModelInfo.revision);
   // reportEvent("ComModelInfo : id=" + string(id));
   // reportEvent("ComModelInfo : revision =" + string(revision));

  // GNSS : Initial position
  // SbgEComInitConditionConf sbgEComInitConditionConf;
  // sbgEComInitConditionConf.latitude  = 47.788295;
  // sbgEComInitConditionConf.longitude = -3.334517;
  // sbgEComInitConditionConf.altitude  = 0.0;
  // sbgEComInitConditionConf.year      = 2016;
  // sbgEComInitConditionConf.month     = 10;
  // sbgEComInitConditionConf.day       = 19;
  // errorCode = sbgEComCmdSensorSetInitCondition(&comHandle, &sbgEComInitConditionConf);
  // if (errorCode != SBG_NO_ERROR){reportConfigWarning("sbgEComCmdSensorSetInitCondition Error");}

  // GNSS : Port config
  //SbgEComAidingAssignConf sbgEComAidingAssignConf;
  //sbgEComAidingAssignConf.gps1Port         = SBG_ECOM_MODULE_PORT_C;
  //sbgEComAidingAssignConf.gps1Sync         = SBG_ECOM_MODULE_SYNC_DISABLED;
  //sbgEComAidingAssignConf.rtcmPort         = SBG_ECOM_MODULE_DISABLED;
  //sbgEComAidingAssignConf.odometerPinsConf = SBG_ECOM_MODULE_ODO_DISABLED;
  //errorCode = sbgEComCmdSensorSetAidingAssignment(&comHandle, &sbgEComAidingAssignConf);
  //if (errorCode != SBG_NO_ERROR){reportConfigWarning("sbgEComAidingAssignConf Error");}

  // GNSS : Rejection mode config
  //SbgEComGnssRejectionConf sbgEComGnssRejectionConf;
  //sbgEComGnssRejectionConf.position = SBG_ECOM_AUTOMATIC_MODE; // SBG_ECOM_AUTOMATIC_MODE, SBG_ECOM_ALWAYS_ACCEPT_MODE, SBG_ECOM_NEVER_ACCEPT_MODE
  //sbgEComGnssRejectionConf.velocity = SBG_ECOM_AUTOMATIC_MODE;
  //sbgEComGnssRejectionConf.course   = SBG_ECOM_AUTOMATIC_MODE;
  //sbgEComGnssRejectionConf.hdt      = SBG_ECOM_AUTOMATIC_MODE;
  //errorCode = sbgEComCmdGnss1SetRejection(&comHandle, &sbgEComGnssRejectionConf);
  //if (errorCode != SBG_NO_ERROR){reportConfigWarning("sbgEComAidingAssignConf Error");}

  // MAGNETIC config
  // SbgEComMagRejectionConf sbgEComMagRejectionConf;
  // sbgEComMagRejectionConf.magneticField = SBG_ECOM_AUTOMATIC_MODE; //SBG_ECOM_ALWAYS_ACCEPT_MODE;
  // errorCode = sbgEComCmdMagSetRejection(&comHandle, &sbgEComMagRejectionConf);
  // if (errorCode != SBG_NO_ERROR){reportConfigWarning("sbgEComCmdMagSetRejection Error");}

  // magCalibResults.matrix[3] = magCalibResults.matrix[1];
  // magCalibResults.matrix[6] = magCalibResults.matrix[2];
  // magCalibResults.matrix[7] = magCalibResults.matrix[5];
  // errorCode = sbgEComCmdMagSetCalibData(&comHandle, magCalibResults.offset, magCalibResults.matrix);

  // SAVE AND REBOOT
  errorCode = sbgEComCmdSettingsAction(&comHandle, SBG_ECOM_SAVE_SETTINGS);
  if (errorCode != SBG_NO_ERROR){reportConfigWarning("sbgEComCmdSettingsAction Error");}

  // ************************** SBG Callback for data ************************
  errorCode = sbgEComSetReceiveCallback(&comHandle, onLogReceived, this);
  if (errorCode != SBG_NO_ERROR){reportConfigWarning("sbgEComSetReceiveCallback Error");}

  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void InsSBG::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  // Register("FOOBAR", 0);
  Register("INS_CMD", 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool InsSBG::buildReport()
{
  ACTable actab(6);
  actab << "Lat | Long | Ele | Roll | Pitch | Yaw";
  actab.addHeaderLines();

  actab << latitude << longitude << altitude << roll << pitch << yaw;
  m_msgs << actab.getFormattedString();

  return(true);
}

// Data Callback
SbgErrorCode InsSBG::onLogReceived(SbgEComHandle *pHandle, SbgEComCmdId logCmd, const SbgBinaryLogData *pLogData, void *pUserArg)
{
  InsSBG* pInsSBG = (InsSBG*)pUserArg;
  float time_of_week;
  switch (logCmd){
    case SBG_ECOM_LOG_EKF_EULER:
      pInsSBG->new_ekf = true;
      pInsSBG->roll    = pLogData->ekfEulerData.euler[0];
      pInsSBG->pitch   = pLogData->ekfEulerData.euler[1];
      pInsSBG->yaw     = pLogData->ekfEulerData.euler[2];
      break;
    case SBG_ECOM_LOG_EKF_NAV:
      pInsSBG->new_nav   = true;
      pInsSBG->latitude  = pLogData->ekfNavData.position[0];
      pInsSBG->longitude = pLogData->ekfNavData.position[1];
      pInsSBG->altitude  = pLogData->ekfNavData.position[2];
      break;
    case SBG_ECOM_LOG_EKF_QUAT:
      pInsSBG->new_std = true;
      pInsSBG->e_roll    = pLogData->ekfQuatData.eulerStdDev[0];
      pInsSBG->e_pitch   = pLogData->ekfQuatData.eulerStdDev[1];
      pInsSBG->e_yaw     = pLogData->ekfQuatData.eulerStdDev[2];
      break;
    case SBG_ECOM_LOG_UTC_TIME:
      pInsSBG->new_time = true;
      pInsSBG->year          = pLogData->utcData.year;
      pInsSBG->month         = pLogData->utcData.month;
      pInsSBG->day           = pLogData->utcData.day;
      pInsSBG->hour          = pLogData->utcData.hour;
      pInsSBG->minute        = pLogData->utcData.minute;
      pInsSBG->second        = pLogData->utcData.second;
      pInsSBG->nanoSecond    = pLogData->utcData.nanoSecond;
      pInsSBG->gpsTimeOfWeek = pLogData->utcData.gpsTimeOfWeek;
      break;
    default:
      break;
  }
  return SBG_NO_ERROR;
}

// Set New pose to the INS (for first fix for instance)
int InsSBG::newPose(string pose){

  if(strContains(pose, "LATITUDE=") & strContains(pose, "LONGITUDE=")){
    string lat_str = tokStringParse(pose, "LATITUDE", ',', '=');
    string lon_str = tokStringParse(pose, "LONGITUDE", ',', '=');

    SbgEComInitConditionConf sbgEComInitConditionConf;
    sbgEComInitConditionConf.latitude = std::atof(lat_str.c_str());
    sbgEComInitConditionConf.longitude = std::atof(lon_str.c_str());
    
    // sbgEComInitConditionConf.altitude = 0.0;
    // sbgEComInitConditionConf.year = 2015;
    // sbgEComInitConditionConf.month = 5;
    // sbgEComInitConditionConf.day = 26;

    errorCode = sbgEComCmdSensorSetInitCondition(&comHandle, &sbgEComInitConditionConf);
    if (errorCode != SBG_NO_ERROR){reportConfigWarning("sbgEComCmdSensorSetInitCondition Error");}

    // SAVE AND REBOOT
    errorCode = sbgEComCmdSettingsAction(&comHandle, SBG_ECOM_SAVE_SETTINGS);
    if (errorCode != SBG_NO_ERROR){reportConfigWarning("sbgEComCmdSettingsAction Error");}

  }
  else{
    reportRunWarning("Bad msg received for new Pose");
    return false;
  }

  return true;
}


bool InsSBG::Notify_attitude(float *roll, float *pitch, float *yaw){
  string msg;
  msg += "TIME=" + doubleToString(MOOS::Time()) + ",";
  msg += "ROLL=" + doubleToString(*roll, 6) + ",";
  msg += "PITCH=" + doubleToString(*pitch, 6) + ",";
  msg += "YAW=" + doubleToString(*yaw, 6);
  Notify("INS_EULER", msg);
  return true;
}
bool InsSBG::Notify_attitude_std(float *e_roll, float *e_pitch, float *e_yaw){
  string msg;
  msg += "TIME=" + doubleToString(MOOS::Time()) + ",";
  msg += "EROLL=" + doubleToString(*e_roll, 6) + ",";
  msg += "EPITCH=" + doubleToString(*e_pitch, 6) + ",";
  msg += "EYAW=" + doubleToString(*e_yaw, 6);
  Notify("INS_EEULER", msg);
  return true;
}
bool InsSBG::Notify_pose(float *lat, float *lon, float *altitude){
  string msg;
  msg += "TIME=" + doubleToString(MOOS::Time()) + ",";
  msg += "LAT=" + doubleToString(*lat, 6) + ",";
  msg += "LON=" + doubleToString(*lon, 6) + ",";
  msg += "ALT=" + doubleToString(*altitude, 6);
  Notify("INS_NAV", msg);
  return true;
}

// reportEvent("Good msg received: " + message);
// reportRunWarning("Bad msg received: " + message);
// reportConfigWarning("Problem configuring FOOBAR. Expected a number but got: " + str);*
