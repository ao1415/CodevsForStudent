#pragma once

#include "Simulator.hpp"

class Evaluation {
public:

	const int getScore(const StageArray& _stage, const int _obstacle, const int _attackScore, const int turn) {
		obstacle = _obstacle;
		attackScore = _attackScore;

	}

	void show() const {
		cerr << "お邪魔配置スコア\t:" << obstacleScore << endl;
		cerr << "結合スコア\t\t:" << linkScore << endl << endl;

		cerr << "凹凸スコア\t\t:" << flatScore << endl;
		cerr << "ブロックの高さ平均\t:" << average << endl;
		cerr << "U字形状スコア\t\t:" << heightError << endl;
		cerr << "発火位置\t\t:" << triggerHeight << endl << endl;

		cerr << "お邪魔ブロック数\t:" << obstacle << endl << endl;

		cerr << "想定連鎖スコア\t\t:" << maxScore << endl;
		cerr << "発火連鎖スコア\t\t:" << attackScore << endl << endl;

		cerr << "連鎖尾スコア\t\t:" << trashScore << endl << endl;

		//cerr << "総合スコア\t\t:" << score << endl;
		cerr << waitPoint << ", wait:" << waitNumber << endl;
	}

private:

	//ステージ上のお邪魔ブロックの高さの総和
	int obstacleScore = 0;
	//10未満で繋がったブロック数
	int linkScore = 0;

	//ブロックの山谷の数
	int flatScore = 0;
	//ブロックの高さの平均
	int average = 0;
	//ブロックの形状
	int heightError = 0;

	//発火点の高さ
	int triggerHeight = 0;

	//お邪魔ブロックの数
	int obstacle = 0;

	//組まれている連鎖のスコア
	int maxScore = 0;
	//発火した時のスコア
	int attackScore = 0;

	//ごみの数
	int trashScore = 0;

	//発火するブロック
	int waitNumber = 0;
	//発火する座標
	Point waitPoint;

	//詰みあがったブロックの数
	array<int, StageWidth> blockTop;

	//ブロックの天辺の検索
	//「下から」の高さを得る
	void searchBlockTop(const StageArray& stage) {

		blockTop.fill(0);

		for (int x = 0; x < stage.getWidth(); x++)
		{
			for (int y = stage.getHeight() - 1; y >= 0; y--)
			{
				if (stage[y][x] == EmptyBlock)
				{
					blockTop[x] = stage.getHeight() - y - 1;
					break;
				}
			}
		}

		average = 0;
		for (const auto& v : blockTop) average += v;
		average /= (int)blockTop.size();
	}
	//お邪魔ブロックの高さの総和を得る
	void searchObstacleBlock(const StageArray& stage) {

		obstacleScore = 0;

		for (int x = 0; x < stage.getWidth(); x++)
		{
			for (int y = stage.getHeight() - 1; y >= 0; y--)
			{
				if (stage[y][x] == ObstacleBlock)
					obstacleScore += stage.getHeight() - y;//下からの高さ
				else if (stage[y][x] == EmptyBlock)
					break;
			}
		}
	}
	//10未満で繋がったブロック数を得る
	void searchLink(const StageArray& stage) {

		linkScore = 0;

		//右, 左下, 下, 右下
		const Point direction[] = { Point(1,0),Point(-1,1),Point(0,1),Point(1,1) };

		for (int y = PackSize; y < stage.getHeight(); y++)
		{
			for (int x = 0; x < stage.getWidth(); x++)
			{
				//ブロックの10以下の連結の検索
				for (const auto& dire : direction)
				{
					int add = stage[y][x];
					Point p = Point(x, y);

					//探査
					while (true)
					{
						p += dire;

						if (!inside(p) || stage[p] == EmptyBlock) break;

						add += stage[p];

						if (add < AddScore)
							linkScore++;
						else
							break;
					}
				}
			}
		}

	}

	//ブロックの凹凸を評価する
	//1ブロックの山谷の数を得る
	void evaluationFlat(const StageArray& stage) {

		flatScore = 0;

		const int sub = 4;

		for (int x = 0; x < (int)blockTop.size(); x++)
		{
			const int l = ((x - 1 >= 0) ? blockTop[x - 1] : StageHeight);
			const int r = ((x + 1 < (int)blockTop.size()) ? blockTop[x + 1] : StageHeight);

			const int lsub = blockTop[x] - l;
			const int rsub = blockTop[x] - r;

			if (lsub >= sub && rsub >= sub)
				flatScore++;

			if (lsub <= -sub && rsub <= -sub)
				flatScore++;

		}
	}
	//形がU字型に似ているかを評価する
	void evaluationShape(const StageArray& stage) {

		heightError = 0;

		for (int x = 0; x < (int)blockTop.size(); x++)
		{
			const int h = ((x - StageWidth / 2)*(x - StageWidth / 2) / 5) + (average - 2);
			const int sub = h - blockTop[x];
			heightError += sub*sub;
		}

		heightError = int(sqrt(heightError));

	}

	//最大連鎖数を得る
	//ここがメイン評価になる
	void searchChain(const StageArray& stage) {

		maxScore = 0;
		triggerHeight = 0;

		Simulator simulator;

		//左上・右上・左・右・左下・下・右下
		const Point direction[] = { Point(-1,-1),Point(1,-1),Point(-1,0),Point(1,0),Point(-1,1),Point(0,1),Point(1,1) };

		//連鎖の検索
		StageArray maxStage;
		for (int x = 0; x < (int)blockTop.size(); x++)
		{
			//for (int y = 0; y < PackSize; y++)
			for (int y = 0; y < PackSize; y++)
			{
				const Point point = Point(x, stage.getHeight() - blockTop[x] - 1 - y);
				for (const auto& dire : direction)
				{
					Point p = point;
					int num = AddScore;

					for (int i = 1; i < StageHeight; i++)
					{
						p += dire;
						if (!inside(p) || stage[p] == EmptyBlock) break;

						num -= stage[p];
						if (num <= 0) break;

						int s;
						StageArray checkStage = stage;
						checkStage[point] = num;
						simulator.next(checkStage, s);
						if (maxScore < s)
						{
							maxScore = s;
							triggerHeight = abs(blockTop[x] + y - average);
							maxStage = move(checkStage);
							waitNumber = num;
							waitPoint = point;
						}
					}

				}
			}
		}

	}

};
