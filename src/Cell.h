#ifndef __SCTMsim__Cell__
#define __SCTMsim__Cell__

#include "setting.h"
#include "LoadCell.h"
#include "Vehicle.h"

class Cell {
 public:
  Cell();
  Cell(int, int);
  Cell(const Cell&);
  ~Cell();
  double GreenShields(double, double, double);
  void initCell(double, double, int);
  void SetParameter(double, double);
  void UpdateWaitTime();
  void UpdateDensity();
  void PushVehicle(Vehicle*);
  void PushNextCell(Cell*);
  void PushBeforeCell(Cell*);
  void init();
    
  int number;        // セル番号
  int subsystem_num; // サブシステム番号
  int merge_num;     // セルへの流入数
  int diverge_num;   // セルからの流出数
  double flow_num;   // 交通量
  double n;        // 現在の車両台数
  double N;        // 最大容量
  double L;        // セル長[m]
  double V;        // 現在の速度[m/s]
  double Vf;       // 自由速度
  double Wc;       // 減速波の速度
  double Qm;       // 最大交通流率
  double rhoc;     // 飽和密度
  double rhoJ;     // 最大密度
  double density;  // 現在の密度
  double SD_density; // 密度の標準偏差
  double predict_time;  // 通過所用時間[s]
  double plus_time;  // 平均+SD
  double minus_time; // 平均-SD

  bool is_dummy;        // ダミーリンクかどうか
  bool is_upper_stream; // 上流か下流か（上流はtrue 下流はfalse）
  vector<Cell*> next;   // 次のセルへのベクトル
  vector<Cell*> before; // 前のセルのベクトル
  vector<Vehicle*> load_vehicles; // セル内に存在する車両ベクトル
};

#endif /* defined(__SCTMsim__Cell__) */
