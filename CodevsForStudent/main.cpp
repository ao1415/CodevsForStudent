#include "Input.hpp"
#include "AI.hpp"

int main() {

	/*
	PackArray packArr;
	packArr[0][0] = 8;
	packArr[0][1] = 0;
	packArr[0][2] = 0;
	packArr[1][0] = 0;
	packArr[1][1] = 0;
	packArr[1][2] = 3;
	packArr[2][0] = 3;
	packArr[2][1] = 5;
	packArr[2][2] = 0;

	Pack pack(packArr);

	StageArray stage;
	stage[3][0] = 0;
	stage[3][1] = 3;
	stage[3][2] = 3;
	stage[3][3] = 0;
	stage[3][4] = 0;
	stage[3][5] = 3;
	stage[3][6] = 0;
	stage[3][7] = 0;
	stage[3][8] = 0;
	stage[3][9] = 0;
	stage[4][0] = 0;
	stage[4][1] = 2;
	stage[4][2] = 6;
	stage[4][3] = 0;
	stage[4][4] = 0;
	stage[4][5] = 5;
	stage[4][6] = 1;
	stage[4][7] = 7;
	stage[4][8] = 8;
	stage[4][9] = 0;
	stage[5][0] = 0;
	stage[5][1] = 6;
	stage[5][2] = 6;
	stage[5][3] = 0;
	stage[5][4] = 0;
	stage[5][5] = 7;
	stage[5][6] = 7;
	stage[5][7] = 4;
	stage[5][8] = 7;
	stage[5][9] = 0;
	stage[6][0] = 0;
	stage[6][1] = 8;
	stage[6][2] = 1;
	stage[6][3] = 0;
	stage[6][4] = 9;
	stage[6][5] = 9;
	stage[6][6] = 5;
	stage[6][7] = 9;
	stage[6][8] = 7;
	stage[6][9] = 0;
	stage[7][0] = 0;
	stage[7][1] = 1;
	stage[7][2] = 4;
	stage[7][3] = 0;
	stage[7][4] = 9;
	stage[7][5] = 6;
	stage[7][6] = 9;
	stage[7][7] = 7;
	stage[7][8] = 6;
	stage[7][9] = 1;
	stage[8][0] = 0;
	stage[8][1] = 8;
	stage[8][2] = 7;
	stage[8][3] = 4;
	stage[8][4] = 7;
	stage[8][5] = 7;
	stage[8][6] = 2;
	stage[8][7] = 5;
	stage[8][8] = 6;
	stage[8][9] = 3;
	stage[9][0] = 0;
	stage[9][1] = 4;
	stage[9][2] = 9;
	stage[9][3] = 5;
	stage[9][4] = 9;
	stage[9][5] = 6;
	stage[9][6] = 9;
	stage[9][7] = 9;
	stage[9][8] = 6;
	stage[9][9] = 1;
	stage[10][0] = 7;
	stage[10][1] = 7;
	stage[10][2] = 4;
	stage[10][3] = 4;
	stage[10][4] = 8;
	stage[10][5] = 8;
	stage[10][6] = 3;
	stage[10][7] = 3;
	stage[10][8] = 5;
	stage[10][9] = 8;
	stage[11][0] = 7;
	stage[11][1] = 7;
	stage[11][2] = 2;
	stage[11][3] = 5;
	stage[11][4] = 4;
	stage[11][5] = 5;
	stage[11][6] = 9;
	stage[11][7] = 6;
	stage[11][8] = 8;
	stage[11][9] = 4;
	stage[12][0] = 8;
	stage[12][1] = 7;
	stage[12][2] = 2;
	stage[12][3] = 2;
	stage[12][4] = 9;
	stage[12][5] = 3;
	stage[12][6] = 9;
	stage[12][7] = 9;
	stage[12][8] = 3;
	stage[12][9] = 4;
	stage[13][0] = 8;
	stage[13][1] = 1;
	stage[13][2] = 7;
	stage[13][3] = 9;
	stage[13][4] = 5;
	stage[13][5] = 9;
	stage[13][6] = 5;
	stage[13][7] = 2;
	stage[13][8] = 4;
	stage[13][9] = 5;
	stage[14][0] = 6;
	stage[14][1] = 6;
	stage[14][2] = 7;
	stage[14][3] = 6;
	stage[14][4] = 8;
	stage[14][5] = 8;
	stage[14][6] = 4;
	stage[14][7] = 1;
	stage[14][8] = 7;
	stage[14][9] = 4;
	stage[15][0] = 8;
	stage[15][1] = 8;
	stage[15][2] = 6;
	stage[15][3] = 5;
	stage[15][4] = 8;
	stage[15][5] = 8;
	stage[15][6] = 8;
	stage[15][7] = 5;
	stage[15][8] = 8;
	stage[15][9] = 5;
	stage[16][0] = 3;
	stage[16][1] = 8;
	stage[16][2] = 3;
	stage[16][3] = 3;
	stage[16][4] = 3;
	stage[16][5] = 6;
	stage[16][6] = 7;
	stage[16][7] = 7;
	stage[16][8] = 6;
	stage[16][9] = 9;
	stage[17][0] = 3;
	stage[17][1] = 3;
	stage[17][2] = 5;
	stage[17][3] = 1;
	stage[17][4] = 5;
	stage[17][5] = 6;
	stage[17][6] = 2;
	stage[17][7] = 9;
	stage[17][8] = 5;
	stage[17][9] = 3;
	stage[18][0] = 2;
	stage[18][1] = 9;
	stage[18][2] = 3;
	stage[18][3] = 2;
	stage[18][4] = 1;
	stage[18][5] = 3;
	stage[18][6] = 2;
	stage[18][7] = 9;
	stage[18][8] = 8;
	stage[18][9] = 3;

	Stopwatch sw;

	sw.start();
	Game game(stage, 0);
	game.update(6, 1, pack);
	sw.stop();
	cout << sw.microseconds() << endl;

	sw.start();
	Simulator simu;
	simu.setBlocks(stage, pack.getArray(1), 6);
	simu.fall(stage);
	int score;
	simu.next(stage, score);
	sw.stop();
	cout << sw.microseconds() << endl;

	/*/
	cout << "ao1415" << endl;
	cout.flush();

	Input::first();

	AI ai;
	Stopwatch sw;

	while (Input::loop())
	{
		Share::update();

		cerr << "turn:" << std::setw(3) << Share::getNow() << "================" << endl;

		sw.start();
		const string com = ai.think();
		sw.stop();
		cerr << "time:" << sw.millisecond() << "ms" << endl;

		cerr.flush();
		cout << com << endl;
		cout.flush();
	}
	//*/

	return 0;
}
