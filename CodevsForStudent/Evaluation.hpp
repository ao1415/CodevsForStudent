#pragma once

#include "Simulator.hpp"

class Evaluation {
public:

	Evaluation() = default;
	Evaluation(const StageArray& stage, const int score, const int obstacle, const int turn) {

		setTopBlock(stage);
		evaluationBlockFlat(stage);
		searchChain(stage, score, obstacle, turn);

		totalScore += chainNumber * 100 + chainScore;
		totalScore -= blockFlatScore * 600;
	}

	const bool operator<(const Evaluation& e) const { return totalScore < e.totalScore; }

	void show() const {
		cerr << "連鎖:" << tChain << ",スコア:" << tScore << endl;
	}

	const int getScore() const { return tScore; }

private:

	array<int, StageWidth> blockTop;

	int chainNumber = 0;
	int chainScore = 0;
	int blockFlatScore = 0;

	int tChain = 0;
	int tScore = 0;
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

			const int lsub = l - blockTop[x];
			const int rsub = r - blockTop[x];

			//谷
			if (lsub > sub && rsub > sub)
				blockFlatScore += max(lsub, rsub) - sub;

			//山
			if (lsub < -sub && rsub < -sub)
				blockFlatScore += -min(lsub, rsub) - sub;

		}
	}

	void searchChain(const StageArray& stage, const int score, const int obstacle, const int turn) {


	}

};
