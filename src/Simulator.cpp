#include "Simulator.h"

Simulator::Simulator(string dir)
{
  // 初期化
  directory = dir;
    
  link_num = 0;
  subsystem_num = 0;
  cell_num = 0;
  load_num = 0;
  od_num = 0;
  vehicle_num = 0;
  signal_num = 0;
  node_num = 0;
    
  T = 0;
  total_OD_num = 0;
  total_vehicle_num = 0;
  exit_vehicle_num = 0;
    
  freeflow_speed = 35.0; // km/h
  backwave_speed = 15.0; // km/h
}


// シミュレーション開始
void Simulator::start()
{
  // 実行時間計測用
  clock_t start, end;
  start = clock();
    
  ReadFile();    // ファイル読み込み
  MakeCell();    // セルを作成
  ConnectCell(); // サブシステム同士を接続
        
  // シミュレーション結果を記録
  RecordResult result = RecordResult(directory);
        
  Loop(result); // シミュレーションループ
    
  end = clock();
  cout << "Run Time: " << (end - start) / CLOCKS_PER_SEC << " second" << endl;
  cout << endl;
  cout << "Recording..." << endl;
    
  result.RecordResultIteration(T, vehicles);  // シミュレーション結果を記録
}


// ファイル読み込み
void Simulator::ReadFile()
{
    
  string filename;
  // リンクファイルの読み込み
  vector<int> linkVector(5);
    
  filename = directory + "/network.csv";
  if((fp = fopen(filename.c_str(), "r")) == NULL)
  {
    cout << "Error: can't open network file." << endl;
    exit(EXIT_FAILURE);
  }
    
  while(fscanf(fp, "%d,%d,%d,%d,%d", &linkVector[0], &linkVector[1], &linkVector[2], &linkVector[3], &linkVector[4]) != EOF)
  {
    if(node_num < linkVector[1])
    {
      node_num = linkVector[1];
    }
    link.push_back(linkVector);
    link_num++;
  }
  fclose(fp);
  node_num++;
    
    
  // 車両ファイルの読み込み
  vector<int> odVector(4);
    
  filename = directory + "/vehicle.csv";
  if((fp = fopen(filename.c_str(), "r")) == NULL)
  {
    cout << "Error: can't open vehicle file." << endl;
    exit(EXIT_FAILURE);
  }
    
  while(fscanf(fp, "%d,%d,%d,%d", &odVector[0], &odVector[1], &odVector[2], &odVector[3]) != EOF)
  {
    OD* origin_dest = new OD(od_num, odVector[0], odVector[1], odVector[2], odVector[3]);
    ods.push_back(origin_dest);
    od_num++;
    total_OD_num += odVector[2];

    if(origin_node_number.size() == 0){
      origin_node_number.push_back(odVector[1]);
      continue;
    }

    bool flag = false;
    for(int i = 0; i < origin_node_number.size(); i++)
    {
      if(origin_node_number[i] == odVector[1])
      {
        flag = true;
        break;
      }
    }

    if(!flag) origin_node_number.push_back(odVector[1]);
  }
  fclose(fp);
    
    
  for(int i = 0; i < node_num; i++)
  {
    Node* signal_node = new Node(signal_num);
    nodes.push_back(signal_node);
    signal_num++;
  }
    
  // 信号ファイルの読み込み
  vector<int> signalVector(6);
    
  filename = directory + "/signal.csv";
  if((fp = fopen(filename.c_str(), "r")) == NULL)
  {
    cout << "Error: can't open signal file." << endl;
    exit(EXIT_FAILURE);
  }
    
  int j = 0;
  vector<int> adjacents;
  map<int, vector<int> > signalMap;
    
  while(fscanf(fp, "%d,%d,%d,%d,%d,%d", &signalVector[0], &signalVector[1], &signalVector[2], &signalVector[3], &signalVector[4], &signalVector[5]) != EOF){
    vector<int> times;
    adjacents.push_back(signalVector[1]);
    times.push_back(signalVector[2]);
    times.push_back(signalVector[3]);
    times.push_back(signalVector[4]);
    times.push_back(signalVector[5]);
    signalMap.insert(make_pair(signalVector[1], times));
        
    if(j == 3)
    {
      nodes[signalVector[0]] -> setSignal(signalMap, adjacents);
      signal_num++;
            
      j = 0;
      adjacents.clear();
      signalMap.clear();
      continue;
    }
    j++;
  }
  fclose(fp);
    
  linkVector.clear();
  odVector.clear();
  signalVector.clear();
}


