#pragma once

#include "Evaluation.hpp"
#include "Simulator.hpp"

class AI {
public:

	const string think() {

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
		StageArray stage;
		int obstacle;
		Evaluation evaluation;
		Evaluation firstEvaluation;
		int score;

		const bool operator<(const Data& d) const { return evaluation < d.evaluation; }
	};

	Data lastData;

	int enMaxScore;
	Simulator::SimulationData enemyData;

	const vector<Data> attackThink() {

		//vector<Data> commands;
		vector<Data> allCommands;
		Simulator simulator;

		const auto& packs = Share::getPacks();
		const int now = Share::getNow();

		int enObstacle = Share::getEnObstacle();
		enMaxScore = getMaxScore(Share::getEnStage(), Share::getEnObstacle(), now);

		enemyData = simulator.getSimulationData(Share::getEnStage(), now);

		//全探査
		{
			const auto& myStage = Share::getMyStage();
			int myObstacle = Share::getMyObstacle();

			set<Hash::Type> hashSet;

			const auto& myPack = packs[now].getFullObstacle(myObstacle);
			const auto& myPackArr = myPack.getArray();
			const auto& mySides = myPack.getSide();

			//発火判定
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
				const int packWidth = mySides[r].second - mySides[r].first + 1;
				const int left = 0 - mySides[r].first;
				const int right = StageWidth - packWidth + 1 - mySides[r].first;

				for (int pos = left; pos < right; pos++)
				{
					auto nextStage = simulator.csetBlocks(myStage, myPackArr[r], pos);
					simulator.fall(nextStage);

					int score;
					simulator.next(nextStage, score);

					if (!simulator.isDead(nextStage))
					{
						const auto hash = Hash::FNVa(nextStage.data(), sizeof(StageArray));

						if (hashSet.find(hash) == hashSet.end())
						{
							Data data;
							data.command = Command(pos, r);
							data.stage = move(nextStage);
							data.obstacle = myObstacle - score2obstacle(score);
							data.score = score;

							hashSet.insert(hash);

							const auto eval = Evaluation(data.stage, data.score, myObstacle, Share::getNow() + 1, enemyData.maxScore, enemyData.maxScoreTurn, enMaxScore);
							/*
							if (shotJudge(data, score))
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
		}
		/*
		if (!commands.empty())
		{
			cerr << "発火!" << endl;
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

		Simulator simulator;
		array<priority_queue<Data>, Turn + 1> qData;
		array<set<Hash::Type>, Turn> hashSet;

		//gccでコンパイルできなかった
		//qData[0].swap(priority_queue<Data>(commands.begin(), commands.end()));
		for (const auto& com : commands) { qData[0].push(com); }

		//gccでコンパイルできなかった
		//Timer timer(1000ms);

		Timer timer(chrono::milliseconds(1800));
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
					const auto& stage = topData.stage;
					int obstacle = topData.obstacle;
					const auto& pack = packs[turn].getFullObstacle(obstacle);
					const auto& packArr = pack.getArray();
					const auto& sides = pack.getSide();
					const auto& topScore = topData.score;

					for (int r = 0; r < Rotation; r++)
					{
						const int packWidth = sides[r].second - sides[r].first + 1;
						const int left = 0 - sides[r].first;
						const int right = StageWidth - packWidth + 1 - sides[r].first;

						for (int pos = left; pos < right; pos++)
						{
							auto nextStage = simulator.csetBlocks(stage, packArr[r], pos);
							simulator.fall(nextStage);

							int score;
							simulator.next(nextStage, score);

							if (!simulator.isDead(nextStage))
							{
								const auto hash = Hash::FNVa(nextStage.data(), sizeof(StageArray));

								if (hashSet[t].find(hash) == hashSet[t].end())
								{
									Data data;
									data.command = command;
									data.stage = move(nextStage);
									data.obstacle = obstacle - score2obstacle(score);

									data.score = topScore + score;

									data.evaluation = Evaluation(data.stage, data.score, obstacle, turn + 1, enemyData.maxScore, enemyData.maxScoreTurn, enMaxScore);
									data.firstEvaluation = firstEvaluation;

									hashSet[t].insert(hash);

									qData[t + 1].emplace(data);
								}
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
			//top.evaluation.show();
			top.firstEvaluation.show();
			return top.command;
		}

		lastData = Data();

		cerr << "詰みです" << endl;
		return Command();
	}

	const int getMaxScore(const StageArray& seedStage, const int seedObstacle, const int turn, const int count = 2) {

		Simulator simulator;

		int maxScore = 0;
		const auto& packs = Share::getPacks();

		queue<pair<StageArray, int>> stages;
		stages.push({ seedStage,seedObstacle });

		for (int i = 0; i < count; i++)
		{
			if (turn + i >= Share::getTurn()) break;
			queue<pair<StageArray, int>> next;

			while (!stages.empty())
			{
				const auto& front = stages.front();
				stages.pop();
				int obstacle = front.second;

				const auto& stage = front.first;
				const auto& pack = packs[turn + i].getFullObstacle(obstacle);
				const auto& packArr = pack.getArray();
				const auto& sides = pack.getSide();

				for (int r = 0; r < Rotation; r++)
				{
					const int packWidth = sides[r].second - sides[r].first + 1;
					const int left = 0 - sides[r].first;
					const int right = StageWidth - packWidth + 1 - sides[r].first;

					for (int pos = left; pos < right; pos++)
					{
						auto nextStage = simulator.csetBlocks(stage, packArr[r], pos);
						simulator.fall(nextStage);

						int score;
						simulator.next(nextStage, score);

						if (!simulator.isDead(nextStage))
						{
							maxScore = max(maxScore, score);
							next.emplace(pair<StageArray, int>(nextStage, obstacle));
						}
					}

				}

			}

			stages = move(next);
		}

		return maxScore;
	}

};
