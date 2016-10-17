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

	cout << "ao1415" << endl;
	cout.flush();

	StageArray dummy;
	for (int y = 0; y < StageHeight; y++)
	{
		for (int x = 0; x < StageWidth; x++)
		{
			dummy[y + 3][x] = (x + y) % 2;
		}
	}

	cerr << "ダミー初期" << endl;
	dummy.show();
	Simulator simu;
	const auto arr = simu.cfall(dummy);
	cerr << "ダミー変化なし" << endl;
	dummy.show();
	cerr << "アレイ変化" << endl;
	arr.show();

	simu.fall(dummy);
	cerr << "ダミー変化" << endl;
	dummy.show();

	Input::first();

	while (Input::loop())
	{
		cout << rand() % 1 << " " << rand() % 4 << endl;
		cout.flush();
	}

	return 0;
}
