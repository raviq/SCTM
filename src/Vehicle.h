#ifndef __SCTMsim__Vehicle__
#define __SCTMsim__Vehicle__

#include "setting.h"

class Vehicle
{
public:
    Vehicle(int);
    Vehicle(const Vehicle&);
    ~Vehicle();
    void PushPass(int);
    void PushTransitTime();
    
    int number;      // 車両番号
    int od_num;      // OD番号
    int stay_time;   // 滞在時間[s]
    int link_time;   // リンクの通過時間
    int sum_time;    // 所用時間合計
    int origin;      // 出発地ノード
    int destination; // 目的地ノード
    double loads; // 車両の台数
    double travel_distance; // 車両の移動距離
    bool move;    // セル間の移動直後はfalse
    bool arrival; // 目的地に到着した場合はtrue
    bool use_dial; // 1度Dial法を使用した場合はtrue

    vector<int> transit_link_time; // リンクを通過する時間を記録
    vector<int> pass_route;    // 通過経路を記録
    vector<vector<int> > route; // 車両が通過する予定の経路
    vector<vector<double> > choice_probability; // 各分岐点における各リンクの選択確率
};

#endif /* defined(__SCTMsim__Vehicle__) */
