#ifndef __SCTMsim__LoadCell__
#define __SCTMsim__LoadCell__

#include "setting.h"
#include "Cell.h"
#include "Subsystem.h"
#include "Vehicle.h"

class Subsystem;

class LoadCell
{
 public:
  LoadCell(Subsystem*);
  LoadCell(const LoadCell&);
  ~LoadCell();
  void PushVehicle(Vehicle*);
    
  int number; // 流入先セルの番号
  double n;
  double load_surplus;
  Subsystem* inflow_subsystem; // 流入先のサブシステム
  vector<Vehicle*> load_vehicles; // 積み込まれている車両
};

#endif /* defined(__SCTMsim__LoadCell__) */
