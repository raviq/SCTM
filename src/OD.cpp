#include "OD.h"

OD::OD(int id, int orig, int dest, int num, int time)
{
  number = id;
  origin = orig;
  destination = dest;
  load_vehicle_num = num;
  depart_time = time;
  counter = 0;
  load_vehicle.resize(LOAD_TIMES);

  averageVehicleNum(); // 車両の積み込み台数を振り分け
}

OD::OD(const OD&)
{
}

OD::~OD(){}

void OD::averageVehicleNum()
{
  // 車両の搭載台数をload_times分に分割する
  for(int i = 0; i < LOAD_TIMES; i++)
  {
    load_vehicle[i] = (double)load_vehicle_num / LOAD_TIMES;
  }
}