// セルとサブシステムと積み込みセルの作成
void Simulator::MakeCell()
{
    
  for(int i = 0; i < link_num; i++)
  {
    
    // セルを2単位で分割
    for(int j = 0; j < 2; j++){
      
      Cell* cell = new Cell(cell_num, subsystem_num); // セルの作成
      cells.push_back(cell);
      
      if(j == 0){ // 上流セルの設定
        cells[cell_num]->initCell((double)link[i][2] / 2, (double)link[i][3] / 2, link[i][4]);
        
        Subsystem* sub = new Subsystem(subsystem_num, nodes[link[i][0]], nodes[link[i][1]]); // サブシステムの作成
        subsystems.push_back(sub);
        subsystems[subsystem_num] -> firstCell = cells[cell_num];
        
        LoadCell* loadCell = new LoadCell(subsystems[subsystem_num]); // 積み込みセルの作成
        loads.push_back(loadCell);
        load_num++;
        
        cells[cell_num] -> is_upper_stream = true;
        cells[cell_num] -> merge_num = 0;
        cells[cell_num] -> diverge_num = 1;
      }
      
      if(j == 1){ // 下流セルの設定
        cells[cell_num] -> initCell((double)link[i][2] / 2, (double)link[i][3] / 2, link[i][4]);
        
        subsystems[subsystem_num] -> secondCell = cells[cell_num];
        
        cells[cell_num] -> is_upper_stream = false;
        cells[cell_num] -> merge_num = 1;
        cells[cell_num] -> diverge_num = 0;
      }
      
      cells[cell_num] -> SetParameter(freeflow_speed, backwave_speed);
      cell_num++;
    }
    subsystems[subsystem_num] -> initMode();
    subsystem_num++;
        
    // セルの結合
    for (int j = 2; j > 1; j--)
    {
      cells[cell_num - j] -> PushNextCell(cells[cell_num - j + 1]);
      cells[cell_num - j + 1] -> PushBeforeCell(cells[cell_num - j]);
    }
        
    // ノードの更新
    subsystems[i] -> destNode -> UpdateNode(subsystems[i] -> originNode -> number);
  }
}


// セルとサブシステムを接続する
void Simulator::ConnectCell()
{
    
  for(int i = 0; i < subsystem_num; i++)
  {
        
    bool diverge = false;
        
    for(int j = 0; j < subsystem_num; j++)
    {
      int upper_origin = subsystems[i] -> originNode -> number;
      int upper_dest = subsystems[i] -> destNode -> number;
      int lower_origin = subsystems[j] -> originNode -> number;
      int lower_dest = subsystems[j] -> destNode -> number;
            
      // 分流先のリンク
      if((upper_dest == lower_origin) && (upper_origin != lower_dest) && (i != j))
      {
                
        Cell* originCell = subsystems[i] -> secondCell;
        Cell* destCell = subsystems[j] -> firstCell;
                
        // セルの接続
        originCell -> PushNextCell(destCell);
        destCell -> PushBeforeCell(originCell);
        originCell -> diverge_num++;
        destCell -> merge_num++;
                
        // サブシステムの接続
        subsystems[i] -> PushNextSub(subsystems[j]);
        subsystems[j] -> PushBeforeSub(subsystems[i]);
        subsystems[j] -> inflow.insert(make_pair(i, 0));
        subsystems[i] -> outflow.insert(make_pair(j, 0));
        subsystems[i] -> outflow_surplus.insert(make_pair(j, 0));
        diverge = true;
      }
    }
  }
}

void Simulator::UpdateAllVehiclesRoutes()
{

    cout << " \t\t\t\t  UPDATING VEHICLES? " << vehicle_num << "\n";

  if ( T == 500 )
  {
    cout << " #########################\n";
    cout << "      YES \n";
    cout << " #########################\n";
    for (int k = 0 ; k < vehicle_num ; k++)
      vehicles[k] -> route = route;
  }
  
}


// 予測経路を更新
void Simulator::GetRoute()
{
    
  vector<vector<double> > route_cost(node_num, vector<double>(node_num, INF)); // 経路コスト
  vector<vector<double> > link_cost(node_num, vector<double>(node_num, INF)); // リンクコスト

  // 所要時間とコストを初期化
  route.resize(node_num);
  for(int i = 0; i < node_num; i++)
  {
    route[i].resize(node_num);
  }

  route_choice_probability.resize(node_num);
  for(int i = 0; i < node_num; i++)
  {
    route_choice_probability[i].resize(node_num);
    for(int j = 0; j < node_num; j++)
    {
      route_choice_probability[i][j].assign(node_num, 0.0);
    }
  }


  // セルの通過時間を更新
  for(int i = 0; i < cell_num; i++)
  {
    cells[i] -> UpdateWaitTime();
  }
        
  for(int i = 0; i < subsystem_num; i++)
  {
    int origin_num = subsystems[i] -> originNode -> number;
    int dest_num = subsystems[i] -> destNode -> number;
        
    link_cost[origin_num][dest_num] = subsystems[i] -> firstCell -> predict_time + subsystems[i] -> secondCell -> predict_time;
    
    if(subsystems[i] -> firstCell -> is_dummy)
    { // ダミーリンクに対する処理
      link_cost[origin_num][dest_num] = DUMMY;
    }
  }

  // 最適経路を計算
  Dijkstra(route_cost, link_cost);

  // DIAL法により経路選択確率を決定
  if(DIAL){
    Dial(route_cost, link_cost);
  }

  route_cost.clear();
  link_cost.clear();
}


