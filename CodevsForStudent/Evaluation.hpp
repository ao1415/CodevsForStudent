#pragma once

#include "Simulator.hpp"

class Evaluation {
public:

	Evaluation() = default;
	Evaluation(const StageArray& stage, const int score, const int obstacle, const int turn, const int triggerTurn) {

		setTopBlock(stage);
		//evaluationBlockFlat(stage);
		searchChain(stage, score, obstacle, turn);

		totalScore += chainNumber[0] * 100 + chainScore[0];
		totalScore += chainNumber[1] * 10 + chainScore[1] / 10;

		//totalScore -= blockFlatScore * 1000;

		const auto func = [](const int range) {
			switch (range)
			{
			case 0: return 50.0;
			case 1: return 40.0;
			case 2: return 25.0;
			default: return -10.0;
			}
		};

		totalScore += int(score*func(abs(triggerTurn - turn)));

	}

	const bool operator<(const Evaluation& e) const { return totalScore < e.totalScore; }

	void show() const {
		cerr << "�A��:" << chainNumber[0] << ",�X�R�A:" << chainScore[0] << endl;
	}

private:

	array<int, StageWidth> blockTop;

	array<int, 2> chainNumber = {};
	int chainNumberTrigger = StageHeight;
	int chainNumberTriggerRange = INT32_MAX;
	array<int, 2> chainScore = {};
	int chainScoreTrigger = StageHeight;
	int chainScoreTriggerRange = INT32_MAX;

	int blockFlatScore = 0;

	int totalScore = 0;

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

	void evaluationBlockFlat(const StageArray& stage) {

		const int sub = 8;

		for (int x = 0; x < (int)blockTop.size(); x++)
		{
			const int l = ((x - 1 >= 0) ? blockTop[x - 1] : 3);
			const int r = ((x + 1 < (int)blockTop.size()) ? blockTop[x + 1] : 3);

			const int lsub = blockTop[x] - l;
			const int rsub = blockTop[x] - r;

			if (lsub >= sub && rsub >= sub)
				blockFlatScore += max(lsub, rsub) - sub + 1;

			if (lsub <= -sub && rsub <= -sub)
				blockFlatScore += -min(lsub, rsub) - sub + 1;

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

		const auto blockContainPacks = Share::getBlockContainPacks();

		for (int n = 1; n < AddScore; n++)
		{
			int first = find_blockTurn(n, blockContainPacks, turn) - turn;

			for (int x = 0; x < stage.getWidth(); x++)
			{
				const Point point(x, blockTop[x]);

				if (deleteCheck(point, stage, n))
				{
					auto next = stage;

					next[point] = n;
					int score;
					int chain = simulator.next(next, score);
					const double e = nearEval(first);
					score = int(e*score);
					chain = int(e*chain);

					if (score > chainScore[0])
					{
						chainScore[1] = chainScore[0];
						chainScore[0] = score;
					}
					else if (score > chainScore[1])
					{
						chainScore[1] = score;
					}
					if (chain > chainNumber[0])
					{
						chainNumber[1] = chainNumber[0];
						chainNumber[0] = chain;
					}
					else if (chain > chainNumber[1])
					{
						chainNumber[1] = chain;
					}
				}
			}
		}

	}

};
