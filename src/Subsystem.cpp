#include "Subsystem.h"

Subsystem::Subsystem(){}

Subsystem::Subsystem(int num, Node* origin, Node* destination)
{  
  number = num;
  originNode = origin;
  destNode = destination;

  move_origin_num = 0;
  move_dest_num = 0;
  move_inflow_num = 0;
  move_outflow_num = 0;

  arrive_flow = 0;
  arrive_flow_surplus = 0;

  inside_outflow = 0;
  inside_inflow = 0;
  inside_outflow_surplus = 0;

  // パラメータベクトルの初期化
  Vf.resize(2);
  Qm.resize(2);
  Wc.resize(2);
  rhoc.resize(2);
  rhoJ.resize(2);
  for(int i = 0; i < 2; i++)
  {
    Vf[i].resize(NOISE_NUM, 0);
    Qm[i].resize(NOISE_NUM, 0);
    Wc[i].resize(NOISE_NUM, 0);
    rhoc[i].resize(NOISE_NUM, 0);
    rhoJ[i].resize(NOISE_NUM, 0);
  }

  FF = new Mode();
  CC = new Mode();
  CF = new Mode();
  FC1 = new Mode();
  FC2 = new Mode();

  u = MatrixXd::Zero(2,1);
  Q = MatrixXd::Zero(2,2);
  mean_density = MatrixXd::Zero(2,1);
  variance_density = MatrixXd::Zero(2,1);
  diagonal_density = MatrixXd::Zero(2,2);

  Pff = 1;
  Pcc = 0;
  Pcf = 0;
  Pfc = 0;
  Pfc1 = 0;
  Pfc2 = 0;
}

Subsystem::Subsystem(const Subsystem&){}

Subsystem::~Subsystem()
{
}

void Subsystem::initMode()
{
  // パラメータにノイズを付加
  SetParameters();

  double first_unit = STEP / 3600.0 / firstCell -> L;
  double second_unit = STEP / 3600.0 / secondCell -> L;

  // FFモードの初期化
  FF -> A1(0,0) = - first_unit;
  FF -> A1(1,0) = second_unit;
  FF -> B(0,0) = first_unit;

  if(REVISION)
  {
    FF -> B(1,1) = - second_unit;
  }
   else
	{
    FF -> A2(1,1) = - second_unit;
  }

  FF -> omega(0,0) = firstCell -> Vf;
  FF -> omega(1,0) = secondCell -> Vf;
  FF -> gamma(0,0) = pow(FF -> omega(0,0) * RATIO, 2);
  FF -> gamma(1,0) = pow(FF -> omega(1,0) * RATIO, 2);

  // CCモードの初期化    
  CC -> A2(0,1) = first_unit;
  CC -> A2(1,1) = - second_unit;
  CC -> B(1,1) = - second_unit;
  CC -> B2(0,1) = - first_unit;
  CC -> B2(1,1) = second_unit;

  if(REVISION)
  {
    CC -> B(0,0) = first_unit;
  } 
  else 
  {
    CC -> A1(0,0) = - first_unit;
    CC -> B1(0,0) = first_unit;
  }
    
  CC -> omega(0,0) = firstCell -> Wc;
  CC -> omega(1,0) = secondCell -> Wc;
  CC -> lambda(0,0) = firstCell -> rhoJ;
  CC -> lambda(1,0) = secondCell -> rhoJ;
  CC -> gamma(0,0) = pow(CC -> omega(0,0) * RATIO, 2);
  CC -> gamma(1,0) = pow(CC -> omega(1,0) * RATIO, 2);

  // CFモードの初期化
  CF -> B0(0,1) = - first_unit;
  CF -> B0(1,1) = second_unit;

  if(REVISION)
  {
    CF -> B(0,0) = first_unit;
    CF -> B(1,1) = - second_unit;
  } 
  else 
  {
    CF -> A1(0,0) = - first_unit;
    CF -> A2(1,1) = - second_unit;
    CF -> B1(0,0) = first_unit;
  }

  CF -> omega(0,0) = firstCell -> Wc;
  CF -> omega(1,0) = secondCell -> Vf;
  CF -> lambda(0,0) = firstCell -> rhoJ;
  CF -> lambda(1,0) = secondCell -> Qm;
  CF -> gamma(0,0) = pow(CF -> omega(0,0) * RATIO, 2);
  CF -> gamma(1,0) = pow(CF -> omega(1,0) * RATIO, 2);
  
  // FC1モードの初期化    
  FC1 -> A1(0,0) = - first_unit;
  FC1 -> A1(1,0) = second_unit;
  FC1 -> B(0,0) = first_unit;
  FC1 -> B(1,1) = - second_unit;
  FC1 -> omega(0,0) = firstCell -> Vf;
  FC1 -> gamma(0,0) = pow(FC1 -> omega(0,0) * RATIO, 2);

  // FC2モードの初期化
  FC2 -> A2(0,1) = first_unit;
  FC2 -> A2(1,1) = - second_unit;
  FC2 -> B2(0,1) = - first_unit;
  FC2 -> B2(1,1) = second_unit;
  FC2 -> B(0,0) = first_unit;
  FC2 -> B(1,1) = - second_unit;
  FC2 -> omega(1,0) = secondCell -> Wc;
  FC2 -> lambda(1,0) = secondCell -> rhoJ;
  FC2 -> gamma(1,0) = pow(FC2 -> omega(1,0) * RATIO, 2);
}


