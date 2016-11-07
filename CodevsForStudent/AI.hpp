#pragma once

#include "Evaluation.hpp"
#include "Simulator.hpp"

class AI {
public:

	const string think() {

		if (commands.empty())
		{
			commands = thinkChain();
			const int r = abs(Share::getNow() + (int)commands.size() - triggerTurn);
			if (r < 3) triggerTurn += 20;
		}

		const Command com = commands.front();
		commands.pop();

		return com.toString();
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
		Evaluation evaluation;

		const bool operator<(const Data& d) const { return evaluation < d.evaluation; }

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

	int triggerTurn = 0;
	queue<Command> commands;

	const queue<Command> thinkChain() const {

		const int now = Share::getNow();
		const int Width = 64;

		const auto& packs = Share::getPacks();

		priority_queue<Data> que;
		Simulator simulator;

		{
			Data now;
			now.stage = Share::getMyStage();
			now.obstacle = Share::getMyObstacle();
			now.turn = Share::getNow();
			que.emplace(now);
		}

		Timer timer(chrono::milliseconds(1000));
		timer.start();
		while (!timer)
		{
			priority_queue<Data> next;

			if (que.empty()) break;
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
				const auto topScore = top.score;

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
							data.score = topScore + score;
							data.turn = nextTurn;
							data.evaluation = Evaluation(nextStage, score, obstacle, nextTurn, triggerTurn);

							next.emplace(data);
						}
					}

				}
			}

			que = move(next);
		}

		queue<Command> com;
		if (!que.empty())
		{
			const auto top = que.top();

			cerr << "先読み手数:" << top.turn << endl;
			top.evaluation.show();
			com.push(top.command.front());
			return com;
		}

		cerr << "詰みです" << endl;
		com.push(Command());
		return com;
	}

};
