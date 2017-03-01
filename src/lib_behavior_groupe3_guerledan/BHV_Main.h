/************************************************************/
/*    NAME: Groupe 3 Guerlédan                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_Main.h                                      */
/*    DATE:                                                 */
/************************************************************/

#ifndef Main_HEADER
#define Main_HEADER

#include <string>
#include "IvPBehavior.h"

class BHV_Main : public IvPBehavior {
public:
  BHV_Main(IvPDomain);
  ~BHV_Main() {};
  
  bool         setParam(std::string, std::string);
  void         onSetParamComplete();
  void         onCompleteState();
  void         onIdleState();
  void         onHelmStart();
  void         postConfigStatus();
  void         onRunToIdleState();
  void         onIdleToRunState();
  IvPFunction* onRunState();

protected: // Local Utility functions

protected: // Configuration parameters

protected: // State variables
};

#define IVP_EXPORT_FUNCTION

extern "C" {
  IVP_EXPORT_FUNCTION IvPBehavior * createBehavior(std::string name, IvPDomain domain) 
  {return new BHV_Main(domain);}
}
#endif
