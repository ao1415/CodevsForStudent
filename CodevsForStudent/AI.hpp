#pragma once

#include "Evaluation.hpp"
#include "Simulator.hpp"

class AI {
public:

	const string think() {

		const auto packs = Share::getPacks();
		Simulator simulator;

		Data enemy;
		{
			enemy.stage = Share::getEnStage();
			enemy.obstacle = Share::getEnObstacle();

			Evaluation evaluation;
			enemy.score = evaluation.getScore(enemy.stage, enemy.obstacle, 0, Share::getNow());

			enemy.evaluation = move(evaluation);
		}

		Data now;
		now.stage = Share::getMyStage();
		now.obstacle = Share::getMyObstacle();

		const int nowTurn = Share::getNow();
		const int maxTurn = Share::getTurn();

		const int Turn = 10;
		const int ChokudaiWidth = 10;

		array <priority_queue<Data>, Turn> qData;
		array<set<size_t>, Turn> hashSet;

		qData[0].emplace(now);

		Timer timer(2000, Timer::MilliSecond);
		timer.start();

		for (int c = 0; c < 2; c++)
		{
			for (int t = 0; t < Turn - 1; t++)
			{
				const int turn = nowTurn + t;
				if (turn >= maxTurn) break;

				for (int i = 0; i < ChokudaiWidth; i++)
				{
					if (qData[t].empty()) break;

					int obstacle = qData[t].top().obstacle;

					const auto pack = packs[turn].getFullObstacle(obstacle);
					const auto packArr = pack.getArray();
					const auto sides = pack.getSide();

					for (int r = 0; r < Rotation; r++)
					{
						const int packWidth = sides[r].second - sides[r].first + 1;
						const int left = 0 - sides[r].first;
						const int right = StageWidth - packWidth + 1 - sides[r].first;

						for (int pos = left; pos < right; pos++)
						{
							Data top = qData[t].top();

							int score;
							simulator.setBlocks(top.stage, packArr[r], pos);
							simulator.fall(top.stage);
							simulator.next(top.stage, score);

							if (!simulator.isDead(top.stage))
							{
								const size_t hash = Hash::FNVa((char*)top.stage.data(), sizeof(StageArray));
								if (hashSet[t + 1].find(hash) == hashSet[t + 1].end())
								{
									Evaluation evaluation;
									top.score += evaluation.getScore(top.stage, obstacle, score, turn, enemy.evaluation);

									top.obstacle = obstacle;
									if (t == 0)
									{
										top.evaluation = move(evaluation);
										top.command.pos = pos;
										top.command.rota = r;
									}
									qData[t + 1].emplace(top);
								}
							}
						}
					}
					qData[t].pop();
				}

			}
		}

		for (int i = Turn - 1; i >= 0; i--)
		{
			if (!qData[i].empty())
			{
				qData[i].top().evaluation.showTotalScore();
				return qData[i].top().command.toString();
			}
		}

		cerr << "‹l‚Ý‚Å‚·" << endl;
		return "0 0";
	}

private:

	struct Command {
		int pos = 0;
		int rota = 0;

		const string toString() const { return to_string(pos) + " " + to_string(rota); }

	};

	struct Data {
		StageArray stage;
		int obstacle = 0;
		int score = INT32_MIN;
		Command command;

		Evaluation evaluation;

		const bool operator<(const Data& d) const { return score < d.score; }
	};

	int addScore = 0;

};
