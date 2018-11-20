#ifndef __SCTMsim__TestSimulator__
#define __SCTMsim__TestSimulator__

#include "setting.h"
#include "Cell.h"
#include "Subsystem.h"
#include "LoadCell.h"
#include "Vehicle.h"
#include "OD.h"
#include "Node.h"
#include "RecordResult.h"

class TestSimulator
{
 public:
  TestSimulator(string);
  void start(); // シミュレーション開始

 private:
  void ReadFile(); // ファイル読み込み
  void MakeCell(); // セルとサブシステムの作成
  void ConnectCell(); // セルの接続
  void Dijkstra(vector<vector<int> >&); // ダイクストラでの最短経路選択
  void GetRoute(); // 経路を取得
  void LoadVehicle(); // 車両を搭載
  void Loop(RecordResult); // シミュレーションループ
  void MoveVehiclesStraightAndMerge (Subsystem*); // 単路、合流部の車両の移動
  void MoveVehiclesDiverge(Subsystem*); // 合流部の車両の移動
  void MoveVehiclesLoad(LoadCell*); // 積み込みセルからセルへ車両を移動

  void Clear();
  void init(); // 繰り返しのための初期化

  vector<Cell*> cells;
  vector<Subsystem*> subsystems;
  vector<LoadCell*> loads;
  vector<OD*> ods;
  vector<Vehicle*> vehicles;
  vector<Node*> nodes;

  vector<vector<vector<int> > > route; // ノードからノードへの最短ルート（コストはセルの長さ）
  vector<vector<double> > link; // データを格納する変数 0.出発地 1.目的地 2.長さ 3.最大車両容量

  FILE *fp;
  string directory; // カレントディレクトリ

  int link_num;   // リンクの数
  int subsystem_num; // サブシステムの数
  int cell_num;   // セルの数
  int load_num;   // 車両積み込みセルの数
  int od_num;     // ODの数
  int vehicle_num;// 車両数
  int signal_num; // 信号数
  int node_num;   // ノードの数

  int unit; // 分割単位
  int T; // 経過時間
  int total_OD_num;    // 合計OD
  double total_vehicle_num;    // 合計車両台数
  double exit_vehicle_num;  // 目的値に到達した車両

  // セルのパラメータ
  double freeflow_speed;  // 自由速度
  double backwave_speed;  // 後方への減速度
  double maximum_flow;    // 最大交通流率
  double critical_density;// 最大密度
  double jam_density;     // 渋滞密度
};

#endif /* defined(__SCTMsim__TestSimulator__) */
