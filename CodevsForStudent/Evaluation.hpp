#pragma once

#include "Simulator.hpp"

class Evaluation {
public:

	Evaluation() = default;
	Evaluation(const StageArray& stage, const int score, const int obstacle) {

		searchChain(stage, score, obstacle);

		totalScore = chainNumber * 1000 + chainScore;

	}

	const bool operator<(const Evaluation& e) const { return totalScore < e.totalScore; }

	void show() const {
		cerr << "連鎖:" << chainNumber << ",スコア:" << chainScore << endl;
	}

private:

	int chainNumber = 0;
	int chainScore = 0;

	int totalScore = 0;

	void searchChain(const StageArray& stage, const int score, const int obstacle) {

		array<int, StageWidth> topBlock;
		topBlock.fill(stage.getHeight() - 1);

		for (int x = 0; x < stage.getWidth(); x++)
		{
			for (int y = stage.getHeight() - 1; y >= 0; y--)
			{
				if (stage[y][x] == EmptyBlock)
				{
					topBlock[x] = y;
					break;
				}
			}
		}

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
					const Point point(x, topBlock[x] - dy);

					if (deleteCheck(point, stage, n))
					{
						auto next = stage;
						next[point] = n;
						int score;
						const int chain = simulator.next(next, score);

						chainScore = max(chainScore, score);
						chainNumber = max(chainNumber, chain);
					}
				}
			}
		}

	}

};