// パラメータにノイズを付加
void Subsystem::SetParameters()
{
  // ノイズのパターン別にノイズを付加したパラメータを代入
  for (int i = 0; i < NOISE_NUM; i++)
  {
    vector<vector<double> > supplyNoise;
    supplyNoise = makeSupplyNoise();
        
    Vf[0][i] = firstCell -> Vf + supplyNoise[0][0];
    Wc[0][i] = firstCell -> Wc + supplyNoise[0][1];
    Qm[0][i] = firstCell -> Qm + supplyNoise[0][2];
    rhoc[0][i] = Qm[0][i] / Vf[0][i];
    rhoJ[0][i] = (Qm[0][i] / Wc[0][i]) + rhoc[0][i];

    Vf[1][i] = secondCell -> Vf + supplyNoise[1][0];
    Wc[1][i] = secondCell -> Wc + supplyNoise[1][1];
    Qm[1][i] = secondCell -> Qm + supplyNoise[1][2];
    rhoc[1][i] = Qm[1][i] / Vf[1][i];
    rhoJ[1][i] = (Qm[1][i] / Wc[1][i]) + rhoc[1][i];
  }
}

// サブシステム間の車両の移動台数を求める
void Subsystem::SetAdjacentFlow()
{

  double origin_unit = STEP / 3600.0;

  // サブシステムから目的地への移動台数の決定
  double Ps1 = Pff + Pcf;
  double Ps2 =  Pcc + Pfc;
  double send1 = secondCell -> Vf * secondCell -> density;
  double send2 = secondCell -> Qm;
  double send = origin_unit * (Ps1 * send1 + Ps2 * send2);
  
  // 目的地に到着させる車両台数を決定
  if(destNode -> ending)
    arrive_flow = send;
  
  // サブシステムからサブシステムへの移動台数の決定
  for(int j = 0; j < next.size(); j++)
  {
    
    double dest_unit = STEP / 3600.0;
    
    int next_num = next[j] -> number;
    int sum1 = 0;
    int sum2 = 0;
    int sum3 = 0;
    int sum4 = 0;
    
    double Pf = next[j] -> Pff + next[j] -> Pfc;
    double Pc = next[j] -> Pcc + next[j] -> Pcf;
    
    // 移動先のサブシステムの状況にあわせて移動する車両台数を決定する
    for(int k = 0; k < NOISE_NUM; k++)
    {
      if(k < Pf * NOISE_NUM)
      {
        if(send < dest_unit * next[j] -> Qm[0][k])
        {
          sum1++;
        }
        else 
        {
          sum2++;
        }
        
      }
      else 
      {
        if(send < dest_unit * next[j] -> Wc[0][k] * (next[j] -> rhoJ[0][k] - next[j] -> firstCell -> density))
        {
          sum3++;
        }
        else
        {
          sum4++;
        }
      }
    }
    
    // 各イベントの生起確率
    double P1 = (double)sum1 / NOISE_NUM;
    double P2 = (double)sum2 / NOISE_NUM;
    double P3 = (double)sum3 / NOISE_NUM;
    double P4 = (double)sum4 / NOISE_NUM;
    
    // 各イベントの車両の移動台数
    double flow1 = send;
    double flow2 = dest_unit * next[j] -> firstCell -> Qm;
    double flow3 = send;
    double flow4 = dest_unit * next[j] -> firstCell -> Wc * (next[j] -> firstCell -> rhoJ - next[j] -> firstCell -> density);
    
    double out_flow = P1 * flow1 + P2 * flow2 + P3 * flow3 + P4 * flow4;
    
    // サブシステムの上流の流入を設定
    outflow[next_num] = out_flow;
    next[j] -> inflow[number] = out_flow;
  }
}


