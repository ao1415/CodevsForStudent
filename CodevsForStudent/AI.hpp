#pragma once

#include "Evaluation.hpp"
#include "Simulator.hpp"
#include "Game.hpp"

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

		//�U�����f
		{
			const auto& myStage = Share::getMyStage();
			int myObstacle = Share::getMyObstacle();

			set<Hash::Type> hashSet;

			const auto& pack = packs[now];
			const auto& fullPack = packs[now].getFullObstacle(myObstacle);
			const auto& fullPackArr = fullPack.getArray();
			const auto& sides = fullPack.getSide();

			//�U������֐�
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
			cerr << "�I���΁I" << endl;
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

		cerr << "�l�݂ł�" << endl;
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
