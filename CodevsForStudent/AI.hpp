#pragma once

#include "Evaluation.hpp"
#include "Simulator.hpp"

class AI {
public:

	const string think() {

		const auto packs = Share::getPacks();
		Simulator simulator;

		int obstacle = Share::getMyObstacle();

		const auto pack = packs[Share::getNow()].getFullObstacle(obstacle);
		const auto packArr = pack.getArray();
		const auto sides = pack.getSide();

		const auto stage = Share::getMyStage();

		int maxScore = INT32_MIN;
		Command maxCommand;

		for (int r = 0; r < Rotation; r++)
		{
			const int packWidth = sides[r].second - sides[r].first + 1;
			const int left = 0 - sides[r].first;
			const int right = StageWidth - packWidth + 1 - sides[r].first;

			for (int pos = left; pos < right; pos++)
			{
				int score;
				auto rStage(simulator.csetBlocks(stage, packArr[r], pos));
				simulator.fall(rStage);
				simulator.next(rStage, score);

				if (!simulator.isDead(rStage))
				{
					Evaluation evaluation;
					int eval = evaluation.getScore(rStage, obstacle, score, Share::getNow());

					if (maxScore < eval)
					{
						maxScore = eval;
						maxCommand.pos = pos;
						maxCommand.rota = r;
					}
				}
			}
		}

		return maxCommand.toString();
	}

private:

	struct Command {
		int pos = 0;
		int rota = 0;

		const string toString() const { return to_string(pos) + " " + to_string(rota); }

	};

	struct Data {
		StageArray stage;
		int obstacle;
		int score;
		string command;

		vector<Evaluation> evaluation;

		const bool operator<(const Data& d) const { return score < d.score; }
	};

	int addScore = 0;

};
