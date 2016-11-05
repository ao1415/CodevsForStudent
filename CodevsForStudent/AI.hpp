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

	const Data enemyThink(int obstacle) {

		return Data();
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

		return vector<Data>();
	}

	const Command chainThink(const vector<Data>& commands) {

		cerr << "�l�݂ł�" << endl;
		return Command();
	}

};
