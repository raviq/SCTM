#ifndef __SCTMsim__Subsystem__
#define __SCTMsim__Subsystem__

#include "setting.h"
#include "Cell.h"
#include "Node.h"
#include "Mode.h"

class Cell;
class Node;

class Subsystem
{
 public:
  Subsystem();
  Subsystem(int, Node*, Node*);
  Subsystem(const Subsystem&);
  ~Subsystem();
    
  void initMode();
  void SetParameters();
  void CalculateModeProbability();
  void CalculateDensity();
  void SetInsideFlow();
  void SetAdjacentFlow();
  void AdjustMergeFlow();
  void AdjustDivergeFlow();

  vector<vector<double> > makeSupplyNoise();
  void PushMode();
  void PushNextSub(Subsystem*);
  void PushBeforeSub(Subsystem*);
  
  int number;    // ノード番号
    
  map<int, double> inflow;      // 車両の流入台数 1:サブシステム番号 2:流入台数
  map<int, double> outflow;     // 車両の流出台数 1:サブシステム番号 2:流出台数
  map<int, double> outflow_surplus; // 車両の流出台数の余り
  
  double move_origin_num;  // 出発地から流入した車両台数のカウント
  double move_dest_num;    // 目的地へ流出した車両台数のカウント
  double move_inflow_num;  // 上流へ移動した車両台数のカウント
  double move_outflow_num; // 下流へ移動した車両台数のカウント

  double arrive_flow;         // 目的地へ流出する車両台数
  double arrive_flow_surplus; // 目的地へ流出する車両台数の余り
 
  double inside_inflow;  // 下流セルに対する流入台数
  double inside_outflow; // 上流セルに対する流出台数
  double inside_outflow_surplus; // 上流セルに対する流出台数の余り  
  

  //行0:上流セル 行1:下流セル
  vector<vector<double> > Vf;   // 自由速度
  vector<vector<double> > Qm;   // 最大交通流率
  vector<vector<double> > Wc;   // 減速波の速度
  vector<vector<double> > rhoc; // 飽和密度
  vector<vector<double> > rhoJ; // 最大密度

  MatrixXd u; // 1:流入 2:流出
  MatrixXd Q; // 密度の平均
  MatrixXd mean_density; // 密度の平均
  MatrixXd variance_density; // 密度の分散
  MatrixXd diagonal_density;

  Mode* FF;  // FFモードの密度と自己相関行列を保持
  Mode* CC;  // CCモードの密度と自己相関行列を保持
  Mode* CF;  // CFモードの密度と自己相関行列を保持
  Mode* FC1; // FC1モードの密度と自己相関行列を保持
  Mode* FC2; // FC2モードの密度と自己相関行列を保持

  double Pff;  // FFモードの発生確率
  double Pcc;  // CCモードの発生確率
  double Pcf;  // CFモードの発生確率
  double Pfc;  // FCモードの発生確率
  double Pfc1; // FC1モードの発生確率
  double Pfc2; // FC2モードの発生確率
    
  Cell* firstCell;  // 上流セル
  Cell* secondCell; // 下流セル

  Node* originNode; // 始点ノード
  Node* destNode;   // 終点ノード
    
  vector<Subsystem*> next;    // 次のサブシステム
  vector<Subsystem*> before;  // 前のサブシステム
};

#endif /* defined(__SCTMsim__Subsystem__)*/
