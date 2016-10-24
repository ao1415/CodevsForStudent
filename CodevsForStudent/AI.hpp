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
