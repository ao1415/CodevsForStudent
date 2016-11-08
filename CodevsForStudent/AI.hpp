#pragma once

#include "Evaluation.hpp"
#include "Simulator.hpp"

class AI {
public:

	const string think() {

		Simulator simulator;
		const int now = Share::getNow();

		const auto enObstacle = Share::getEnObstacle();
		const auto enMaxScore = getStrength(Share::getEnStage(), Share::getEnObstacle());

		const auto myObstacle = Share::getMyObstacle();
		const auto myMaxScore = getStrength(Share::getMyStage(), Share::getMyObstacle());

		const auto enemyData = simulator.getSimulationData(Share::getEnStage(), now);

		const int enSendBlocks = score2obstacle(get<0>(enMaxScore)) - enObstacle;
		const int mySendBlocks = score2obstacle(get<0>(myMaxScore)) - myObstacle;

		//ëäéËÇ™ívñΩìIÇ»çUåÇÇÇ≈Ç´ÇÈ
		const bool enFatalAttacked = (enSendBlocks >= Share::getMyFreeSpace()*0.5);
		//ëäéËÇ™çUåÇÇÇ≈Ç´ÇÈ
		const bool enAttacked = (enSendBlocks >= 50);

		const bool enAttacking = (myObstacle > myObstacleNumber);

		//é©ï™Ç™ívñΩìIÇ»çUåÇÇÇ≈Ç´ÇÈ
		const bool myFatalAttacked = (mySendBlocks >= Share::getEnFreeSpace()*0.5);
		//é©ï™Ç™çUåÇÇÇ≈Ç´ÇÈ
		const bool myAttacked = (mySendBlocks >= 50);

		enAttackChanged = (!enAttackState && enAttackState != enAttacking);
		enAttackState = enAttacking;

		if (enAttacking)
		{
			if (enFatalAttacked)
			{
				if (myAttacked)
				{
					if (enSendBlocks - mySendBlocks >= -5)
					{
						queue<Command> buff;
						for (const auto com : get<1>(myMaxScore)) buff.push(com);
						commandBuffer = move(buff);
						cerr << "ívñΩìIÇ»çUåÇÇÇ≥ÇÍÇΩÇÃÇ≈ëŒâûÇ∑ÇÈ" << endl;
					}
				}
			}
			else if (enAttacked)
			{
				if (myAttacked)
				{
					if (enSendBlocks - mySendBlocks >= 10)
					{
						queue<Command> buff;
						for (const auto com : get<1>(myMaxScore)) buff.push(com);
						commandBuffer = move(buff);
						cerr << "çUåÇÇÇ≥ÇÍÇΩÇÃÇ≈ëŒâûÇ∑ÇÈ" << endl;
					}
				}
			}
		}
		else if (myFatalAttacked)
		{
			myObstacleNumber = myObstacle;
			if (enemyData.maxScoreTurn < 10)
			{
				if (score2obstacle(enemyData.maxScore) > mySendBlocks - enemyData.maxScoreTurn * 6)
				{
					queue<Command> buff;
					for (const auto com : get<1>(myMaxScore)) buff.push(com);
					commandBuffer = move(buff);
					cerr << "îΩåÇÇ≥ÇÍÇƒÇ‡ëÂè‰ïvÇªÇ§Ç»ÇÃÇ≈çUåÇÇ∑ÇÈ" << endl;
				}
			}
			else
			{
				queue<Command> buff;
				for (const auto com : get<1>(myMaxScore)) buff.push(com);
				commandBuffer = move(buff);
				cerr << "îΩåÇÇ≥ÇÍÇ»Ç≥ÇªÇ§Ç»ÇÃÇ≈çUåÇÇ∑ÇÈ" << endl;
			}
		}

		if (commandBuffer.empty() || enAttackChanged)
		{
			enAttackChanged = false;

			Data seed;
			seed.stage = Share::getMyStage();
			seed.obstacle = Share::getMyObstacle();
			seed.turn = Share::getNow();

			const Data data = chainThink(seed, chrono::milliseconds(10000));
			saveData = data;
			saveData.evaluation.show();
			commandBuffer = saveData.command;
		}

		if (!commandBuffer.empty())
		{
			const Command com = commandBuffer.front();
			commandBuffer.pop();
			return com.toString();
		}

		return "0 0";
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
		queue<Command> command;
		StageArray stage;
		int obstacle;
		Evaluation evaluation;
		int turn = 0;

		const bool operator<(const Data& d) const { return evaluation < d.evaluation; }
	};

