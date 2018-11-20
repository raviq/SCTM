#ifndef __SCTMsim__RecordResult__
#define __SCTMsim__RecordResult__

#include "setting.h"
#include "Subsystem.h"
#include "Vehicle.h"
#include "Node.h"

class RecordResult
{
 public:
  RecordResult(string);
  ~RecordResult();
    
  void RecordResultStep(int, vector<Subsystem*>, vector<Node*>); //ステップごとにシミュレーション結果を記録
  void RecordDensity(vector<Subsystem*>);        // 密度の時間推移を記録
  void RecordMoveNumber(vector<Subsystem*>);     // 移動台数の時間推移を記録
  void RecordSurplus(vector<Subsystem*>);        // 移動台数の余りの時間推移を記録
  void RecordAdjacentFlow(vector<Subsystem*>);   // サブシステム間の移動台数の時間推移を記録
  void RecordFreeSpace(vector<Subsystem*>);      // 空き台数の時間推移を記録
  void RecordVehicleNumber(vector<Subsystem*>);  // 車両台数の時間推移を記録
  void RecordTrafficVolume(vector<Subsystem*>);  // 各セルの1分ごとの断面交通量の変化を記録
  void RecordNecessaryTime(vector<Subsystem*>);    // セルの通過所要時間の時間推移を記録
  void RecordModeTransition(vector<Subsystem*>); // モードの発生確率の時間推移を記録
  void RecordSignalChange(vector<Node*>);        // 信号の変化を記録
  void RecordInflowOutflow(vector<Subsystem*>);
  void RecordRouteCost(vector<vector<double> >); // 経過時間ごとの経路コストを記録

  void RecordResultIteration(int, vector<Vehicle*>);
  void RecordOriginDestTime(vector<Vehicle*>);  // 繰り返しごとの各車両の移動時間と距離を記録
  void RecordPassedRoute(vector<Vehicle*>);      // 車両の通過経路を記録
  void RecordEachMoveTime(vector<Vehicle*>);     // 各セルでの車両の移動時間を記録
    
  FILE *fp;
  string directory; // カレントディレクトリ
    
  int simulation_time;
  bool first_record;
};

#endif /* defined(__SCTMsim__RecordResult__) */
