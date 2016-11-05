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
		int score;

		const bool operator<(const Data& d) const { return evaluation < d.evaluation; }
	};

	Data lastData;

	const Data enemyThink(int obstacle) {

		return Data();
	}

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

	const vector<Data> attackThink() {

		return vector<Data>();
	}

	const Command chainThink(const vector<Data>& commands) {

		cerr << "詰みです" << endl;
		return Command();
	}

};
