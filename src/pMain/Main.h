/************************************************************/
/*    NAME: Guerlédan 3                                              */
/*    ORGN: MIT                                             */
/*    FILE: Main.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef Main_HEADER
#define Main_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
using namespace std;

class Main : public AppCastingMOOSApp
{
 public:
   Main();
   ~Main() {};

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();

 protected:
   void registerVariables();
   void modeLeve();
   void modeQC();

 private: // Configuration variables

 private: // State variables
   int mode;
   int motif_courant;
   int paire_ligne_courante;
   double ligne_courante;

   string ligne1;
   string ligne2;
   string spaire;

   double waypoint;
   string logline;

   bool fin_leve;
   bool lancement;
   double retour_qc;

   double current_time;
};

#endif 
