#pragma once

#include "Simulator.hpp"

class Evaluation {
public:

	Evaluation() = default;
	Evaluation(const StageArray& stage, const int score, const int obstacle, const int turn) {

		setTopBlock(stage);
		evaluationBlockFlat(stage);
		searchChain(stage, score, obstacle, turn);

		totalScore += chainNumber * 1000 + chainScore;

		totalScore -= blockFlatScore * 1000;
		//totalScore -= (chainNumberTrigger + chainScoreTrigger) * 10;

	}

	const bool operator<(const Evaluation& e) const { return totalScore < e.totalScore; }

	void show() const {
		cerr << "連鎖:" << chainNumber << ",スコア:" << chainScore << endl;
	}

private:

	array<int, StageWidth> blockTop;

	int chainNumber = 0;
	int chainNumberTrigger = StageHeight;
	int chainScore = 0;
	int chainScoreTrigger = StageHeight;

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

		const int sub = 6;

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

		Simulator simulator;

		const auto blockContainPacks = Share::getBlockContainPacks();

		const auto nearEval = [](const int range) {
			const double x = (range - 1) / 3.3;
			const double r = exp(-(x*x) / 2);
			return r;
		};

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

					score = int(score*nearEval(first));
					chain = int(chain*nearEval(first));

					chainScore = max(chainScore, score);
					chainNumber = max(chainNumber, chain);
				}
			}
		}

	}

};
