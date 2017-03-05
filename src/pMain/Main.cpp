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
#define nomFichierLog "log"
#define TAILLE_LECTURE 50

using namespace std;

//---------------------------------------------------------
// Constructor

Main::Main()
{
   // Mode 0 = mission non lancée ; 1 = levé ; 2 = Quality Check
   mode = 0;
   motif_courant = 1;
   paire_ligne_courante = 1;
   ligne_courante = 0.0;  // dans la paire de lignes : 0 = aucune ligne faite ;
			// 1 = ligne 1 faite ; 2 = lignes 1 et 2 faites

   ligne1="0,0";
   ligne2="0,0";
   spaire=""; // string du numéro de la paire de ligne actuelle

   waypoint=0.0;
   logline="";

   fin_leve = false;
   retour_qc = 0.0; // provisoire : 0 = mode QC non-activé
		    // 1 : à refaire
		    // 2 : valide et cohérent

   current_time = 0.0;
   lancement = false;
}


//---------------------------------------------------------
// Procedure: modeLeve

void Main::modeLeve(){
        char sligne[50];
        sprintf(sligne,"%d", paire_ligne_courante);
        spaire=sligne;
	
	
	string nomFichier = nomFichierLignes+spaire+".txt";

	FILE* fichier = NULL;
	string cheminFichier="/home/guerledan3/moos-ivp-extend/missions/guerledan_mars_test/motifs/"+nomFichier;
	const char *chemin2 = cheminFichier.c_str();
        fichier=fopen(chemin2,"r");

	// valeurs au cas où le fichier ne soit pas présent
        ligne1="0,0"; // ou position actuelle
	ligne2="0,0";

	if(fichier!=NULL){
		char contenu[TAILLE_LECTURE]="";
		fgets(contenu,TAILLE_LECTURE,fichier);
		ligne1=contenu;
		fgets(contenu,TAILLE_LECTURE,fichier);
		ligne2=contenu;
	}
	
	Notify("WAYPT_UPDATE","name="+spaire+"_1 # points="+ligne1+" # endflag = LIGNE_COURANTE = 1 # endflag = LOGLINE=pause");

}

//---------------------------------------------------------
// Procedure: modeQC

void Main::modeQC(){
	// nom fichier écrit par le custom logger ?
	// nom MoosVar nom fichier, numéros nécessaire pour que 
        // les algo python écrives les nouveaux fichiers sans outliers

	char sligne[50];
        sprintf(sligne,"%d", paire_ligne_courante);
        spaire=sligne;

	string nomFichier = nomFichierLog+spaire+".txt";
	string cheminFichier="/home/guerledan3/moos-ivp-extend/missions/guerledan_mars_test/logs/pCustomLogger/"+nomFichier;

	Notify("FICHIER_A_CHECKER",cheminFichier);
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
		if(lancement){
			lancement=false;

			// arrêt du programme : log général ?
		}
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
			Notify("RETOUR_QC",0.0);
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
	if(msg.m_dfVal==0.0){ // désactivé

	}
	else if(msg.m_dfVal==1.0){ // à refaire
		if(retour_qc!=1.0){
			// à faire : déplacer le fichier de log qui ne va pas dans un fichier d'archivage
			
			Notify("LIGNE_COURANTE",0.0); // pour repasser au mode levé
		}
	}
	else if(msg.m_dfVal==2.0){ // valide et cohérent
		if(retour_qc!=2.0){
			paire_ligne_courante += 1;

			Notify("LIGNE_COURANTE",0.0); // pour repasser au mode levé
		
		}
	}

	retour_qc = msg.m_dfVal;
     }
     else if (key == "DB_UPTIME"){
	current_time = msg.m_dfVal;
     }
     else if (key == "LIGNE_COURANTE"){
	if(msg.m_dfVal==0.0){
		if(ligne_courante==2.0){
			ligne_courante=0.0;
			Notify("FIN_LEVE","false");
		}
	}
	else if(msg.m_dfVal==1.0){
		if(ligne_courante==0.0){
			ligne_courante=1.0;
			Notify("WAYPT_UPDATE","name="+spaire+"_2 # points="+ligne2+" # endflag = LIGNE_COURANTE = 2 # endflag = LOGLINE=stop");
		}
	}
	else if(msg.m_dfVal==2.0){
		if(ligne_courante==1.0){
			ligne_courante=2.0;
			Notify("FIN_LEVE","true");
		}
	}
	
     }
     else if (key == "WPT_INDEX"){

	if(msg.m_dfVal==1){
		if(ligne_courante==0.0){
			Notify("LOGLINE","INIT="+spaire);
		}
		else if(ligne_courante==1.0){
			Notify("LOGLINE","restart");
		}
	}

	waypoint = msg.m_dfVal;
     }
     else if (key == "LOGLINE"){
	logline = msg.GetString(); 
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
  Register("LIGNE_COURANTE", 0);
  Register("WPT_INDEX", 0);
  Register("LOGLINE", 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool Main::buildReport() 
{

  
  char smode[50];
  sprintf(smode,"%d", mode);

  char smotif[50];
  sprintf(smotif,"%d", motif_courant);

  char spaireligne[50];
  sprintf(spaireligne,"%d", paire_ligne_courante);

  char sligne[50];
  snprintf(sligne, 50, "%f", ligne_courante);

  char swaypoint[50];
  snprintf(swaypoint, 50, "%f", waypoint);

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
  m_msgs << "\nPaire de lignes courante : ";
  m_msgs << spaireligne;
  m_msgs << "\nLigne courante : ";
  m_msgs << sligne;
  m_msgs << "\n\nWaypoint Index : ";
  m_msgs << swaypoint;
  m_msgs << "\n\nLOGLINE : ";
  m_msgs << logline;
  m_msgs << "\n\nLigne 1 : ";
  m_msgs << ligne1;
  m_msgs << "\nLigne 2 : ";
  m_msgs << ligne2;
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