// ダイクストラ法
void Simulator::Dijkstra(vector<vector<double> >& route_cost, vector<vector<double> >& link_cost)
{
    
  for(int i = 0; i < node_num; i++)
  {
        
    vector<bool> visited(node_num, false); // ノードのコストを決定したかどうか
        
    route_cost[i][i] = 0;
        
    while(true)
    {
      double min = INF;
      for(int x = 0; x < node_num; x++)
      {
        if(!visited[x] && min > route_cost[x][i])
        {
          min = route_cost[x][i];
        }
      }
      if(min == INF)
      {
        route[i][i] = -1; // 自身への経路は-1に設定
        break;
      }
      for(int y = 0; y < node_num; y++)
      {
        if(route_cost[y][i] == min)
        {
          visited[y] = true;
                    
          for(int x = 0; x < node_num; x++)
          {
            if(route_cost[x][i] > link_cost[x][y] + route_cost[y][i])
            {
              route_cost[x][i] = link_cost[x][y] + route_cost[y][i];
              route[x][i] = y;
            }
          }
        }
      }
    }
    visited.clear();
  }
}


void Simulator::Dial(vector<vector<double> >& route_cost, vector<vector<double> >& link_cost)
{
  vector<vector<double> > likelihood(node_num, vector<double>(node_num, 0.0));
  vector<vector<double> > weight(node_num, vector<double>(node_num, 0.0));
  vector<vector<double> > traffic_flow(node_num, vector<double>(node_num, 0.0));
  vector<vector<double> > OD_flow(node_num, vector<double>(node_num, 0.0));

  // コストを昇順にソートしたマップを作成
  vector<map<int, double> > ascending_sorted_cost(node_num);
  vector<map<int, double> > descending_sorted_cost(node_num);
  for(int i = 0; i < node_num; i++){
    vector<double> ascending_cost_vector = route_cost[i];
    vector<double> descending_cost_vector = route_cost[i];    
    sort(ascending_cost_vector.begin(), ascending_cost_vector.end());
    sort(descending_cost_vector.begin(), descending_cost_vector.end(), greater<int>());
    for(int j = 0; j < node_num; j++){
      ascending_sorted_cost[i].insert(make_pair(j, ascending_cost_vector[j]));
      descending_sorted_cost[i].insert(make_pair(j, descending_cost_vector[j]));
    }
    ascending_cost_vector.clear();
    descending_cost_vector.clear();
  }
  
  for(int r = 0; r < origin_node_number.size(); r++)
  {  
    int origin_number = origin_node_number[r];

    // リンク尤度を計算
    for(int i = 0; i < node_num; i++)
    {
      for(int j = 0; j < node_num; j++)
      {      
        if(link_cost[i][j] != INF && route_cost[origin_number][i] < INF && route_cost[origin_number][j] < INF)
        {
           if(route_cost[origin_number][i] < route_cost[origin_number][j])
           {
             likelihood[i][j] = exp(THETA * (route_cost[origin_number][j] - route_cost[origin_number][i] - link_cost[i][j]));
           }
        }
      }
    }

    // 前進処理
    for(map<int, double>::iterator it = ascending_sorted_cost[origin_number].begin(); it != ascending_sorted_cost[origin_number].end(); it++){
      int i = it -> first;
      for(int j = 0; j < node_num; j++)
      {
        if(i == origin_number)
        {
          weight[i][j] = likelihood[i][j];
        } 
        else 
        {
          double weight_sum = 0.0;

          for(int m = 0; m < node_num; m++)
          {
            if(link_cost[m][i] != INF)
            {
              weight_sum += likelihood[m][i];
            }
          }
          weight[i][j] = likelihood[i][j] * weight_sum;
        }
      }
    }

    // 経路選択確率を計算
    vector<double> weight_sum(node_num, 0.0);
    for(int i = 0; i < node_num; i++)
    {
      for(int j = 0; j < node_num; j++)
      {
        weight_sum[i] += weight[i][j];
      }
    }

    for(int i = 0; i < node_num; i++)
    {
      for(int j = 0; j < node_num; j++)
      {
        if(weight_sum[i] > 0.0)
        {
          route_choice_probability[origin_number][i][j] = weight[i][j] / weight_sum[i];
        } 
        else 
        {
          route_choice_probability[origin_number][i][j] = 0.0;
        }
      }
    }


    string filename;
    stringstream ss1;
    ss1 << directory << "/result/likelihood/result_likelihood_" << origin_number << ".csv";
    filename = ss1.str();
  
    if((fp = fopen(filename.c_str(), "w")) == NULL)
    {
      cout << "Error: can't record result likelihood" << endl;
      exit(EXIT_FAILURE);
    }
  
    fprintf(fp, "likelihood");
    for(int i = 0; i < node_num; i++)
    {
      fprintf(fp, ",%d", i);
    }
    fprintf(fp, "\n");
    
    for(int i = 0; i < node_num; i++)
    {
      fprintf(fp, "%d", i);
      for(int j = 0; j < node_num; j++)
      {
        if(likelihood[i][j] == INF)
        {
          fprintf(fp, ",@");
        } 
        else 
        {
          fprintf(fp, ",%lf", likelihood[i][j]);
        }
      }
      fprintf(fp, "\n");
    }
    fclose(fp);


    stringstream ss2;
    ss2 << directory << "/result/choice_probability/result_route_probability_" << origin_number << ".csv";
    filename = ss2.str();
  
    if((fp = fopen(filename.c_str(), "w")) == NULL)
    {
      cout << "Error: can't record result choice probability" << endl;
      exit(EXIT_FAILURE);
    }
  
    fprintf(fp, "route probability");
    for(int i = 0; i < node_num; i++)
    {
      fprintf(fp, ",%d", i);
    }
    fprintf(fp, "\n");
    
    for(int i = 0; i < node_num; i++)
    {
      fprintf(fp, "%d", i);
      for(int j = 0; j < node_num; j++)
      {
        if(route_choice_probability[origin_number][i][j] == INF)
        {
          fprintf(fp, ",@");
        } else {
          fprintf(fp, ",%lf", route_choice_probability[origin_number][i][j]);
        }
      }
      fprintf(fp, "\n");
    }
    fclose(fp);
  }

  likelihood.clear();
  weight.clear();
  traffic_flow.clear();
  OD_flow.clear();
  ascending_sorted_cost.clear();
  descending_sorted_cost.clear();
}


