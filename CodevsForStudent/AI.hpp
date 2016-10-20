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
		now.command.clear();
		now.score = 0;

		priority_queue<Data> qData;
		qData.emplace(now);

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
								top.score += evaluation.getScore(top.stage, top.obstacle, score, turn);

								top.scoreBoard.emplace_back(evaluation);

								top.obstacle = obstacle;
								if (top.command.empty()) top.command = toCommand(pos, r);

								qNext.emplace(top);
							}
						}

					}
				}

				qData.pop();
			}
			qData.swap(qNext);
		}

		if (!qData.empty())
		{
			//qData.top().scoreBoard[0].show();
			qData.top().scoreBoard[0].showTotalScore();
			addScore += qData.top().scoreBoard[0].get();
			//qData.top().scoreBoard[0].showAttackScore();
			cerr << "スコア:" << addScore << endl;
			//cerr << "総合スコア\t\t:" << qData.top().score << endl;
			return qData.top().command;
		}

		cerr << "詰みです" << endl;
		return com;
	}

private:

	struct Data {
		StageArray stage;
		int obstacle;
		int score;
		string command;

		vector<Evaluation> scoreBoard;

		const bool operator<(const Data& d) const { return score < d.score; }
	};

	int addScore = 0;

};
