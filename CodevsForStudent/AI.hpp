#pragma once

#include "Evaluation.hpp"
#include "Simulator.hpp"

class AI {
public:

	const string think() {
		string com = "0 0";

		const auto packs = Share::getPacks();
		Simulator simulator;

		Data now;
		now.stage = Share::getMyStage();
		now.obstacle = Share::getMyObstacle();
		now.score = 0;
		now.evaluation.emplace_back(Evaluation());
		now.evaluation.back().set(addScore);

		priority_queue<Data> qData;
		qData.emplace(now);

		Data topData;

		const int Turn = 10;
		const int BeamWidth = 10;

		for (int t = 0; t < Turn; t++)
		{
			const int turn = Share::getNow() + t;
			if (turn >= Share::getTurn()) break;

			priority_queue<Data> qNext;

			set<size_t> hashSet;

			for (int i = 0; i < BeamWidth; i++)
			{
				if (qData.empty()) break;

				int obstacle = qData.top().obstacle;

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
						Data top = qData.top();
						simulator.setBlocks(top.stage, packArr[r], pos);
						simulator.fall(top.stage);

						int score;
						simulator.next(top.stage, score);

						if (!simulator.isDead(top.stage))
						{
							const size_t hash = Hash::FNVa((char*)top.stage.data(), sizeof(StageArray));
							if (hashSet.find(hash) == hashSet.end())
							{
								hashSet.insert(hash);

								//ScoreBoard scoreBoard;
								//top.score += eval(top, score, scoreBoard);
								//top.score = eval(top, score, scoreBoard);

								Evaluation evaluation;
								evaluation.set(top.evaluation.back().get() + score);
								top.score = evaluation.getScore(top.stage, top.obstacle, score, turn);

								top.obstacle = obstacle;

								if (t == 0)
								{
									top.evaluation.emplace_back(evaluation);
									top.command.pos = pos;
									top.command.rota = r;
								}

								qNext.emplace(top);
							}
						}

					}
				}
				qData.pop();
			}
			qData.swap(qNext);
			if (!qData.empty())
				topData = qData.top();
		}

		if (topData.evaluation.size() > 1)
		{
			topData.evaluation[1].show();
			addScore += topData.evaluation[1].getAttackScore();
			//qData.top().scoreBoard[0].show();
			//qData.top().scoreBoard[0].showAttackScore();
			//cerr << "スコア:" << addScore << endl;
			//cerr << "総合スコア\t\t:" << qData.top().score << endl;
			return topData.command.toString();
		}

		cerr << "詰みです" << endl;
		return com;
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
		Command command;

		vector<Evaluation> evaluation;

		const bool operator<(const Data& d) const { return score < d.score; }
	};

	int addScore = 0;

};