//  シミュレーションループ
void Simulator::Loop(RecordResult result)
{
    
  while(true)
  {

    // 1ステップに1回車両を搭載する
    if(T % (10 * 60 / LOAD_TIMES) == 0)
    {
      // 次に選択する経路を選択
      GetRoute();

         UpdateAllVehiclesRoutes();

      // 車両の搭載
      LoadVehicle();
    }
        
    // 積み込みセルに対する処理
    for(int i = 0; i < loads.size(); i++)
    {
      if(loads[i] -> inflow_subsystem -> originNode -> starting)
      {
        MoveVehiclesLoad(loads[i]);
      }
    }
          
    // 車両移動
    for(int i = 0; i < subsystem_num; i++){
            
      // セルに車両がいる場合
      if(subsystems[i] -> firstCell -> n > 0 || subsystems[i] -> secondCell -> n > 0)
      {
                
        // 単路、合流部であった場合
        if(subsystems[i] -> secondCell -> diverge_num == 0 || subsystems[i] -> secondCell -> diverge_num == 1)
        {
                    
          // 単路、合流部に応じた車両の移動
          MoveVehiclesStraightAndMerge(subsystems[i]);
                    
          // 分流部であった場合
        } else if(subsystems[i] -> secondCell -> diverge_num > 1){
                    
          // 合流部に応じた車両の移動
          MoveVehiclesDiverge(subsystems[i]);
        }
      }
    }
        
    // 各セルの密度の更新と各車両の滞在時間の更新
    for(int i = 0; i < cell_num; i++){
            
      // 各セルの密度の更新
      cells[i] -> UpdateDensity();
            
      // 各車両の滞在時間の更新
      if(cells[i] -> n > 0){
                
        for(int j = 0; j < cells[i] -> load_vehicles.size(); j++)
        {
                    
          if(!cells[i] -> load_vehicles[j] -> move)
          {
            cells[i] -> load_vehicles[j] -> move = true;
          } 
          else 
          {
            cells[i] -> load_vehicles[j] -> stay_time += STEP;
            cells[i] -> load_vehicles[j] -> link_time += STEP;
          }
                    
          cells[i] -> load_vehicles[j] -> sum_time += STEP;
        }
      }
    }

    // サブシステムの密度の分散、モードの生起確率を算出
    for(int i = 0; i < subsystem_num; i++)
    {
      // サブシステムの密度の分散を算出
      subsystems[i] -> CalculateDensity();

      // サブシステムのモードの生起確率を更新
      subsystems[i] -> CalculateModeProbability();
    }

    // 各積み込みセルの密度の更新と各車両の滞在時間の更新
    for(int i = 0; i < load_num; i++)
    {
      LoadCell* load = loads[i];                             
      
      // 各車両の滞在時間の更新
      if(!load -> load_vehicles.empty())
      {
        for(int j = 0; j < load -> load_vehicles.size(); j++)
        {
          loads[i] -> load_vehicles[j] -> stay_time += STEP;
          loads[i] -> load_vehicles[j] -> link_time += STEP;
          loads[i] -> load_vehicles[j] -> sum_time += STEP;
        }
      }
    }
        
        
    // 合流させる車両を調整する
    for(int i = 0; i < subsystem_num; i++)
    {
      // サブシステム間の車両の移動台数を計算
      subsystems[i] -> SetAdjacentFlow();
    }

    for(int i = 0; i < subsystem_num; i++)
    {
      // サブシステム内部の車両移動数を決定
      subsystems[i] -> SetInsideFlow();
    }

    // 合流させる車両を調整する 
    for(int i = 0; i < subsystem_num; i++){
      if(subsystems[i] -> secondCell -> diverge_num >= 1)
      {
        subsystems[i] -> AdjustDivergeFlow();
      }
    }

    // 合流させる車両を調整する 
    for(int i = 0; i < subsystem_num; i++)
    {
      if(subsystems[i] -> firstCell -> merge_num >= 1)
      {
        subsystems[i] -> AdjustMergeFlow();
      }
    }
        
    cout << "Time: " << T << "  Exit: " << exit_vehicle_num << endl;
    cout << "Total OD: " << total_OD_num << "  Total Vehicle: " << total_vehicle_num << endl;
    cout << endl;
        
    // シミュレーション時間の更新
    T += STEP;
        
    // 信号時間の更新
    for(int i = 0; i < node_num; i++)
    {
      if(nodes[i] -> signal)
        nodes[i] -> UpdateTime();
    }
        
    // ステップごとにシミュレーションの結果を記録
    result.RecordResultStep(T, subsystems, nodes);
        
    // 終了条件（全ての車両が目的地に着いた場合）
    double rest_vehicle_num = total_OD_num - exit_vehicle_num;
    rest_vehicle_num = pow(rest_vehicle_num, 2);
    if(rest_vehicle_num <= 0.0000001 || T > MAX_T)
      break;
    
  }
}


