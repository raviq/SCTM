#include "RecordResult.h"

RecordResult::RecordResult(string dir)
{
  directory = dir;
  first_record = true;
}

RecordResult::~RecordResult()
{
}

void RecordResult::RecordResultStep(int time, vector<Subsystem*> subsystems, vector<Node*> nodes){
  simulation_time = time;  //シミュレーション時間を更新

  RecordDensity(subsystems);        // 各セルの密度を記録
  RecordMoveNumber(subsystems);     // 各セルの移動台数を記録
  RecordSurplus(subsystems);        // 各セルの移動台数の余りを記録
  RecordAdjacentFlow(subsystems);   // サブシステム間の移動台数を記録
  RecordFreeSpace(subsystems);      // 各セルの空き台数を記録
  RecordTrafficVolume(subsystems);  // セルの断面交通量を記録
  RecordVehicleNumber(subsystems);  // セルの車両台数を記録
  RecordInflowOutflow(subsystems); // サブシステムのuを記録
  RecordNecessaryTime(subsystems);  // 各セルの通過所要時間を記録
  RecordModeTransition(subsystems); // モードの生起確率を記録
  RecordSignalChange(nodes);        // 信号現示の変化を記録
    
  first_record = false;
}


// セルの密度の分散を各セルごとに記録
void RecordResult::RecordDensity(vector<Subsystem*> subsystems)
{

  for(int i = 0 ; i < subsystems.size(); i++){
    for(int j = 0; j < 2; j++){    
      stringstream ss;
      ss << directory << "/result/density/sctm_result_density_" << i * 2 + j << ".csv";
      string filename = ss.str();
    
      if(first_record){
        if((fp = fopen(filename.c_str(), "w")) == NULL)
        {
          cout << "Error: can't Record result density" << endl;
          exit(EXIT_FAILURE);
        }
      
        fprintf(fp, "Time,Mean,Mean+SD,Mean-SD,SD,Real");
        fprintf(fp, "\n");
      
      } else {
        if((fp = fopen(filename.c_str(), "a")) == NULL)
        {
          cout << "Error: can't Record result density" << endl;
          exit(EXIT_FAILURE);
        }
      }
    
      fprintf(fp, "%d", simulation_time);

      if(TEST){
        fprintf(fp, ",%lf", subsystems[i] -> mean_density(j));
        fprintf(fp, ",%lf", subsystems[i] -> mean_density(j) + sqrt(subsystems[i] -> variance_density(j)));
        fprintf(fp, ",%lf", subsystems[i] -> mean_density(j) - sqrt(subsystems[i] -> variance_density(j)));
        fprintf(fp, ",%lf", subsystems[i] -> variance_density(j));
        if(j == 0)
        {
          fprintf(fp, ",%lf", subsystems[i] -> firstCell -> density);
        } 
        else 
        {
          fprintf(fp, ",%lf", subsystems[i] -> secondCell -> density);
        }
      } 
      else 
      {
        fprintf(fp, ",%lf", subsystems[i] -> mean_density(j));
        fprintf(fp, ",%lf", subsystems[i] -> mean_density(j) + sqrt(subsystems[i] -> variance_density(j)));
        fprintf(fp, ",%lf", subsystems[i] -> mean_density(j) - sqrt(subsystems[i] -> variance_density(j)));
        fprintf(fp, ",%lf", subsystems[i] -> variance_density(j));
        if(j == 0)
        {
          fprintf(fp, ",%lf", subsystems[i] -> firstCell -> density);
        } 
        else 
        {
          fprintf(fp, ",%lf", subsystems[i] -> secondCell -> density);
        }
      }

      fprintf(fp, "\n");

      fclose(fp);
    }
  }
}


