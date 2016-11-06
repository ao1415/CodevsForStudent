#pragma once

#include "Evaluation.hpp"
#include "Simulator.hpp"

class AI {
public:

	const string think() {

		return Command().toString();
	}

private:

	struct Command {
		Command() = default;
		Command(const int _pos, const int _rota) : pos(_pos), rota(_rota) {}

		int pos = 0;
		int rota = 0;
		const string toString() const { return to_string(pos) + " " + to_string(rota); }
	};

	struct Data {
		StageArray stage;
		queue<Command> command;
		int obstacle = 0;
		int score = 0;
		int turn = 0;
	};

	const int getMaxScore(const StageArray& stage, const Pack& pack) const {

		Simulator simulator;

		const auto& packArr = pack.getArray();
		const auto& sides = pack.getSide();

		int maxScore = 0;

		//現在のターンでの最高スコア
		for (int r = 0; r < Rotation; r++)
		{
			const int packWidth = sides[r].second - sides[r].first + 1;
			const int left = 0 - sides[r].first;
			const int right = StageWidth - packWidth + 1 - sides[r].first;

			for (int pos = left; pos < right; pos++)
			{
				auto nextStage = simulator.csetBlocks(stage, packArr[r], pos);

				int score;
				simulator.next(nextStage, score);
				maxScore = max(maxScore, score);

			}
		}

		return maxScore;
	}

	int triggerTurn = 20;

	const vector<Command> thinkChain() const {

		const int now = Share::getNow();
		const int Width = 32;

		const auto& packs = Share::getPacks();

		priority_queue<Data> que;
		Simulator simulator;

		Timer timer(chrono::milliseconds(1000));
		timer.start();
		while (!timer)
		{
			priority_queue<Data> next;

			const int turn = que.top().turn;
			if (turn >= Share::getTurn()) break;

			for (int w = 0; w < Width; w++)
			{
				if (que.empty()) break;

				const auto top = que.top();
				que.pop();

				int obstacle = top.obstacle;

				const auto pack = packs[turn].getFullObstacle(obstacle);
				const auto packArr = pack.getArray();
				const auto sides = pack.getSide();

				const auto stage = top.stage;
				const auto nextTurn = top.turn + 1;

				for (int r = 0; r < Rotation; r++)
				{
					const int packWidth = sides[r].second - sides[r].first + 1;
					const int left = 0 - sides[r].first;
					const int right = StageWidth - packWidth + 1 - sides[r].first;

					for (int pos = left; pos < right; pos++)
					{
						auto nextStage = simulator.csetBlocks(stage, packArr[r], pos);
						const auto eval = simulator.next(nextStage);

						const int score = get<0>(eval);
						const int chain = get<1>(eval);

						if (!simulator.isDead(nextStage))
						{
							Data data = top;
							data.command.emplace(pos, r);
							data.stage = move(nextStage);
							data.obstacle = obstacle - score2obstacle(score);

							que.emplace(data);
						}
					}

				}
			}

			que = move(next);
		}

		return vector<Command>();
	}

};
