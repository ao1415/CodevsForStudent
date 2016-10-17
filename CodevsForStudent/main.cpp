#include "Share.hpp"
#include "Simulator.hpp"

class Input {
public:

	static void first() {
		int w, h, t, s;
		cin >> w >> h >> t >> s >> Share::turn;

		for (int i = 0; i < Share::turn; i++)
		{
			Share::packs.emplace_back(Pack::input());
		}
	}

	static bool loop() {
		string endStr;

		cin >> Share::now >> Share::time;

		cin >> Share::myObstacle;
		for (int y = 0; y < StageHeight; y++)
		{
			for (int x = 0; x < StageWidth; x++)
			{
				cin >> Share::myStage[y + 3][x];
			}
		}
		cin >> endStr;

		cin >> Share::enObstacle;
		for (int y = 0; y < StageHeight; y++)
		{
			for (int x = 0; x < StageWidth; x++)
			{
				cin >> Share::enStage[y + 3][x];
			}
		}
		cin >> endStr;

		return endStr == "END";
	}

};

int main() {

	Simulator simu;

	StageArray dummy;

	dummy[0][0] = 4;
	dummy[0][1] = 4;
	dummy[0][2] = 4;
	dummy[0][3] = 4;
	dummy[0][4] = 4;

	dummy[1][0] = 3;
	dummy[1][1] = 3;
	dummy[1][2] = 3;
	dummy[1][3] = 3;
	dummy[1][4] = 3;

	dummy[2][0] = 1;
	dummy[2][1] = 1;
	dummy[2][2] = 1;
	dummy[2][3] = 1;
	dummy[2][4] = 1;
	dummy[2][5] = 5;

	dummy[3][0] = 7;
	dummy[3][1] = 7;
	dummy[3][2] = 7;
	dummy[3][3] = 7;
	dummy[3][4] = 7;
	dummy[3][5] = 7;

	dummy[4][0] = 6;
	dummy[4][1] = 6;
	dummy[4][2] = 6;
	dummy[4][3] = 6;
	dummy[4][4] = 6;
	dummy[4][5] = 6;

	cerr << "ダミー初期" << endl;
	dummy.show();

	simu.fall(dummy);
	cerr << "ダミー落下" << endl;
	dummy.show();
	
	int score = 0;
	simu.next(dummy, score);

	cerr << "ダミー連鎖" << endl;
	cerr << score << endl;
	dummy.show();

	/*
	cout << "ao1415" << endl;
	cout.flush();

	Input::first();

	while (Input::loop())
	{
		cout << rand() % 1 << " " << rand() % 4 << endl;
		cout.flush();
	}
	*/

	return 0;
}
