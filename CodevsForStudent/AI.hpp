#pragma once

#include "Evaluation.hpp"
#include "Simulator.hpp"
#include "Game.hpp"

class AI {
public:

	const string think() {

		//前半部分で、攻撃するかどうかの判断を行う
		//攻撃判断には探索を行わず、1次譜面のみで判断を行う
		//攻撃する場合には攻撃可能なパターンを書き出し、次に渡す

		//後半部分で、ブロックの置き方を決める。
		//探査を行い、なるべく連鎖数を増やす
		//パターンが渡されている場合、そのパターンから探査する
		//連鎖はなるべく2個か3個のブロックで行う。

		//凝視の改良を行う
		//本当に打っても大丈夫か
		//連鎖の伸びはどうなのか
		//お邪魔の閾値の変動
		//お邪魔対応の閾値の変動
		const auto attackCommands = attackThink();

		const auto command = chainThink(attackCommands);

		return command.toString();
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
		Command command;
		Game game;
		Evaluation evaluation;
		Evaluation firstEvaluation;

		const bool operator<(const Data& d) const { return evaluation < d.evaluation; }
	};

	Data lastData;

	int enMaxScore;
	Simulator::SimulationData enemyData;

	const vector<Data> attackThink() {

		vector<Data> commands;
		vector<Data> allCommands;
		Simulator simulator;

		const auto& packs = Share::getPacks();
		const int now = Share::getNow();

		int enObstacle = Share::getEnObstacle();
		enMaxScore = getMaxScore(Share::getEnStage(), Share::getEnObstacle(), now);

		enemyData = simulator.getSimulationData(Share::getEnStage(), now);

		//攻撃判断
		{
			const auto& myStage = Share::getMyStage();
			int myObstacle = Share::getMyObstacle();

			set<Hash::Type> hashSet;

			const auto& pack = packs[now];
			const auto& fullPack = packs[now].getFullObstacle(myObstacle);
			const auto& fullPackArr = fullPack.getArray();
			const auto& sides = fullPack.getSide();

			//攻撃判定関数
			const auto shotJudge = [&](const Data& data, const int myScore) {

				const int mySendBlock = score2obstacle(myScore) - myObstacle;
				const int enSendBlock = score2obstacle(enMaxScore) - enObstacle;

				if (myObstacle >= 20)
				{
					if (mySendBlock >= 10)
					{
						return true;
					}
				}

				if (enemyData.maxScoreTurn < 10)
				{
					if (score2obstacle(enemyData.maxScore) > mySendBlock - enemyData.maxScoreTurn * 6)
					{
						return false;
					}
				}

				if (mySendBlock > enSendBlock)
				{
					if (mySendBlock >= Share::getEnFreeSpace() * 0.5)
					{
						return true;
					}
				}

				return false;
			};

			for (int r = 0; r < Rotation; r++)
			{
				const int packWidth = sides[r].second - sides[r].first + 1;
				const int left = 0 - sides[r].first;
				const int right = StageWidth - packWidth + 1 - sides[r].first;

				for (int pos = left; pos < right; pos++)
				{
					Game game(myStage, myObstacle);

					game.update(pos, fullPackArr[r]);

					if (!game.isDead())
					{
						Data data;
						data.command = Command(pos, r);
						data.game = move(game);

						const auto eval = Evaluation(data.game.getStage(), data.game.getScore(), myObstacle, Share::getNow() + 1, enemyData.maxScore, enemyData.maxScoreTurn, enMaxScore);

						/*
						if (shotJudge(data, data.game.getScore()))
						{
							data.evaluation = eval;
							data.firstEvaluation = data.evaluation;
							commands.push_back(data);
						}
						*/

						data.evaluation = eval;
						data.firstEvaluation = data.evaluation;

						allCommands.emplace_back(data);
					}
				}
			}
		}
		/*
		if (!commands.empty())
		{
			cerr << "！発火！" << endl;
			return commands;
		}
		*/
		return allCommands;
	}