// セルの密度の分散を各セルごとに記録
void RecordResult::RecordMoveNumber(vector<Subsystem*> subsystems)
{

  for(int i = 0 ; i < subsystems.size(); i++)
  {
    stringstream ss;
    ss << directory << "/result/outflow/sctm_result_move_number_" << i << ".csv";
    string filename = ss.str();
    
    if(first_record){
      if((fp = fopen(filename.c_str(), "w")) == NULL)
      {
        cout << "Error: can't Record result move number" << endl;
        exit(EXIT_FAILURE);
      }
        
      fprintf(fp, "Time");
      for(int j = 0; j < subsystems[i] -> next.size(); j++)
      {
        fprintf(fp, ",Subsystem %d", subsystems[i] -> next[j] -> number);
      }
      fprintf(fp, "\n");

    } else {
      if((fp = fopen(filename.c_str(), "a")) == NULL)
      {
        cout << "Error: can't Record result move number" << endl;
        exit(EXIT_FAILURE);
      }
    }
    
    fprintf(fp, "%d", simulation_time);
    for(int j = 0; j < subsystems[i] -> next.size(); j++)
    {
      fprintf(fp, ",%lf", (subsystems[i] -> outflow[subsystems[i] -> next[j] -> number]));
    }
    fprintf(fp, "\n");
    fclose(fp);
  }
}

// セルの密度の分散を各セルごとに記録
void RecordResult::RecordSurplus(vector<Subsystem*> subsystems)
{

  for(int i = 0 ; i < subsystems.size(); i++)
  {
    stringstream ss;
    ss << directory << "/result/surplus/sctm_result_surplus_outflow_" << i << ".csv";
    string filename = ss.str();
    
    if(first_record)
    {
      if((fp = fopen(filename.c_str(), "w")) == NULL)
      {
        cout << "Error: can't Record result surplus" << endl;
        exit(EXIT_FAILURE);
      }
        
      fprintf(fp, "Time");
      for(int j = 0; j < subsystems[i] -> next.size(); j++)
      {
        fprintf(fp, ",Subsystem %d", subsystems[i] -> next[j] -> number);
      }
      fprintf(fp, "\n");

    } 
    else
     {
      if((fp = fopen(filename.c_str(), "a")) == NULL)
      {
        cout << "Error: can't Record result surplus outflow" << endl;
        exit(EXIT_FAILURE);
      }
    }
    
    fprintf(fp, "%d", simulation_time);
    for(int j = 0; j < subsystems[i] -> next.size(); j++)
    {
      fprintf(fp, ",%lf", (subsystems[i] -> outflow_surplus[subsystems[i] -> next[j] -> number]));
    }
    fprintf(fp, "\n");

    fclose(fp);
  }
}

// セルの空き台数の経時的変化を記録
void RecordResult::RecordAdjacentFlow(vector<Subsystem*> subsystems)
{
  stringstream ss;
  ss << directory << "/result/sctm_result_adjacent_flow.csv";
  string filename = ss.str();
    
  if(first_record)
  {
    if((fp = fopen(filename.c_str(), "w")) == NULL)
    {
      cout << "Error: can't Record result free space" << endl;
      exit(EXIT_FAILURE);
    }
        
    fprintf(fp, "Time");
    for(int i = 0; i < subsystems.size(); i++)
    {
      fprintf(fp, ",Subsystem %d", i);
    }
    fprintf(fp, "\n");
        
  } 
  else 
  {
    if((fp = fopen(filename.c_str(), "a")) == NULL)
    {
      cout << "Error: can't Record result free space" << endl;
      exit(EXIT_FAILURE);
    }
  }
    
  fprintf(fp, "%d", simulation_time);
  for(int i = 0; i < subsystems.size(); i++)
  {
    fprintf(fp, ",%lf", subsystems[i] -> inside_outflow);
  }
  fprintf(fp, "\n");
    
  fclose(fp);
}


// セルの空き台数の経時的変化を記録
void RecordResult::RecordFreeSpace(vector<Subsystem*> subsystems)
{
  stringstream ss;
  ss << directory << "/result/sctm_result_freespace.csv";
  string filename = ss.str();
    
  if(first_record){
    if((fp = fopen(filename.c_str(), "w")) == NULL)
    {
      cout << "Error: can't Record result free space" << endl;
      exit(EXIT_FAILURE);
    }
        
    fprintf(fp, "Time");
    for(int i = 0; i < subsystems.size() * 2; i++)
    {
      fprintf(fp, ",Cell %d", i);
    }
    fprintf(fp, "\n");

  } 
  else 
  {
    if((fp = fopen(filename.c_str(), "a")) == NULL)
    {
      cout << "Error: can't Record result free space" << endl;
      exit(EXIT_FAILURE);
    }
  }
    
  fprintf(fp, "%d", simulation_time);
  for(int i = 0; i < subsystems.size(); i++)
  {
    fprintf(fp, ",%lf", (subsystems[i] -> firstCell -> N - subsystems[i] -> firstCell -> n));
    fprintf(fp, ",%lf", (subsystems[i] -> secondCell -> N - subsystems[i] -> secondCell -> n));
  }
  fprintf(fp, "\n");
    
  fclose(fp);
}

