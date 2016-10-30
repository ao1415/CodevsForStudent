#pragma once

#include "Evaluation.hpp"
#include "Simulator.hpp"

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
		StageArray stage;
		int obstacle;
		Evaluation evaluation;
		Evaluation firstEvaluation;

		const bool operator<(const Data& d) const { return evaluation < d.evaluation; }
	};

	Data lastData;

	const vector<Data> attackThink() {

		vector<Data> commands;
		vector<Data> allCommands;
		Simulator simulator;

		const auto& packs = Share::getPacks();
		const int now = Share::getNow();

		int enMaxScore = 0;
		int enObstacle = Share::getEnObstacle();
		int enMaxSpace = 0;

		const auto enemyData = simulator.getSimulationData(Share::getEnStage(), now);

		//凝視
		{
			const auto& enStage = Share::getEnStage();

			const auto& enPack = packs[now].getFullObstacle(enObstacle);
			const auto& enPackArr = enPack.getArray();
			const auto& enSides = enPack.getSide();

			//現在のターンでの最高スコア
			for (int r = 0; r < Rotation; r++)
			{
				const int packWidth = enSides[r].second - enSides[r].first + 1;
				const int left = 0 - enSides[r].first;
				const int right = StageWidth - packWidth + 1 - enSides[r].first;

				for (int pos = left; pos < right; pos++)
				{
					auto nextStage = simulator.csetBlocks(enStage, enPackArr[r], pos);
					simulator.fall(nextStage);

					int score;
					simulator.next(nextStage, score);
					enMaxScore = max(enMaxScore, score);

					/*
					int space = 0;
					for (int x = 0; x < nextStage.getWidth(); x++)
					{
						for (int y = nextStage.getHeight() - 1; y >= 0; y++)
						{
							if (nextStage[y][x] == EmptyBlock)
								break;
							space++;
						}
					}
					enMaxSpace = max(enMaxSpace, space);
					*/
				}
			}

		}

		//攻撃判断
		{
			const auto& myStage = Share::getMyStage();
			int myObstacle = Share::getMyObstacle();

			set<Hash::Type> hashSet;

			const auto& myPack = packs[now].getFullObstacle(myObstacle);
			const auto& myPackArr = myPack.getArray();
			const auto& mySides = myPack.getSide();

			//攻撃判定関数
			const auto shotJudge = [&](const Data& data, const int myScore) {

				const int mySendBlock = score2obstacle(myScore) - myObstacle;
				const int enSendBlock = score2obstacle(enMaxScore) - enObstacle;

				if (enObstacle >= 20)
				{
					if (enSendBlock - mySendBlock >= 10)
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
					//if (mySendBlock >= 50)
					//{
					//	return true;
					//}
					//if (mySendBlock >= enMaxSpace * 0.3)
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

							hashSet.insert(hash);

							if (shotJudge(data, score))
							{
								data.evaluation = Evaluation(data.stage, 0, myObstacle, Share::getNow() + 1);
								data.firstEvaluation = data.evaluation;
								commands.push_back(data);
							}

							data.evaluation = Evaluation(data.stage, score, myObstacle, Share::getNow() + 1);
							data.firstEvaluation = data.evaluation;

							allCommands.emplace_back(data);
						}
					}
				}
			}
		}

		if (!commands.empty())
		{
			cerr << "！発火！" << endl;
			return commands;
		}

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

		Timer timer(1000ms);
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
									data.evaluation = Evaluation(data.stage, score, obstacle, turn + 1);
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

};
