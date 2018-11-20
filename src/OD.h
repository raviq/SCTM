#ifndef __SCTMsim__OD__
#define __SCTMsim__OD__

#include "setting.h"

class OD
{
 public:
  OD(int, int, int, int, int);
  OD(const OD&);
  ~OD();
  void averageVehicleNum();
    
  int number;  // OD番号
  int origin;  // 出発地ノード
  int destination; // 目的地ノード
  int load_vehicle_num; // 同時に積み込まれる車両台数
  int depart_time; // 車両の出発時間
  int load_times;  // ODデータを分割する回数
  int counter;

  vector<double> load_vehicle;
};

#endif /* defined(__SCTMsim__OD__) */
