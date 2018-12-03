#include "Vehicle.h"

Vehicle::Vehicle(int i)
{
  number = i;
  stay_time = 0;
  link_time = 0;
  sum_time = 0;
  travel_distance = 0;
  loads = 0;
  move = false;
  arrival = false;
  use_dial = false;
}

Vehicle::Vehicle(const Vehicle& vehicle)
{
}

Vehicle::~Vehicle()
{   
}

void Vehicle::PushPass(int pass)
{
  pass_route.push_back(pass);
}

void Vehicle::PushTransitTime()
{
  transit_link_time.push_back(link_time);
}