	bool enAttackState = false;
	bool enAttackChanged = false;

	int myObstacleNumber = 0;

	queue<Command> commandBuffer;
	Data saveData;

	const tuple<int, vector<Command>> getStrength(const StageArray& stage, int obstacle) const {

		int maxScore = 0;
		vector<Command> commands;

		const auto& packs = Share::getPacks();
		const auto& now = Share::getNow();

		Simulator simulator;

		int obstacle1 = obstacle;

		const auto& pack1 = packs[now].getFullObstacle(obstacle1);
		const auto& packArr1 = pack1.getArray();
		const auto& sides1 = pack1.getSide();

		//2É^Å[ÉìÇ≈ÇÃç≈çÇÉXÉRÉA
		for (int r1 = 0; r1 < Rotation; r1++)
		{
			const int packWidth1 = sides1[r1].second - sides1[r1].first + 1;
			const int left1 = 0 - sides1[r1].first;
			const int right1 = StageWidth - packWidth1 + 1 - sides1[r1].first;

			for (int pos1 = left1; pos1 < right1; pos1++)
			{
				auto nextStage1 = simulator.csetBlocks(stage, packArr1[r1], pos1);
				simulator.fall(nextStage1);

				int score1;
				simulator.next(nextStage1, score1);

				if (!simulator.isDead(nextStage1))
				{
					if (maxScore < score1)
					{
						maxScore = score1;
						vector<Command> com;
						com.push_back(Command(pos1, r1));
						commands = move(com);
					}

					int obstacle2 = obstacle1 - score2obstacle(score1);

					const auto& pack2 = packs[now + 1].getFullObstacle(obstacle2);
					const auto& packArr2 = pack2.getArray();
					const auto& sides2 = pack2.getSide();

					for (int r2 = 0; r2 < Rotation; r2++)
					{
						const int packWidth2 = sides2[r2].second - sides2[r2].first + 1;
						const int left2 = 0 - sides2[r2].first;
						const int right2 = StageWidth - packWidth2 + 1 - sides2[r2].first;

						for (int pos2 = left2; pos2 < right2; pos2++)
						{
							auto nextStage2 = simulator.csetBlocks(nextStage1, packArr2[r2], pos2);
							simulator.fall(nextStage2);

							int score2;
							simulator.next(nextStage2, score2);

							if (!simulator.isDead(nextStage2))
							{
								if (maxScore < score2)
								{
									maxScore = score2;
									vector<Command> com;
									com.push_back(Command(pos1, r1));
									com.push_back(Command(pos2, r2));
									commands = move(com);
								}
							}
						}
					}
				}
			}
		}

		return{ maxScore,commands };
	}

	const Data chainThink(const Data& seed, const chrono::milliseconds& ms) {

		const auto& packs = Share::getPacks();
		const int now = seed.turn;
		const int maxTurn = Share::getTurn();

		const int Turn = 10;
		const int ChokudaiWidth = 1;

		Simulator simulator;
		array<priority_queue<Data>, Turn + 1> qData;
		//array<set<Hash::Type>, Turn> hashSet;

		qData[0].push(seed);

		Timer timer(ms);

		timer.start();

		while (!timer)
		{
			bool flag = false;
			for (int t = 0; t < Turn; t++)
			{
				const int turn = now + t;
				if (turn >= maxTurn) break;

				for (int i = 0; i < ChokudaiWidth; i++)
				{
					if (qData[t].empty()) break;

					const auto topData = qData[t].top();
					qData[t].pop();

					const auto& command = topData.command;
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
								Data data;
								data.command = command;
								data.command.emplace(Command(pos, r));

								data.stage = move(nextStage);
								data.obstacle = obstacle - score2obstacle(score);
								data.evaluation = Evaluation(data.stage, score, obstacle, turn + 1);

								qData[t + 1].emplace(data);

								flag = true;
							}
						}
					}

				}
			}
			if (!flag) break;
		}

		if (!qData[Turn].empty())
		{
			const auto& top = qData[Turn].top();
			return top;
		}

		cerr << "ãlÇ›Ç≈Ç∑" << endl;
		return Data();
	}

};