// 積み込みセルに車両の搭載
void Simulator::LoadVehicle()
{
    
  // 出発時刻を順に確認
  for(int i = 0; i < od_num; i++)
  {
        
    if(ods[i] -> counter == LOAD_TIMES) continue;
        
    if(ods[i] -> depart_time == (T / 600) * 10)
    {
            
      // 移動先が目的地の場合
      if(ods[i] -> origin == ods[i] -> destination)
      {
                
        // 車両を搭載
        Vehicle* vehicle = new Vehicle(vehicle_num);
        vehicles.push_back(vehicle);
        vehicles[vehicle_num] -> origin = ods[i] -> origin;
        vehicles[vehicle_num] -> destination = ods[i] -> destination;
        vehicles[vehicle_num] -> loads = ods[i] -> load_vehicle[ods[i] -> counter];
        vehicles[vehicle_num] -> arrival = true;
                
        total_vehicle_num += vehicles[vehicle_num] -> loads;
        vehicle_num++;
        exit_vehicle_num += ods[i] -> load_vehicle[ods[i] -> counter];
        ods[i] -> counter++;
        continue;
      }
            
      // リンクを順に確認
      for(int j = 0; j < subsystem_num; j++)
      {
                
        // 最短距離を指定
        if(subsystems[j] -> originNode -> number == ods[i] -> origin && subsystems[j] -> destNode -> number == route[ods[i] -> origin][ods[i] -> destination]){
                    
          // 車両を搭載
          Vehicle* vehicle = new Vehicle(vehicle_num);
          vehicles.push_back(vehicle);
          vehicles[vehicle_num] -> origin = ods[i] -> origin;
          vehicles[vehicle_num] -> destination = ods[i] -> destination;
          vehicles[vehicle_num] -> loads = ods[i] -> load_vehicle[ods[i] -> counter];
          vehicles[vehicle_num] -> PushPass(ods[i] -> origin);
          vehicles[vehicle_num] -> route = route;
          vehicles[vehicle_num] -> choice_probability = route_choice_probability[ods[i] -> origin];
          
          loads[j] -> PushVehicle(vehicles[vehicle_num]);
          loads[j] -> n += vehicles[vehicle_num] -> loads;

          nodes[vehicles[vehicle_num] -> origin] -> starting = true;
          nodes[vehicles[vehicle_num] -> destination] -> ending = true;

          total_vehicle_num += vehicles[vehicle_num] -> loads;
          vehicle_num++;
          ods[i] -> counter++;
          break;
        }
      }
    }
  }
}


