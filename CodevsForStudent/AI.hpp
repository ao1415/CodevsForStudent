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

		return "0 0";
	}

private:

	struct Command {
		int pos = 0;
		int rota = 0;
		const string toString() const { return to_string(pos) + " " + to_string(rota); }
	};

	struct Data {
		Command command;
	};

};
