#include "Node.h"

Node::Node(int n)
{
  number = n;
  T = 0;
  signal = false;
  starting = false;
  ending = false;
  adjacent_num = 0;
}

void Node::setSignal(map<int, vector<int> > time, vector<int> nodes)
{
  signal_times = time;
  signal = true;
  adjacent_nodes = nodes;
  adjacent_num = (int)nodes.size();
    
  // 信号ベクトルの初期化
  for(int i = 0; i < adjacent_num; i++)
  {
    signal_phase.insert(make_pair(adjacent_nodes[i], 0));
  }
    
  // サイクル時間の設定
  for(int i = 0; i < adjacent_num; i++)
  {
    if(signal_times[adjacent_nodes[i]][3] > 0)
    {
      cycle = signal_times[adjacent_nodes[i]][3];
      break;
    }
  }
    
  // 信号の色の初期化
  changeSignal();
}

Node::Node(const Node&)
{
}

Node::~Node()
{
}

void Node::UpdateNode(int nodeNum)
{
  if(!signal)
  {  // 信号の存在する交差点である場合
    adjacent_nodes.push_back(nodeNum);
    adjacent_num++;
  }
}

// 現在の信号時間の更新
void Node::UpdateTime()
{
  if(T + STEP >= cycle)
  {
    T = T + STEP - cycle;
  }
  else
  {
    T += STEP;
  }
        
  // 現在の信号の色の更新
  changeSignal();
}

// 信号時間によって信号の色を変更
void Node::changeSignal()
{
  for(int i = 0; i < adjacent_num; i++)
  {
        
    if(signal_times[adjacent_nodes[i]][3] == -1)
    {
      signal_phase[adjacent_nodes[i]] = NOSIGNAL;

    } 
    else if(signal_times[adjacent_nodes[i]][0] <= T && T < signal_times[adjacent_nodes[i]][1])
    {
      signal_phase[adjacent_nodes[i]] = BLUE;

    } 
    else if(signal_times[adjacent_nodes[i]][1] <= T && T < signal_times[adjacent_nodes[i]][2])
    {
      signal_phase[adjacent_nodes[i]] = YELLOW;

    } 
    else if(signal_times[adjacent_nodes[i]][2] > signal_times[adjacent_nodes[i]][0])
    {
      if((signal_times[adjacent_nodes[i]][2] <= T && T <= cycle) || (0 <= T && T < signal_times[adjacent_nodes[i]][0]))
      {
        signal_phase[adjacent_nodes[i]] = RED;
      }  

    }
    else 
    {
      if(signal_times[adjacent_nodes[i]][2] <= T && T < signal_times[adjacent_nodes[i]][0])
      {
        signal_phase[adjacent_nodes[i]] = RED;
      }
    }
  }
}