// 単路、合流部での車両を移動させる
void Simulator::MoveVehiclesStraightAndMerge(Subsystem* subsystem)
{
    
  Cell* firstCell = subsystem -> firstCell;  // サブシステムの上流セル
  Cell* secondCell = subsystem -> secondCell; // サブシステムの下流セル
    
  // 車両が目的地に到着しているかを確認
  double arrival_flow = subsystem -> arrive_flow + subsystem -> arrive_flow_surplus;
  subsystem -> arrive_flow_surplus = subsystem -> arrive_flow;
  bool arrive_flag = false;
  for(vector<Vehicle*>::iterator it = secondCell -> load_vehicles.begin(); it != secondCell -> load_vehicles.end(); ){
    Vehicle* vehicle = *it;
    int next_node = vehicle -> route[subsystem -> destNode -> number][vehicle -> destination];

    // 目的地に到達していた場合
    if(next_node == -1)
    {
      
      // ダミーリンクは全ての車両をゴールさせる
      if(arrival_flow >= vehicle -> loads)
      {
        vehicle -> PushPass(subsystem -> destNode -> number);
        vehicle -> PushTransitTime();
        vehicle -> arrival = true;
            
        // 車両数の更新
        secondCell -> n -= vehicle -> loads;
        arrival_flow -= vehicle -> loads;
        exit_vehicle_num += vehicle -> loads;
        subsystem -> move_dest_num += vehicle ->loads;
        
        // 車両の削除
        it = secondCell -> load_vehicles.erase(it);
        arrive_flag = true;
        
      }
       else 
      {
        // 移動できなかった台数は次のステップに持ち越し
        subsystem -> arrive_flow_surplus = arrival_flow;
        break;
      }
      
    }
     else 
    {
      it++;
    }
  }
  // 1台でも到着していた場合は余りを初期化
  if(arrive_flag || secondCell -> load_vehicles.empty())
  {
    subsystem -> arrive_flow_surplus = 0;
  }

  // 上流セルの流出車両の移動処理
  if(!firstCell -> load_vehicles.empty())
  {
        
    // 流出台数 = 求めた台数 + 前のステップの余り
    double first_flow = subsystem -> inside_outflow + subsystem -> inside_outflow_surplus;
    bool init_flag = false;

    // 車両の移動
    for(vector<Vehicle*>::iterator it = firstCell -> load_vehicles.begin(); it != firstCell -> load_vehicles.end(); )
    {
      Vehicle* vehicle = *it;

      // 移動した直後の車両がいたらそれ以上移動しない
      if(!vehicle -> move)
      {
        init_flag = true;
        break;
      }

      // 求めた流量分だけ移動
      if(first_flow >= vehicle -> loads)
      {
        vehicle -> stay_time = 0;
        vehicle -> move = false;
        vehicle -> travel_distance += firstCell -> L;
                
        // 車両数の更新
        secondCell -> n += vehicle -> loads;
        secondCell -> flow_num += vehicle -> loads;
        firstCell -> n -= vehicle -> loads;
        first_flow -= vehicle -> loads;
                
        // 車両の移し替え
        secondCell -> PushVehicle(vehicle);
        it = firstCell -> load_vehicles.erase(it);
        init_flag = true;
        
      } 
      else 
      {
        // 移動できなかった車両は次のステップに持ち越し
        subsystem -> inside_outflow_surplus = first_flow;
        break;
      }
    }

    // 車両が1台でも移動した場合は余りを初期化
    if(init_flag)
      subsystem -> inside_outflow_surplus = 0;

  }
    
    
  // 下流セルの流出車両の移動処理
  if(!secondCell -> load_vehicles.empty() && subsystem -> next.size() > 0)
  {
    Cell* next_firstCell = subsystem -> next[0] -> firstCell; // 移動先のサブシステムの上流セル
    int next_num = subsystem -> next[0] -> number;
            
    // 移動する順番でない場合（信号が赤だった場合）
    if(subsystem -> destNode -> signal && subsystem -> destNode -> signal_phase[subsystem -> originNode -> number] == RED)
    {
      return;
    }
                       
    // 流出台数 = 求めた台数 + 前のステップの余り
    double second_flow = subsystem -> outflow[next_num] + subsystem -> outflow_surplus[next_num];
    bool init_flag = false;
            
    // 車両の移動
    for(vector<Vehicle*>::iterator it = secondCell -> load_vehicles.begin(); it != secondCell -> load_vehicles.end(); )
    {
      Vehicle* vehicle = *it;
      int next_node = vehicle -> route[subsystem -> destNode -> number][vehicle -> destination];

      // 移動した直後の車両または到着できなかった車両がいたらそれ以上移動しない
      if(!vehicle -> move || next_node == -1)
      {
        init_flag = false;
        break;
      }

      // 求めた流量分の車両を移動
      if(second_flow >= vehicle -> loads)
      {
        vehicle -> stay_time = 0;
        vehicle -> move = false;
        vehicle -> travel_distance += secondCell -> L;
                        
        // 車両数の更新
        next_firstCell -> n += vehicle -> loads;
        next_firstCell -> flow_num += vehicle -> loads;
        secondCell -> n -= vehicle -> loads;
        second_flow -= vehicle -> loads;
        subsystem -> move_outflow_num += vehicle -> loads;
        subsystems[next_num] -> move_inflow_num += vehicle -> loads;
                        
        // 車両の移し替え
        next_firstCell -> PushVehicle(vehicle);
        it = secondCell -> load_vehicles.erase(it);
        init_flag = true;
                        
      } 
      else
      {
        // 移動できなかった車両は次のステップに持ち越し
        subsystem -> outflow_surplus[subsystem -> next.front() -> number] = second_flow;
        break;
      }
    }

    // 車両が1台でも移動した場合は余りを初期化
    if(init_flag)
      subsystem -> outflow_surplus[subsystem -> next.front() -> number] = 0;

  }
}


