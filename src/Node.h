#ifndef __SCTMsim__Node__
#define __SCTMsim__Node__

#include "setting.h"

class Node
{
 public:
  Node(int);
  Node(int, map<int, vector<int> >, vector<int>);
  Node(const Node&);
  ~Node();
  void setSignal(map<int, vector<int> >, vector<int>);
  void UpdateNode(int); // ノードの情報を更新
  void UpdateTime(); // 信号の時間を更新
    
  int number;       // ノード番号
  int adjacent_num; // 隣接しているノードの数
  bool signal;      // 信号の有無
  bool starting;    // 起点かどうか
  bool ending;      // 終点かどうか

  map<int, int> signal_phase; // 現在の信号の色を更新
  vector<int> adjacent_nodes; // vector 0:北 1:東 2:南 3:西 のノード
    
 private:
  void changeSignal(); // 現在の信号の色を更新
    
  int cycle; // 信号のサイクル時間
  int T;     // 現在の時刻
    
  // map key:周辺のノード value:信号の変化時間ベクトル
  // vector 0:青になる時刻 1:黄色になる時刻 2:赤になる時刻 3:サイクル時間
  map<int, vector<int> > signal_times;
};

#endif /* defined(__SCTMsim__Node__) */