// セルの車両台数の経時的変化を記録
void RecordResult::RecordVehicleNumber(vector<Subsystem*> subsystems)
{
  stringstream ss;
  ss << directory << "/result/sctm_result_vehicle_number.csv";
  string filename = ss.str();
    
  if(first_record)
  {
    if((fp = fopen(filename.c_str(), "w")) == NULL)
    {
      cout << "Error: can't Record result the number of vehicle" << endl;
      exit(EXIT_FAILURE);
    }
        
    fprintf(fp, "Time");
    for(int i = 0; i < subsystems.size() * 2; i++)
    {
      fprintf(fp, ",Cell %d", i);
    }
    fprintf(fp, "\n");

  } 
  else 
  {
    if((fp = fopen(filename.c_str(), "a")) == NULL)
    {
      cout << "Error: can't Record result the number of vehicle" << endl;
      exit(EXIT_FAILURE);
    }
  }
    
  fprintf(fp, "%d", simulation_time);
  for(int i = 0; i < subsystems.size(); i++)
  {
    fprintf(fp, ",%lf", (subsystems[i] -> firstCell -> n));
    fprintf(fp, ",%lf", (subsystems[i] -> secondCell -> n));
  }
  fprintf(fp, "\n");
    
  fclose(fp);
}

// 10分間隔の交通流量
void RecordResult::RecordTrafficVolume(vector<Subsystem*> subsystems)
{

  if(first_record){
    for(int i = 0; i < subsystems.size(); i++){
      for(int j = 0; j < 2; j++){
        stringstream ss;
        ss << directory << "/result/traffic_volume/sctm_result_traffic_volume_" << i * 2 + j << ".csv";
        string filename = ss.str();
        
        if((fp = fopen(filename.c_str(), "w")) == NULL)
        {
          cout << "Error: can't Record result traffic volume" << endl;
          cout << "test" << endl;
          exit(EXIT_FAILURE);
        }
        fprintf(fp, "Time,Flow\n");

        fclose(fp);
      }
    }
  }

  if(simulation_time % (60 * 10) == 0 && simulation_time >= (60 * 10))
  {
    for(int i = 0; i < subsystems.size(); i++)
    {
      for(int j = 0; j < 2; j++)
      {
        stringstream ss;
        ss << directory << "/result/traffic_volume/sctm_result_traffic_volume_" << i * 2 + j << ".csv";
        string filename = ss.str();

        if((fp = fopen(filename.c_str(), "a")) == NULL)
        {
          cout << "Error: can't Record result traffic volume" << endl;
          exit(EXIT_FAILURE);
        }

        fprintf(fp, "%d", simulation_time);
        if(j == 0)
        {
          fprintf(fp, ",%lf", subsystems[i] -> firstCell -> flow_num);
          subsystems[i] -> firstCell -> flow_num = 0;
        } 
        else 
        {
          fprintf(fp, ",%lf", subsystems[i] -> secondCell -> flow_num);
          subsystems[i] -> secondCell -> flow_num = 0;
        }
        fprintf(fp, "\n");

        fclose(fp);
      }
    }
  }
}

void RecordResult::RecordNecessaryTime(vector<Subsystem*> subsystems)
{
    
  for(int i = 0 ; i < subsystems.size(); i++)
  {
    stringstream ss1;
    ss1 << directory << "/result/time/sctm_result_necessary_time_" << i << ".csv";
    string filename = ss1.str();
    
    if(first_record)
    {
      if((fp = fopen(filename.c_str(), "w")) == NULL)
      {
        cout << "Error: can't Record result necessary time" << endl;
        exit(EXIT_FAILURE);
      }
      
      fprintf(fp, "Time,Mean,Mean+SD,Mean-SD,SD");
      fprintf(fp, "\n");
      
    } 
    else 
    {
      if((fp = fopen(filename.c_str(), "a")) == NULL)
      {
        cout << "Error: can't Record result density" << endl;
        exit(EXIT_FAILURE);
      }
    }
    
    double mean_time = subsystems[i] -> firstCell -> predict_time + subsystems[i] -> secondCell -> predict_time;
    double plus_time = subsystems[i] -> firstCell -> plus_time + subsystems[i] -> secondCell -> plus_time;
    double minus_time = subsystems[i] -> firstCell -> minus_time + subsystems[i] -> secondCell -> minus_time;

    fprintf(fp, "%d", simulation_time);
    fprintf(fp, ",%lf", mean_time);
    fprintf(fp, ",%lf", plus_time);
    fprintf(fp, ",%lf", minus_time);
    fprintf(fp, ",%lf", mean_time - minus_time);
    fprintf(fp, "\n");
    
    fclose(fp);
  }
}