// セルの流入台数を分岐先ごとに求めて車両を移動させる
void Simulator::MoveVehiclesDiverge(Subsystem* subsystem)
{
  Cell* firstCell = subsystem -> firstCell;  // サブシステムの上流セル
  Cell* secondCell = subsystem -> secondCell; // サブシステムの下流セル
    
  // 車両が目的地に到達しているかどうかの確認
  double arrival_flow = subsystem -> arrive_flow + subsystem -> arrive_flow_surplus;
  subsystem -> arrive_flow_surplus = subsystem -> arrive_flow;
  bool arrive_flag = false;
  for(vector<Vehicle*>::iterator it = secondCell -> load_vehicles.begin(); it != secondCell -> load_vehicles.end(); ){
    Vehicle* vehicle = *it;
    int next_node = vehicle -> route[subsystem -> destNode -> number][vehicle -> destination];

    // 目的地に到達していた場合
    if(next_node == -1){

      // ダミーリンクは全ての車両をゴールさせる
      if(arrival_flow >= vehicle -> loads)
      {
        vehicle -> PushPass(subsystem -> destNode -> number);
        vehicle -> PushTransitTime();
        vehicle -> arrival = true;
            
        // 車両数の更新
        secondCell -> n -= vehicle -> loads;
        arrival_flow -= vehicle -> loads;
        exit_vehicle_num += vehicle -> loads;
        subsystem -> move_dest_num += vehicle ->loads;
            
        // 車両の差し替え
        it = secondCell -> load_vehicles.erase(it);
        arrive_flag = true;
            
      }
       else 
      {
        // 移動できなかった車両は次のステップに持ち越し
        subsystem -> arrive_flow_surplus = arrival_flow;
        break;
      }

    }
     else 
    {
      it++;
    }
  }
  // 1台でも車両が移動した場合は余りを初期化
  if(arrive_flag || secondCell -> load_vehicles.empty())
  {
    subsystem -> arrive_flow_surplus = 0;
  }

  // 上流セルの流出車両の移動処理
  if(!firstCell -> load_vehicles.empty())
  {

    // 流出台数 = 求めた台数 + 前のステップの余り
    double first_flow = subsystem -> inside_outflow + subsystem -> inside_outflow_surplus;
    bool init_flag = false;
        
    // 車両の移動
    for(vector<Vehicle*>::iterator it = firstCell -> load_vehicles.begin(); it != firstCell -> load_vehicles.end(); )
    {
      Vehicle* vehicle = *it;
            
      // 移動した直後の車両がいたらそれ以上移動しない
      if(!vehicle -> move)
      {
        init_flag = true;
        break;
      }

      // 求めた流量分の車両を移動
      if(first_flow >= vehicle -> loads)
      {
        vehicle -> stay_time = 0;
        vehicle -> move = false;
        vehicle -> travel_distance += firstCell -> L;
                
        // 車両数の更新
        secondCell -> n += vehicle -> loads;
        secondCell -> flow_num += vehicle -> loads;
        firstCell -> n -= vehicle -> loads;
        first_flow -= vehicle -> loads;
                
        // 車両の移し替え
        secondCell -> PushVehicle(vehicle);
        it = firstCell -> load_vehicles.erase(it);
        init_flag = true;
        
      }
       else 
      {
        // 移動できなかった車両は次のステップに持ち越し
        subsystem -> inside_outflow_surplus = first_flow;
        break;
      }
    }

    // 車両が1台でも移動した場合は余りを初期化
    if(init_flag)
      subsystem -> inside_outflow_surplus = 0;
    
  }
    
  // サブシステムの下流セルの流出車両の移動処理
  if(!secondCell -> load_vehicles.empty() && subsystem -> next.size() > 0)
  {
        
    // 移動する順番でない場合（信号が赤だった場合）
    if(subsystem -> destNode -> signal && subsystem -> destNode -> signal_phase[subsystem -> originNode -> number] == RED)
    {
      return;
    }
        
    map<int, double> second_flow = subsystem -> outflow;
    map<int, bool> init_flag;
        
    // 前のステップの余りを足す
    for(map<int, double>::iterator it = subsystem -> outflow.begin(); it != subsystem -> outflow.end(); it++)
    { 
      second_flow[it -> first] += subsystem -> outflow_surplus[it -> first];
      init_flag.insert(make_pair(it -> first, false));
    }
    
    // 車両を目的地の方向へ流す
    for(vector<Vehicle*>::iterator v_it = secondCell -> load_vehicles.begin(); v_it != secondCell -> load_vehicles.end(); )
    {
      Vehicle* vehicle = *v_it;
      double probability = 0.0;
      double random = ((double)rand()) / RAND_MAX;
      bool move_flag = false;
      bool dial_flag = false;
      
      for(map<int, double>::iterator it = second_flow.begin(); it != second_flow.end(); it++)
      {
        int next_num = it -> first;
        Cell* next_firstCell = subsystems[next_num] -> firstCell;
        double outflow_num = it -> second;
        int next_node = vehicle -> route[subsystem -> destNode -> number][vehicle -> destination];
        
        // DIAL法を使用する場合
        if(DIAL && !vehicle -> use_dial)
        {
          probability += vehicle -> choice_probability[subsystem -> destNode -> number][subsystems[next_num] -> destNode -> number];

          // cout << "vehicle : " << vehicle -> number << " choice : " << probability << " random : " << random << endl;
          if(probability <= random)
          {
            continue;
          }
           else 
          {
            dial_flag = true;
          }

        }
         else
         {
          // 車両のもつ経路で進む
          if(subsystems[next_num] -> destNode -> number != next_node)
          {
            continue;
          }
        }

        //目的地に到達できない車両がいた場合は移動しない
        if(!vehicle -> move || next_node == -1){
          ;
                        
          // 求めた流量だけ車両を移動させる
        } else if(outflow_num >= vehicle -> loads)
        {
          vehicle -> stay_time = 0;
          vehicle -> move = false;
          vehicle -> travel_distance += secondCell -> L;
            
          // 車両数の更新
          next_firstCell -> n += vehicle -> loads;
          next_firstCell -> flow_num += vehicle -> loads;
          secondCell -> n -= vehicle -> loads;
          outflow_num -= vehicle -> loads;
          subsystem -> move_outflow_num += vehicle -> loads;
          subsystems[next_num] -> move_inflow_num += vehicle -> loads;
            
          // 車両の移し替え
          next_firstCell -> PushVehicle(vehicle);
          v_it = secondCell -> load_vehicles.erase(v_it);
            
          // 移動できなかった車両は次のステップに持ち越し
          second_flow[next_num] = outflow_num;
          init_flag[next_num] = true;
          move_flag = true;

          if(dial_flag)
            vehicle -> use_dial = true;
          

        }
         else 
        {
          subsystem -> outflow_surplus[next_num] = outflow_num;
        }
        break;
      }

      if(!move_flag) 
      	v_it++;
    }

    // 車両が1台でも移動した場合は余りを初期化
    for(map<int, double>::iterator it = subsystem -> outflow.begin(); it != subsystem -> outflow.end(); it++)
    {
      if(init_flag[it -> first])
        subsystem -> outflow_surplus[it -> first] = 0;
      
    }
    
    second_flow.clear();
    init_flag.clear();
  }
}


