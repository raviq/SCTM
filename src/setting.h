//
//

// マジックナンバー
#define REVISION true 	// 密度の更新式を改良する場合はtrue
#define TEST true	 	// テストの場合はtrue
#define STEP 1        	// シミュレーション間隔時間
#define NOISE_NUM 1000 	// ノイズの数
#define RATIO 0.05    	// 不確実性の分散の範囲(%)
#define LOAD_TIMES (600 / STEP) // 積み込み時間の分割回数（10分間のデータを何分割にするか）

#define MAX_T 1000

#define INF 100000000 	// 無限大
#define DUMMY 1000 		// ダミーリンクのコスト

#define DIAL false  	// Dial法を使用する場合はtrue
#define THETA -0.1 		// Dial法のパラメータ

#define BLUE 1
#define YELLOW 2
#define RED 3
#define NOSIGNAL -1

// インクルード
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <ctime>
#include <climits>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <boost/random.hpp>
#include <boost/math/constants/constants.hpp>
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Eigen>
#include <eigen3/Eigen/Dense>

// ネームスペース
using namespace std;
using namespace boost;
using namespace boost::math::constants;
using namespace Eigen;
