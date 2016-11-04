#pragma once

#include "Evaluation.hpp"
#include "Simulator.hpp"

class AI {
public:

	const string think() {

		//�O�������ŁA�U�����邩�ǂ����̔��f���s��
		//�U�����f�ɂ͒T�����s�킸�A1�����ʂ݂̂Ŕ��f���s��
		//�U������ꍇ�ɂ͍U���\�ȃp�^�[���������o���A���ɓn��

		//�㔼�����ŁA�u���b�N�̒u���������߂�B
		//�T�����s���A�Ȃ�ׂ��A�����𑝂₷
		//�p�^�[�����n����Ă���ꍇ�A���̃p�^�[������T������
		//�A���͂Ȃ�ׂ�2��3�̃u���b�N�ōs���B

		//�Î��̉��ǂ��s��
		//�{���ɑł��Ă����v��
		//�A���̐L�т͂ǂ��Ȃ̂�
		//���ז���臒l�̕ϓ�
		//���ז��Ή���臒l�̕ϓ�
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

	const priority_queue<Data> enemyThink(int obstacle) {

		const auto& packs = Share::getPacks();
		const int now = Share::getNow();
		const int maxTurn = Share::getTurn();

		const int Turn = 10;
		const int ChokudaiWidth = 1;

		Simulator simulator;
		array<priority_queue<Data>, Turn + 1> qData;
		array<set<pair<int, Hash::Type>>, Turn> hashSet;

		{
			const auto& stage = Share::getMyStage();

			set<pair<int, Hash::Type>> hashSet;

			const auto& pack = packs[now].getFullObstacle(obstacle);
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

					int score;
					simulator.next(nextStage, score);

					if (!simulator.isDead(nextStage))
					{
						const auto stageHash = Hash::FNVa(nextStage.data(), sizeof(StageArray));
						const pair<int, Hash::Type> hash = { score,stageHash };

						if (hashSet.find(hash) == hashSet.end())
						{
							Data data;
							data.command = Command(pos, r);
							data.stage = move(nextStage);
							data.obstacle = obstacle - score2obstacle(score);

							hashSet.insert(hash);

							data.evaluation = Evaluation(data.stage, score, obstacle, Share::getNow() + 1);
							data.firstEvaluation = data.evaluation;

							data.score = score;

							qData[0].emplace(data);
						}
					}
				}
			}
		}

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

							int score;
							simulator.next(nextStage, score);

							if (!simulator.isDead(nextStage))
							{
								const auto stageHash = Hash::FNVa(nextStage.data(), sizeof(StageArray));
								const pair<int, Hash::Type> hash = { score,stageHash };

								if (hashSet[t].find(hash) == hashSet[t].end())
								{
									Data data;
									data.stage = move(nextStage);
									data.obstacle = obstacle - score2obstacle(score);
									data.evaluation = Evaluation(data.stage, score, obstacle, turn + 1);
									data.firstEvaluation = firstEvaluation;

									data.score += score;

									hashSet[t].insert(hash);

									qData[t + 1].emplace(data);
								}
							}
						}
					}

				}
			}
		}

		return qData[Turn];
	}

	const int getMaxScore(const StageArray& stage, const Pack& pack) const {

		Simulator simulator;

		const auto& packArr = pack.getArray();
		const auto& sides = pack.getSide();

		int maxScore = 0;

		//���݂̃^�[���ł̍ō��X�R�A
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

	const vector<Data> attackThink() {

		vector<Data> commands;
		vector<Data> allCommands;
		Simulator simulator;

		const auto& packs = Share::getPacks();
		const int now = Share::getNow();

		int enObstacle = Share::getEnObstacle();
		int enMaxScore = getMaxScore(Share::getEnStage(), packs[now].getFullObstacle(enObstacle));

		const auto enemyData = simulator.getSimulationData(Share::getEnStage(), now);

		//���Ƃ����ĉ��ǂ�����
		//�U�����f
		const auto& myStage = Share::getMyStage();
		int myObstacle = Share::getMyObstacle();
		int myMaxScore = getMaxScore(Share::getMyStage(), packs[now].getFullObstacle(myObstacle));

		{
			set<pair<int, Hash::Type>> hashSet;

			const auto& myPack = packs[now].getFullObstacle(myObstacle);
			const auto& myPackArr = myPack.getArray();
			const auto& mySides = myPack.getSide();

			//�U������֐�
			const auto shotJudge = [&](const Data& data, const int myScore) {

				const int mySendBlock = score2obstacle(myScore) - myObstacle;
				const int enSendBlock = score2obstacle(enMaxScore) - enObstacle;

				if (myObstacle >= 20)
				{
					if (mySendBlock - enSendBlock >= 5)
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
				//����𓦂����玀�S����Ȃ�Δ��΂ɕύX����
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
					//simulator.fall(nextStage);

					int score;
					simulator.next(nextStage, score);

					if (!simulator.isDead(nextStage))
					{
						const auto stageHash = Hash::FNVa(nextStage.data(), sizeof(StageArray));
						const pair<int, Hash::Type> hash = { score,stageHash };

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
			bool attackFlag = true;
			const auto& enQueue = enemyThink(score2obstacle(myMaxScore) - myObstacle);

			if (!enQueue.empty())
			{
				const auto data = enQueue.top();
				if (score2obstacle(data.score) - (score2obstacle(myMaxScore) - myObstacle) >= -10)
					attackFlag = false;
			}

			if (attackFlag)
			{
				cerr << "�I���΁I" << endl;
				return commands;
			}
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
		array<set<pair<int, Hash::Type>>, Turn> hashSet;

		//gcc�ŃR���p�C���ł��Ȃ�����
		//qData[0].swap(priority_queue<Data>(commands.begin(), commands.end()));
		for (const auto& com : commands) { qData[0].push(com); }

		//gcc�ŃR���p�C���ł��Ȃ�����
		//Timer timer(1000ms);

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
							//simulator.fall(nextStage);

							int score;
							simulator.next(nextStage, score);

							if (!simulator.isDead(nextStage))
							{
								const auto stageHash = Hash::FNVa(nextStage.data(), sizeof(StageArray));
								const pair<int, Hash::Type> hash = { score,stageHash };

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

		cerr << "�l�݂ł�" << endl;
		return Command();
	}

};