// サブシステム内部の車両の移動台数を決定
void Subsystem::SetInsideFlow()
{
  double unit = STEP / 3600.0;

  // モードによる車両の流出台数
  double flowFF = firstCell -> density * firstCell -> Vf;
  double flowCC = secondCell -> Wc * (secondCell -> rhoJ - secondCell -> density);
  double flowCF = secondCell -> Qm;
  double flowFC1 = firstCell -> density  * firstCell -> Vf;
  double flowFC2 = secondCell -> Wc * (secondCell -> rhoJ - secondCell -> density);
    
  // 各確率と車両の流出台数の積の和で最終的な車両の流出台数を算出
  double inside_flow = unit * (Pff * flowFF + Pcc * flowCC + Pcf * flowCF + Pfc1 * flowFC1 + Pfc2 * flowFC2);
    
  // 上流セルの流出台数と下流セルの流入台数を設定
  inside_outflow = inside_flow;
  inside_inflow = inside_flow;
}


// 合流に応じて移動させる車両台数を調節する
void Subsystem::AdjustMergeFlow()
{
  // 合流部の車両の合計流入台数を取得
  double capasity = firstCell -> N - firstCell -> n;
  double flowSum = 0;
  for(int i = 0; i < inflow.size(); i++)
  {
    flowSum += inflow[before[i] -> number];
  }
    
  // 移動先に空きがない場合
  if(flowSum <= 0 || capasity < 0)
  {

    // 移動台数を全て0に更新
    for(int i = 0; i < before.size(); i++)
    {
      inflow[before[i] -> number] = 0.0;
      before[i] -> outflow[number] = 0.0;        
    }
            
  // 合流台数が空き台数以下だった場合はそのまま合流させる
  } else if(flowSum > capasity)
  { 

    for(int i = 0; i < inflow.size(); i++)
    {
      int before_num = before[i] -> number;
                
      // ネットワークに設定された合流率
      if(TEST)
      {
        if(before_num == 4)
        {
          before[i] -> outflow[number] = capasity * (1.0 / 3.0);
          inflow[before_num] = capasity * (1.0 / 3.0);

        }
        else if(before_num == 2)
        {
          before[i] -> outflow[number] = capasity * (2.0 / 3.0);
          inflow[before_num] = capasity * (2.0 / 3.0);
        }

      // 流入台数の比で車両を移動(移動先の空き台数 × 移動元の移動台数 / 合計流入台数)
      }
      else
      { 
        before[i] -> outflow[number] = capasity * inflow[before_num] / flowSum;
        inflow[before_num] = capasity * inflow[before_num] / flowSum;
      }
    }
  }
}


