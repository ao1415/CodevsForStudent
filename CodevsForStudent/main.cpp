#include "Input.hpp"
#include "AI.hpp"

int main() {

	//*
	cout << "ao1415" << endl;
	cout.flush();

	Input::first();

	AI ai;
	Stopwatch sw;

	while (Input::loop())
	{
		cerr << "turn:" << Share::getNow() << "================" << endl;

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
