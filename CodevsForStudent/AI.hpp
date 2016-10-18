#pragma once

#include "Simulator.hpp"

class AI {
public:

	const string think() {
		string com = "0 0";

		Data now;
		now.stage = Share::getMyStage();
		now.obstacle = Share::getMyObstacle();
		now.score = 0;
		now.command.clear();

		const int Turn = 10;
		const int ChokudaiWidth = 10;

		const auto packs = Share::getPacks();

		Simulator simulator;

		array<priority_queue<Data>, Turn> qData;
		array<set<size_t>, Turn> hashSet;

		qData[0].push(now);

		Timer timer(300, Timer::MilliSecond);
		timer.start();

		while (!timer)
		{
			for (int t = 0; t < Turn - 1; t++)
			{
				for (int i = 0; i < ChokudaiWidth; i++)
				{
					if (qData[t].empty()) break;

					const int turn = Share::getNow() + t;
					int obstacle = qData[t].top().obstacle;

					const auto pack = packs[turn].getFullObstacle(obstacle);
					const auto packArr = pack.getArray();
					const auto sides = pack.getSide();

					for (int r = 0; r < 4; r++)
					{
						const int packWidth = sides[r].second - sides[r].first + 1;
						const int left = 0 - sides[r].first;
						const int right = StageWidth - packWidth + 1 - sides[r].first;

						for (int pos = left; pos < right; pos++)
						{
							Data top = qData[t].top();
							setBlocks(top.stage, packArr[r], pos);
							int score;
							simulator.next(top.stage, score);

							if (!simulator.isDead(top.stage))
							{
								const size_t hash = Hash::FNVa((char*)top.stage.data(), sizeof(int)*top.stage.num_elements());
								if (hashSet[t + 1].find(hash) == hashSet[t + 1].end())
								{
									hashSet[t + 1].insert(hash);

									top.score += score < 5 ? 0 : score;
									top.command.push_back(toCommand(pos, r));
									qData[t + 1].emplace(top);
								}
							}
						}
					}

					qData[t].pop();

				}
			}
		}

		if (!qData[Turn - 1].empty())
		{
			cerr << "Score:" << qData[Turn - 1].top().score << endl;
			return qData[Turn - 1].top().command[0];
		}
		cerr << "�l�݂ł�" << endl;

		return com;
	}

private:

	struct Data {
		StageArray stage;
		int obstacle;
		int score;
		vector<string> command;

		const bool operator<(const Data& d) const { return score < d.score; }

	};

	void setBlocks(StageArray& stage, const PackArray& pack, const int pos) const {

		for (int y = 0; y < PackSize; y++)
		{
			for (int x = 0; x < PackSize; x++)
			{
				if (0 <= x + pos && x + pos < stage.getWidth())
					stage[y][x + pos] = pack[y][x];
			}
		}

	}

	const int eval(const Data& data) const {

		for (int y = 0; y < data.stage.getHeight(); y++)
		{
			for (int x = 0; x < data.stage.getWidth(); x++)
			{

			}
		}

	}

};