// 分流に応じて移動させる車両台数を調節する
void Subsystem::AdjustDivergeFlow()
{
  // 初期化  
  double arrive_sum = 0.0;
  map<int, double> diverge_sum;
  for(vector<Subsystem*>::iterator it = next.begin(); it != next.end(); it++)
  {
    Subsystem* nextSubsystem = *it;
    diverge_sum.insert(make_pair(nextSubsystem -> number, 0.0));
  }

  // 分岐先ごとに車両台数を計算
  for(vector<Vehicle*>::iterator v_it = secondCell -> load_vehicles.begin(); v_it != secondCell -> load_vehicles.end(); v_it++)
  {
    Vehicle* vehicle = *v_it;
    int next_node = vehicle -> route[destNode -> number][vehicle -> destination];

    // 目的地に到達していた場合
    if(next_node == -1)
    {
      arrive_sum += vehicle -> loads;
      continue;
    }

    for(vector<Subsystem*>::iterator s_it = next.begin(); s_it != next.end(); s_it++)
    {
      Subsystem* nextSubsystem = *s_it;

      if(next_node == nextSubsystem -> destNode -> number)
      {
        diverge_sum[nextSubsystem -> number] += vehicle -> loads;
        break;
      }
    }
  }
 
  for(vector<Subsystem*>::iterator s_it = next.begin(); s_it != next.end(); s_it++)
  {
    Subsystem* nextSubsystem = *s_it;
    int next_num = nextSubsystem -> number;
    double turning_ratio = 1.0; // 分岐率を設定

    if(TEST)
    {
      // ネットワークに設定されている分岐率
      if(next_num == 3)
      {
        turning_ratio = 1.0 / 3.0;

      } 
      else if(next_num == 4)
      {
        if(number == 1)
        {
          turning_ratio = 1.0 / 3.0;

        }
        else if(number == 3)
        {
          turning_ratio = 2.0 / 3.0;

        }
      }
      else if(next_num == 5)
      {
        turning_ratio = 2.0 / 3.0;

      } else if(next_num == 6)
      {
        if(number == 1)
        {
          turning_ratio = 2.0 / 3.0;

        }
        else if(number == 3)
        {
          turning_ratio = 1.0 / 3.0;
        }
      }

    } 
    else 
    {
      // 分岐ごとの比で分岐率を決定
      if(secondCell -> n > 0)
      {
        turning_ratio = diverge_sum[next_num] / secondCell -> n;
      }
      else
      {
        turning_ratio = 1.0;
      }
    }
  
    outflow[next_num] = outflow[next_num] * turning_ratio;
  }

  if(!TEST) arrive_flow *= (arrive_sum / secondCell -> n);
  diverge_sum.clear();
}


// 密度の平均と分散を計算
void Subsystem::CalculateDensity()
{
  // 流入と流出のパラメータu
  u = MatrixXd::Zero(2,1);
  
  // 流入台数の更新
  u(0,0) += move_inflow_num;
  u(0,0) += move_origin_num;
  
  // 流出台数の更新
  u(1,0) += move_outflow_num;
  u(1,0) += move_dest_num;

  // 後にSTEPをかけるため
  u(0,0) /= (STEP / 3600.0);
  u(1,0) /= (STEP / 3600.0);
  
  // 初期化
  move_origin_num = 0;
  move_dest_num = 0;
  move_inflow_num = 0;
  move_outflow_num = 0;

  // 各モードにおける密度の平均値を計算
  FF -> GetDensity(u, mean_density);
  CC -> GetDensity(u, mean_density);
  CF -> GetDensity(u, mean_density);
  FC1 -> GetDensity(u, mean_density);
  FC2 -> GetDensity(u, mean_density);

  // 各モードにおける自己相関行列を計算
  FF -> GetAutoCorrelation(u, mean_density, Q);
  CC -> GetAutoCorrelation(u, mean_density, Q);
  CF -> GetAutoCorrelation(u, mean_density, Q);
  FC1 -> GetAutoCorrelation(u, mean_density, Q);
  FC2 -> GetAutoCorrelation(u, mean_density, Q);

  // 密度の平均値を計算
  mean_density = Pff * FF -> rho + Pcc * CC -> rho + Pcf * CF -> rho + Pfc1 * FC1 -> rho + Pfc2 * FC2 -> rho;

  // 密度の自己相関行列を計算
  Q = Pff * FF -> Q + Pcc * CC -> Q + Pcf * CF -> Q + Pfc1 * FC1 -> Q + Pfc2 * FC2 -> Q;

  // 密度の分散を計算
  diagonal_density = Q - (mean_density * mean_density.transpose());

  variance_density(0,0) = diagonal_density(0,0);
  variance_density(1,0) = diagonal_density(1,1);

  if(variance_density(0,0) < 0)
  {
    variance_density(0,0) = -variance_density(0,0);
  }
  if(variance_density(1,0) < 0)
  {
    variance_density(1,0) = -variance_density(1,0);
  }

  firstCell -> SD_density = variance_density(0,0);
  secondCell -> SD_density = variance_density(1,0);
}


