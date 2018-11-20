#include "setting.h"
#include "Simulator.h"
#include "TestSimulator.h"

int main(int argc, const char * argv[])
{		
	cout << "Simulation started." << endl;

	// カレントディレクトリを取得
	string dir(getcwd(NULL, 0));

	// シミュレート開始
	if(TEST)
	{
		dir += "/test_data";
		TestSimulator test(dir);
		test.start();
		cout << "Test simulation ended." << endl;
		// Rで密度とモードの生起確率の変移のグラフを描画
		system("cd test_data/graph/; sudo ./make_graph.sh; cd ../..");
	}
	else
	{
		dir += "/data";
		Simulator sim(dir);
		sim.start();

		cout << "Simulation ended." << endl;

		// Rで10分間の交通量を記録し描画
		system("cd data/graph/; sudo ./make_graph.sh; cd ../..");
	}

	return 0;
}