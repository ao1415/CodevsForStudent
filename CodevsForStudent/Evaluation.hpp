#pragma once

#include "Simulator.hpp"

class Evaluation {
public:

	Evaluation() = default;
	Evaluation(const StageArray& stage, const int score, const int obstacle) {

		setTopBlock(stage);
		evaluationBlockFlat(stage);
		searchChain(stage, score, obstacle);

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

	void searchChain(const StageArray& stage, const int score, const int obstacle) {

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

		Simulator simulator;

		for (int n = 1; n < AddScore; n++)
		{
			for (int x = 0; x < stage.getWidth(); x++)
			{
				for (int dy = 0; dy < 3; dy++)
				{
					const Point point(x, blockTop[x] - dy);

					if (deleteCheck(point, stage, n))
					{
						auto next = stage;
						next[point] = n;
						int score;
						const int chain = simulator.next(next, score);

						if (score > chainScore || (score == chainScore && point.y < chainScoreTrigger))
						{
							chainScore = score;
							chainScoreTrigger = point.y;
						}
						if (chain > chainNumber || (chain == chainNumber&& point.y < chainNumberTrigger))
						{
							chainNumber = chain;
							chainNumberTrigger = point.y;
						}

						//chainScore = max(chainScore, score);
						//chainNumber = max(chainNumber, chain);
					}
				}
			}
		}

	}

};