void RecordResult::RecordModeTransition(vector<Subsystem*> subsystems)
{
  for(int i = 0 ; i < subsystems.size(); i++)
  {
        
    stringstream ss;
    ss << directory << "/result/mode/sctm_result_mode_" << i << ".csv";
    string filename = ss.str();
        
    if(first_record){
      if((fp = fopen(filename.c_str(), "w")) == NULL)
      {
        cout << "Error: can't Record result mode" << endl;
        exit(EXIT_FAILURE);
      }
      fprintf(fp, "Time,FF,CC,CF,FC1,FC2\n");

    } 
    else 
    {
      if((fp = fopen(filename.c_str(), "a")) == NULL)
      {
        cout << "Error: can't Record result mode" << endl;
        exit(EXIT_FAILURE);
      }
    }
        
    fprintf(fp, "%d", simulation_time);
    fprintf(fp, ",%lf,%lf,%lf,%lf,%lf\n", subsystems[i] -> Pff, subsystems[i] -> Pcc, subsystems[i] -> Pcf, subsystems[i] -> Pfc1, subsystems[i] -> Pfc2);
        
    fclose(fp);
  }
}

void RecordResult::RecordSignalChange(vector<Node*> nodes)
{
  for(int i = 0; i < nodes.size(); i++)
  {
    if(!nodes[i] -> signal) continue;
        
    stringstream ss;
    ss << directory << "/result/signal/sctm_result_signal_" << i << ".csv";
    string filename = ss.str();
        
    if(first_record){
      if((fp = fopen(filename.c_str(), "w")) == NULL)
      {
        cout << "Error: can't Record result signal" << endl;
        exit(EXIT_FAILURE);
      }
      fprintf(fp, "Time,North,East,South,West\n");

    } 
    else
     {
      if((fp = fopen(filename.c_str(), "a")) == NULL)
      {
        cout << "Error: can't Record result signal" << endl;
        exit(EXIT_FAILURE);
      }
    }
        
    fprintf(fp, "%d", simulation_time);
    for(int j = 0; j <  nodes[i] -> adjacent_num; j++)
    {
      int node_number = nodes[i] -> adjacent_nodes[j];

      if(nodes[i] -> signal_phase[node_number] == BLUE)
      {
        fprintf(fp, ",blue");
      } else if(nodes[i] -> signal_phase[node_number] == YELLOW)
      {
        fprintf(fp, ",yellow");
      } else if(nodes[i] -> signal_phase[node_number] == RED)
      {
        fprintf(fp, ",red");
      } else if(nodes[i] -> signal_phase[node_number] == NOSIGNAL)
      {
        fprintf(fp, ",no signal");
      }
    }
    fprintf(fp, "\n");
        
    fclose(fp);
  }
}

void RecordResult::RecordRouteCost(vector<vector<double> > cost)
{
  for(int i = 0; i < cost.size(); i++)
  {
    stringstream ss;
    ss << directory << "/result/cost/sctm_result_cost_" << i << ".csv";
    string filename = ss.str();
        
    if(first_record)
    {
      if((fp = fopen(filename.c_str(), "w")) == NULL)
      {
        cout << "Error: can't Record result cost" << endl;
        exit(EXIT_FAILURE);
      }
            
      fprintf(fp, "Time");
      for(int j = 0; j < cost.size(); j++)
      {
        fprintf(fp, ",Node %d", j);
      }
      fprintf(fp, "\n");

    } 
    else 
    {
      if((fp = fopen(filename.c_str(), "a")) == NULL)
      {
        cout << "Error: can't Record result cost" << endl;
        exit(EXIT_FAILURE);
      }
    }
        
    fprintf(fp, "%d", simulation_time);
    for(int j = 0; j < cost.size(); j++)
    {
      fprintf(fp, ",%lf", cost[i][j]);
    }
    fprintf(fp, "\n");
        
    fclose(fp);
  }
}

