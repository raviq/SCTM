#include "LoadCell.h"

LoadCell::LoadCell(Subsystem* subsystem)
{
  number = subsystem -> number;
  inflow_subsystem = new Subsystem();
  inflow_subsystem = subsystem;
  n = 0;
  load_surplus = 0;
}

LoadCell::LoadCell(const LoadCell&)
{
}

LoadCell::~LoadCell()
{
  delete inflow_subsystem;
}

void LoadCell::PushVehicle(Vehicle* vehicle)
{
  load_vehicles.push_back(vehicle);
}
