#include "Cell.h"

Cell::Cell(){
  merge_num = 0;
  diverge_num = 0;
  flow_num = 0;
  predict_time = 0;
}

Cell::Cell(int c, int s)
{
  number = c;
  subsystem_num = s;
  merge_num = 0;
  diverge_num = 0;
  flow_num = 0;
  predict_time = 0;
  plus_time = 0;
  minus_time = 0;
}

Cell::Cell(const Cell&){}

Cell::~Cell(){}

// セルの設定
void Cell::initCell(double length, double capacity, int dummy)
{
  N = capacity;
  n = 0.0;
  density = 0.0;
  SD_density = 0.0;

  if(TEST)
  {
    L = length; // mile
  }
else 
  {
    L = length / 1000.0; // km
  }

  if(dummy == 1)
  {
    is_dummy = true;
  }
  else 
  {
    is_dummy = false;
  }
}

// パラメータを設定
void Cell::SetParameter(double velocity, double wave_speed)
{
  Vf = velocity;
  Wc = wave_speed;
  rhoJ = N / L;
  rhoc = (Wc * rhoJ) / (Vf + Wc);
  Qm = rhoc * Vf;
}

// 密度の更新
void Cell::UpdateDensity()
{  
  density = n / L;
    
  if(density > rhoJ)
  {
    density = rhoJ;
  }
}

// 最小待機時間を更新
void Cell::UpdateWaitTime()
{
  V = GreenShields(density, rhoJ, Vf);
  
  double mile = 1.0;
  if(TEST)
  {
    mile = 1.61;  // 1mile = 1.61km
  }
  if(V >= 1.0 * mile)
  {  // 1km/h以上
    predict_time = (double)L / V;
  }
  else
  {
    predict_time = (double)L / 1.0 * mile;
  }

  double SD_V;
  SD_V = GreenShields(density + SD_density, rhoJ, Vf);
  plus_time = (double)L / SD_V;

  SD_V = GreenShields(density - SD_density, rhoJ, Vf);
  minus_time = (double)L / SD_V;
}

// グリーンシールズ・オルコットの方程式
double Cell::GreenShields(double k, double kj, double vf)
{
  return vf * (1.0 - (k / kj));
}

// セルに車両を積み込む
void Cell::PushVehicle(Vehicle* vehicle)
{
  load_vehicles.push_back(vehicle);
}

// 次の行き先セルを設定
void Cell::PushNextCell(Cell* cell)
{
  next.push_back(cell);
}

// 前にいたセルを設定
void Cell::PushBeforeCell(Cell* cell)
{
  before.push_back(cell);
}
