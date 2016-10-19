#include "Share.hpp"
#include "AI.hpp"

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

	//*
	cout << "ao1415" << endl;
	cout.flush();

	Input::first();

	AI ai;

	while (Input::loop())
	{
		cerr << "turn:" << Share::getNow() << "================" << endl;
		const string com = ai.think();
		cerr.flush();
		cout << com << endl;
		cout.flush();
	}
	//*/

	return 0;
}