	const Command chainThink(const vector<Data>& commands) {

		const auto& packs = Share::getPacks();
		const int now = Share::getNow();
		const int maxTurn = Share::getTurn();

		const int Turn = 10;
		const int ChokudaiWidth = 1;

		array<priority_queue<Data>, Turn + 1> qData;

		for (const auto& com : commands) { qData[0].push(com); }

		Timer timer(chrono::milliseconds(1000));
		timer.start();

		while (!timer)
		{
			for (int t = 0; t < Turn; t++)
			{
				const int turn = now + t + 1;
				if (turn >= maxTurn) break;

				for (int i = 0; i < ChokudaiWidth; i++)
				{
					if (qData[t].empty()) break;

					const auto topData = qData[t].top();
					qData[t].pop();

					const auto& command = topData.command;
					const auto& firstEvaluation = topData.firstEvaluation;
					const auto& game = topData.game;
					const auto& score = game.getScore();

					int obstacle = game.getObstacle();
					const auto& pack = packs[turn];
					const auto& fullPack = pack.getFullObstacle(obstacle);
					const auto& fullPackArr = fullPack.getArray();
					const auto& sides = fullPack.getSide();

					for (int r = 0; r < Rotation; r++)
					{
						const int packWidth = sides[r].second - sides[r].first + 1;
						const int left = 0 - sides[r].first;
						const int right = StageWidth - packWidth + 1 - sides[r].first;

						for (int pos = left; pos < right; pos++)
						{
							Game nGame(game.getStage(), obstacle);

							nGame.update(pos, fullPackArr[r]);

							if (!nGame.isDead())
							{
								Data data;
								data.command = command;
								data.game = move(nGame);

								data.evaluation = Evaluation(data.game.getStage(), score + data.game.getScore(), obstacle, turn + 1, enemyData.maxScore, enemyData.maxScoreTurn, enMaxScore);
								data.firstEvaluation = firstEvaluation;

								qData[t + 1].emplace(data);
							}
						}
					}

				}
			}
		}

		if (!qData[Turn].empty())
		{
			const auto& top = qData[Turn].top();
			lastData = top;
			top.evaluation.show();
			top.firstEvaluation.show();
			int count = 0;
			for (const auto& q : qData)
				count += (int)q.size();
			cerr << count << endl;
			return top.command;
		}

		lastData = Data();

		cerr << "詰みです" << endl;
		return Command();
	}

	const int getMaxScore(const StageArray& seedStage, const int seedObstacle, const int turn, const int count = 2) {

		int maxScore = 0;
		const auto& packs = Share::getPacks();

		queue<Game> stages;
		stages.emplace(Game(seedStage, seedObstacle));

		for (int i = 0; i < count; i++)
		{
			if (turn + i >= Share::getTurn()) break;
			queue<Game> next;

			while (!stages.empty())
			{
				const auto& game = stages.front();
				stages.pop();

				const auto& stage = game.getStage();
				int obstacle = game.getObstacle();
				const auto& pack = packs[turn + i];
				const auto& fullPack = pack.getFullObstacle(obstacle);
				const auto& fullPackArr = fullPack.getArray();
				const auto& sides = fullPack.getSide();

				for (int r = 0; r < Rotation; r++)
				{
					const int packWidth = sides[r].second - sides[r].first + 1;
					const int left = 0 - sides[r].first;
					const int right = StageWidth - packWidth + 1 - sides[r].first;

					for (int pos = left; pos < right; pos++)
					{
						Game nGame(stage, obstacle);

						nGame.update(pos, fullPackArr[r]);

						if (!nGame.isDead())
						{
							maxScore = max(maxScore, nGame.getScore());
							next.emplace(nGame);
						}
					}

				}

			}

			stages = move(next);
		}

		return maxScore;
	}

};