// 密度によってモードを決定
void Subsystem::CalculateModeProbability()
{
  Pff = 0;
  Pcc = 0;
  Pcf = 0;
  double Pdfc = 0;
  double Pufc = 0;
  Pfc1 = 0;
  Pfc2 = 0;
    
  int sumFF = 0;
  int sumCC = 0;
  int sumCF = 0;
  int sumFC = 0;
  int sumDFC = 0;
    
  // ノイズの数に対するモードの発生確率を計算
  for(int i = 0; i < NOISE_NUM; i++){
    if(firstCell -> density < rhoc[0][i] && secondCell -> density < rhoc[1][i])
    {
      sumFF++;
    } 
    else if(firstCell -> density >= rhoc[0][i] && secondCell -> density >= rhoc[1][i])
    {
      sumCC++;
    } 
    else if(firstCell -> density >= rhoc[0][i] && secondCell -> density < rhoc[1][i])
    {
      sumCF++;
    }
    else
	{
      sumFC++;
    }
  }
    
  if(sumFC != 0)
  {
    for(int i = 0; i < sumFC; i++)
    {
      if(Vf[0][i] * firstCell -> density <= Wc[1][i] * (rhoJ[1][i] - secondCell -> density))
      {
        sumDFC++;
      }
    }
  }

  Pff = (double)sumFF / NOISE_NUM;
  Pcc = (double)sumCC / NOISE_NUM;
  Pcf = (double)sumCF / NOISE_NUM;
  Pfc = 1 - (Pff + Pcc + Pcf);
    
  if(sumFC != 0)
  {
    Pdfc = (double)sumDFC / sumFC;
  }
  else 
  {
    Pdfc = 1;
  }

  Pufc = 1 - Pdfc;
  Pfc1 = Pdfc * Pfc;
  Pfc2 = Pufc * Pfc;
}
               
// 正規乱数発生処理
vector<vector<double> > Subsystem::makeSupplyNoise()
{
  vector<vector<double> > sd; // 標準偏差
  vector<vector<double> > supplyNoise; // ノイズベクトル
  double mean = 0.0; // 平均値
  int num = 0; // ノイズの列数

  // ベクトルの初期化
  sd.resize(2);
  supplyNoise.resize(2);
  for(int i = 0; i < 2; i++)
  {
    sd[i].resize(3);
    supplyNoise[i].resize(3);
    supplyNoise[i].assign(3,0);
  }
    
  // 標準偏差の10%をノイズの範囲とする
  for(int i = 0; i < 3; i++)
  {
        
    if(i == 0)
    {
      sd[0][i] = firstCell -> Vf * RATIO;
      sd[1][i] = secondCell -> Vf * RATIO;
    }
    else if(i == 1)
    {
      sd[0][i] = firstCell -> Wc * RATIO;
      sd[1][i] = secondCell -> Wc * RATIO;
    }
    else
    {
      sd[0][i] = firstCell -> Qm * RATIO;
      sd[1][i] = secondCell -> Qm * RATIO;
    }
  }
    
  // 正規乱数を発生させる
  while(num != 3)
  {
    double whiteGaussianNoise1; // ホワイトガウスノイズ1
    double whiteGaussianNoise2; // ホワイトガウスノイズ2
        
    for(int i = 0; i < 2; i++)
    {
      bool sw = false;
            
      if(!sw)
      {
        double PI = pi<double>(); // 円周率
        double randNum_1 = ((double)rand()) / RAND_MAX; // (0,9]の乱数取得
        double randNum_2 = ((double)rand()) / RAND_MAX; // (0,9]の乱数取得
        sw = true;
                
        whiteGaussianNoise1 = sd[i][num] * sqrt(-2.0 * log(randNum_1)) * cos(2.0 * PI * randNum_2) + mean; // 正規乱数1
        whiteGaussianNoise2 = sd[i][num] * sqrt(-2.0 * log(randNum_1)) * sin(2.0 * PI * randNum_2) + mean; // 正規乱数2
                
        supplyNoise[i][num] = whiteGaussianNoise1;
      } 
      else 
      {
        sw = false;
        supplyNoise[i][num] = whiteGaussianNoise2;
      }
    }
    num++;
  }
    
  return supplyNoise;
}

void Subsystem::PushNextSub(Subsystem* sub)
{
  next.push_back(sub);
}

void Subsystem::PushBeforeSub(Subsystem* sub)
{
  before.push_back(sub);
}
