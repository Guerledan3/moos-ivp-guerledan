/************************************************************/
/*    NAME: Guerlédan 3                                              */
/*    ORGN: MIT                                             */
/*    FILE: Main.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "Main.h"
#include "stdio.h"
#include "stdlib.h"

#define nomFichierLignes "lignes"
#define TAILLE_LECTURE 50
using namespace std;

//---------------------------------------------------------
// Constructor

Main::Main()
{
   // Mode 1 = levé ; 2 = Quality Check
   mode = 0;
   motif_courant = 1;
   ligne_courante = 1;

   fin_leve = false;
   retour_qc = 0.0;

   current_time = 0.0;
   lancement = false;
}


//---------------------------------------------------------
// Procedure: modeLeve

void Main::modeLeve(){
        char sligne[50];
        sprintf(sligne,"%d", ligne_courante);
        string sligne2=sligne;
	
	
	string nomFichier = nomFichierLignes+sligne2+".txt";

	FILE* fichier = NULL;
	string cheminFichier="/home/guerledan3/moos-ivp-extend/missions/guerledan_mars_test/motifs/"+nomFichier;
	const char *chemin2 = cheminFichier.c_str();
        fichier=fopen(chemin2,"r");

	if(fichier!=NULL){
		char contenu[TAILLE_LECTURE]="";
		fgets(contenu,TAILLE_LECTURE,fichier);
		string ligne1=contenu;
		fgets(contenu,TAILLE_LECTURE,fichier);
		string ligne2=contenu;
	}     
}

//---------------------------------------------------------
// Procedure: modeQC

void Main::modeQC(){

}

//---------------------------------------------------------
// Procedure: OnNewMail

bool Main::OnNewMail(MOOSMSG_LIST &NewMail)
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

     if (key == "LANCEMENT"){
	if(MOOSStrCmp(msg.m_sVal, "false")) {
		lancement=false;
	}
	else if(MOOSStrCmp(msg.m_sVal, "true")) {
		if(!lancement){
			lancement=true;
			// utilisé lors de la première itération
      			mode=1;
      			modeLeve();
  			 
		}
	}
     }
     else if(key == "FIN_LEVE"){
       if(MOOSStrCmp(msg.m_sVal, "false")) {
	        if(fin_leve==true){

			mode = 1;
			fin_leve=false;

			if(lancement){
				modeLeve();
			}
		}
	}
	else if(MOOSStrCmp(msg.m_sVal, "true")) {
	  	if(fin_leve==false){

			mode = 2;
			fin_leve=true;

			if(lancement){
				modeQC();
			}
		}
	}
     }
     else if (key == "RETOUR_QC"){
	retour_qc = msg.m_dfVal;
     }
     else if (key == "DB_UPTIME"){
	current_time = msg.m_dfVal;
     }
     else if(key != "APPCAST_REQ") // handle by AppCastingMOOSApp
       reportRunWarning("Unhandled Mail: " + key);
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool Main::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool Main::Iterate()
{
  AppCastingMOOSApp::Iterate();

  

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool Main::OnStartUp()
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
    /*if(param == "speed") {
      handled = true;
    }
    else if(param == "BAR") {
      handled = true;
    }*/

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void Main::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();

  Register("FIN_LEVE", 0); 
  Register("RETOUR_QC", 0);
  Register("DB_UPTIME", 0);
  Register("LANCEMENT", 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool Main::buildReport() 
{

 
  char smode[50];
  sprintf(smode,"%d", mode);

  char smotif[50];
  sprintf(smotif,"%d", motif_courant);

  char sligne[50];
  sprintf(sligne,"%d", ligne_courante);

  char stime[50];
  snprintf(stime, 50, "%f", current_time);

  char slancement[50];
  sprintf(slancement,"%d", lancement);

  char sfinleve[50];
  sprintf(sfinleve,"%d", fin_leve);

  char sretourqc[50];
  snprintf(sretourqc, 50, "%f", retour_qc);

  m_msgs << "\n\nMode : ";
  m_msgs << smode;
  m_msgs << "\nMotif courant : ";
  m_msgs << smotif;
  m_msgs << "\nLigne courante : ";
  m_msgs << sligne;
  m_msgs << "\n\nCurrent Time : ";
  m_msgs << stime;
  m_msgs << "\n\nLancement (bool) : ";
  m_msgs << slancement;
  m_msgs << "\nFin Levé (bool) : ";
  m_msgs << sfinleve;
  m_msgs << "\nRetour QC : ";
  m_msgs << sretourqc;

  return(true);
}




