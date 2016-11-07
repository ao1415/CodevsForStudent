#pragma once

#include "Simulator.hpp"

class Evaluation {
public:

	Evaluation() = default;
	Evaluation(const StageArray& stage, const int score, const int obstacle, const int turn, const int triggerTurn) {

		setTopBlock(stage);
		searchChain(stage, score, obstacle, turn);

		totalScore += chainNumber * 100 + chainScore;

		const auto nearEval = [&](const int range) {
			if (range == 0) { attackFlag = true; return 1.0; }
			if (range == 1) { attackFlag = true; return 0.8; }
			if (range == 2) { attackFlag = true; return 0.5; }
			return -2.0;
		};
		const int r = abs(triggerTurn - turn);

		totalScore += int(nearEval(r) * score * 50);

	}

	const bool operator<(const Evaluation& e) const { return totalScore < e.totalScore; }

	void show() const {
		cerr << "連鎖:" << tChain << ",スコア:" << tScore << endl;
	}

	const int getScore() const { return tScore; }
	const bool attack() const { return attackFlag; }

private:

	array<int, StageWidth> blockTop;

	int chainNumber = 0;
	int chainScore = 0;

	int tChain = 0;
	int tScore = 0;
	int totalScore = 0;

	bool attackFlag = false;

	void setTopBlock(const StageArray& stage) {

		blockTop.fill(stage.getHeight() - 1);

		for (int x = 0; x < stage.getWidth(); x++)
		{
			for (int y = stage.getHeight() - 1; y >= 0; y--)
			{
				if (stage[y][x] == EmptyBlock)
				{
					blockTop[x] = y;
					break;
				}
			}
		}

	}

	void searchChain(const StageArray& stage, const int score, const int obstacle, const int turn) {

		const auto deleteCheck = [](const Point& pos, const StageArray& stage, const int n) {

			const Point direction[] = { Point(-1,-1),Point(-1,1),Point(-1,0),Point(1,0),Point(1,-1),Point(0,-1),Point(1,-1) };
			for (const auto& dire : direction)
			{
				int add = n;
				Point p = pos;
				while (add < AddScore)
				{
					p += dire;
					if (inside(p) && stage[p] != EmptyBlock) add += stage[p];
					else break;
				}

				if (add == AddScore)
					return true;
			}

			return false;
		};

		const auto find_blockTurn = [](const int num, const array<vector<int>, 10>& blockContainPacks, const int turn) {
			for (const auto& val : blockContainPacks[num - 1])
				if (val > turn)
					return val;
			return 1000;
		};

		const auto nearEval = [](const int range) {
			const double e = exp(range - 10);
			const double r = 1 / (1 + e);
			return r;
		};

		Simulator simulator;

		const auto packs = Share::getPacks();
		const auto blockContainPacks = Share::getBlockContainPacks();

		for (int n = 1; n < AddScore; n++)
		{
			int first = find_blockTurn(n, blockContainPacks, turn) - turn;

			const double e = nearEval(first);

			for (int x = 0; x < stage.getWidth(); x++)
			{
				const Point point(x, blockTop[x]);

				if (first + turn < Share::getTurn())
				{
					if (deleteCheck(point, stage, n))
					{
						auto next = stage;

						next[point] = n;

						const auto data = simulator.next(next);
						int score = get<0>(data);
						int chain = get<1>(data);

						const int eScore = int(e*score);
						const int eChain = int(e*chain);

						if (eScore > chainScore)
						{
							tScore = score;
							chainScore = eScore;
						}
						if (eChain > chainNumber)
						{
							tChain = chain;
							chainNumber = eChain;
						}
					}
				}
			}
		}
	}

};