// 積み込みセルから車両をセルへ流す
void Simulator::MoveVehiclesLoad(LoadCell* loadCell)
{
    
  if(loadCell -> load_vehicles.empty()) return;  // セルが空の場合は車両の移動なし
    
  Cell* inflowCell = loadCell -> inflow_subsystem -> firstCell;
  int nextNum = loadCell -> inflow_subsystem -> number;
  int sum1 = 0;
  int sum2 = 0;
  int sum3 = 0;
  int sum4 = 0;
  double unit = STEP / 3600.0;
  double send = loadCell -> n / unit;  // セルの中に存在する車両台数n

  if(send > inflowCell -> Qm)
  {
    send = inflowCell -> Qm;
  }

  double Pf = subsystems[nextNum] -> Pff + subsystems[nextNum] -> Pfc;
  double Pc = subsystems[nextNum] -> Pcc + subsystems[nextNum] -> Pcf;
    
  // 移動先のサブシステムの状況にあわせて移動する車両台数を決定する
  for(int i = 0; i < NOISE_NUM; i++)
  {
    if(Pf * NOISE_NUM > i)
    {
      if(send < subsystems[nextNum] -> Qm[0][i])
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
      if(send < subsystems[nextNum] -> Wc[0][i] * (subsystems[nextNum] -> rhoJ[0][i] - inflowCell -> density))
      {
        sum3++;
      }
    	else
       {
        sum4++;
      }
    }
  }
    
  // 4つのイベントの生起確率を計算
  double P1 = (double)sum1 / NOISE_NUM;
  double P2 = (double)sum2 / NOISE_NUM;
  double P3 = (double)sum3 / NOISE_NUM;
  double P4 = (double)sum4 / NOISE_NUM;
    
  // 各イベントの車両の移動台数
  double flow1 = send;
  double flow2 = inflowCell -> Qm;
  double flow3 = send;
  double flow4 = inflowCell -> Wc * (inflowCell -> rhoJ - inflowCell -> density);

  double out_flow = unit * (P1 * flow1 + P2 * flow2 + P3 * flow3 + P4 * flow4);
  double flow = out_flow + loadCell -> load_surplus; // 実際に動かす車両台数
  bool init_flag = false;

  // 車両の移動
  for(vector<Vehicle*>::iterator it = loadCell -> load_vehicles.begin(); it != loadCell -> load_vehicles.end(); )
  {
    Vehicle* vehicle = *it;
        
    if(flow >= vehicle -> loads)
    {
      // セルとサブシステム状態更
      vehicle -> stay_time = 0;
      vehicle -> move = false;
            
      // 車両数の更新
      inflowCell -> n += vehicle -> loads;
      inflowCell -> flow_num += vehicle -> loads;
      loadCell -> n -= vehicle -> loads;
      flow -= vehicle -> loads;
      subsystems[inflowCell -> subsystem_num] -> move_origin_num += vehicle -> loads;

      // 車両の移し替え
      inflowCell -> PushVehicle(vehicle);
      it = loadCell -> load_vehicles.erase(it);
      init_flag = true;
            
    } else {
      // 移動できなかった車両は次のステップに持ち越し
      loadCell -> load_surplus = flow;
      break;
    }
  }

  // 車両が1台でも移動した場合は余りを初期化
  if(init_flag){ 
    loadCell -> load_surplus = 0;
  }
}