void RecordResult::RecordInflowOutflow(vector<Subsystem*> subsystems)
{
  for(int i = 0 ; i < subsystems.size(); i++)
  {
        
    stringstream ss;
    ss << directory << "/result/flow/sctm_result_inflow_outflow_" << i << ".csv";
    string filename = ss.str();
        
    if(first_record)
    {
      if((fp = fopen(filename.c_str(), "w")) == NULL)
      {
        cout << "Error: can't Record result inflow outflow" << endl;
        exit(EXIT_FAILURE);
      }
      fprintf(fp, "Time,inflow,outflow\n");

    } 
    else 
    {
      if((fp = fopen(filename.c_str(), "a")) == NULL)
      {
        cout << "Error: can't Record result inflow outflow" << endl;
        exit(EXIT_FAILURE);
      }
    }
        
    fprintf(fp, "%d", simulation_time);
    fprintf(fp, ",%lf,%lf\n", subsystems[i] -> u(0,0), subsystems[i] -> u(1,0));
        
    fclose(fp);
  }
}


void RecordResult::RecordResultIteration(int time, vector<Vehicle*> vehicles)
{
  simulation_time = time;  // シミュレーション時間を更新
    
  RecordOriginDestTime(vehicles); // 車両の移動距離と時間を記録
  RecordPassedRoute(vehicles);    // 車両の通過経路を記録
  RecordEachMoveTime(vehicles);   // 車両のセル間の移動時間を記録
}

// 全車両の移動時間を記録
void RecordResult::RecordOriginDestTime(vector<Vehicle*> vehicles)
{
  string filename = directory + "/result/sctm_result_move_time.csv";
  if((fp = fopen(filename.c_str(), "w")) == NULL)
  {
    cout << "Error: can't Record result move time" << endl;
    exit(EXIT_FAILURE);
  }
    
  fprintf(fp, "Vehicle,");
  fprintf(fp, "Origin,Destination,Time");
  fprintf(fp, "\n");
    
  for(int i = 0; i < vehicles.size(); i++)
  {
    fprintf(fp, "%d,", i);
    fprintf(fp, "%d,%d,%lf", vehicles[i] -> origin, vehicles[i] -> destination, (double)vehicles[i] -> sum_time / 60);
    fprintf(fp, "\n");
  }
    
  fclose(fp);
}

// 通った経路を記録
void RecordResult::RecordPassedRoute(vector<Vehicle*> vehicles)
{
  string filename = directory + "/result/sctm_result_route.csv";
  if((fp = fopen(filename.c_str(), "w")) == NULL)
  {
    cout << "Error: can't Record result route" << endl;
    exit(EXIT_FAILURE);
  }
    
  fprintf(fp, "Vehicle,");
  for(int i = 1; i <= 10; i++)
  {
    fprintf(fp, "Route %d,", i);
  }
  fprintf(fp, "\n");
    
  for(int i = 0; i < vehicles.size(); i++)
  {
    fprintf(fp, "%d,", i);
    for(int j = 0; j < vehicles[i] -> pass_route.size(); j++)
    {
      fprintf(fp, "%d,", vehicles[i] -> pass_route[j]);
    }
    fprintf(fp, "\n");
  }
    
  fclose(fp);
}

void RecordResult::RecordEachMoveTime(vector<Vehicle*> vehicles)
{
  string filename = directory + "/result/sctm_result_each_move.csv";
  if((fp = fopen(filename.c_str(), "w")) == NULL)
  {
    cout << "Error: can't Record result each move" << endl;
    exit(EXIT_FAILURE);
  }
    
  fprintf(fp, "Vehicle");
  for(int i = 0; i <= 10; i++)
  {
    fprintf(fp, ",Pass %d", i);
  }
  fprintf(fp, "\n");
    
  for(int i = 0; i < vehicles.size(); i++)
  {
    fprintf(fp, "%d", i);
    for(int j = 0; j < vehicles[i] -> transit_link_time.size(); j++)
    {
      fprintf(fp, ",%d", vehicles[i] -> transit_link_time[j]);
    }
    fprintf(fp, "\n");
  }
    
  fclose(fp);
}
