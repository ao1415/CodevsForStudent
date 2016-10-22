#pragma once

#include "Simulator.hpp"

class Evaluation {
public:

	const int getScore(const StageArray& _stage, const int _obstacleNumber, const int _attackScore, const int _turn) {
		turn = _turn;
		obstacleNumber = _obstacleNumber;
		attackScore = _attackScore;

		searchBlockHeight(_stage);
		addObstacleHeight(_stage);
		searchLinkNumber(_stage);

		evaluationBlockFlat(_stage);
		evaluationShapeError(_stage);

		searchChain(_stage);

		setShotThreshold();

		setTotalScore();

		return totalScore;
	}
	const int getScore(const StageArray& _stage, const int _obstacleNumber, const int _attackScore, const int _turn, const Evaluation& _enemy) {
		turn = _turn;
		obstacleNumber = _obstacleNumber;
		attackScore = _attackScore;

		searchBlockHeight(_stage);
		addObstacleHeight(_stage);
		searchLinkNumber(_stage);

		evaluationBlockFlat(_stage);
		evaluationShapeError(_stage);

		searchChain(_stage);

		setShotThreshold(_enemy);

		setTotalScore();

		return totalScore;
	}

	void show() const {
		cerr << "お邪魔配置スコア\t:" << obstacleHeightSum << endl;
		cerr << "結合スコア\t\t:" << blockLink << endl << endl;

		cerr << "凹凸スコア\t\t:" << blockFlatScore << endl;
		cerr << "ブロックの高さ平均\t:" << blockHeightAverage << endl;
		cerr << "U字形状スコア\t\t:" << shapeError << endl;
		cerr << "発火位置\t\t:" << triggerHeight << endl << endl;

		cerr << "お邪魔ブロック数\t:" << obstacleNumber << endl << endl;

		cerr << "想定連鎖スコア\t\t:" << chainScore << endl;
		cerr << "発火連鎖スコア\t\t:" << attackScore << endl << endl;

		cerr << "連鎖尾スコア\t\t:" << trashNumber << endl << endl;

		cerr << "総合スコア\t\t:" << totalScore << endl;
		cerr << triggerPoint << ", wait:" << triggerBlock << endl;
	}

	void showTotalScore() const {
		cerr << "総合スコア:" << totalScore << endl;
	}

	void showAttackScore() const {
		cerr << "チェインスコア:" << attackScore << endl;
	}

	const int getAttackScore() const { return attackScore; }

private:

	//現在のターン数
	int turn = 0;

	//ステージ上のお邪魔ブロックの高さの総和
	int obstacleHeightSum = 0;
	//10未満で繋がったブロック数
	int blockLink = 0;

	//ブロックの山谷の数
	int blockFlatScore = 0;
	//ブロックの高さの平均
	int blockHeightAverage = 0;
	//ブロックの形状誤差
	int shapeError = 0;

	//発火点の高さ
	int triggerHeight = 0;

	//お邪魔ブロックの数
	int obstacleNumber = 0;

	//組まれている連鎖のスコア
	int chainScore = 0;
	//発火した時のスコア
	int attackScore = 0;

	//発火する閾値
	int shotThreshold = 0;

	//ごみの数
	int trashNumber = 0;

	//総合スコア
	int totalScore = 0;
	//発火するブロック
	int triggerBlock = 0;
	//発火する座標
	Point triggerPoint;

	//詰みあがったブロックの数
	array<int, StageWidth> blockTop;

	//ブロックの天辺の検索
	//「下から」の高さを得る
	void searchBlockHeight(const StageArray& stage) {

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

		blockHeightAverage = 0;
		for (const auto& v : blockTop) blockHeightAverage += v;
		blockHeightAverage /= (int)blockTop.size();
	}
	//お邪魔ブロックの高さの総和を得る
	void addObstacleHeight(const StageArray& stage) {

		obstacleHeightSum = 0;

		for (int x = 0; x < stage.getWidth(); x++)
		{
			for (int y = stage.getHeight() - 1; y >= 0; y--)
			{
				if (stage[y][x] == ObstacleBlock)
					obstacleHeightSum += stage.getHeight() - y;//下からの高さ
				else if (stage[y][x] == EmptyBlock)
					break;
			}
		}
	}
	//10未満で繋がったブロック数を得る
	void searchLinkNumber(const StageArray& stage) {

		blockLink = 0;

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
							blockLink++;
						else
							break;
					}
				}
			}
		}

	}

	//ブロックの凹凸を評価する
	//1ブロックの山谷の数を得る
	void evaluationBlockFlat(const StageArray& stage) {

		blockFlatScore = 0;

		const int sub = 4;

		for (int x = 0; x < (int)blockTop.size(); x++)
		{
			const int l = ((x - 1 >= 0) ? blockTop[x - 1] : StageHeight);
			const int r = ((x + 1 < (int)blockTop.size()) ? blockTop[x + 1] : StageHeight);

			const int lsub = blockTop[x] - l;
			const int rsub = blockTop[x] - r;

			if (lsub >= sub && rsub >= sub)
				blockFlatScore += max(lsub, rsub) - sub + 1;

			if (lsub <= -sub && rsub <= -sub)
				blockFlatScore += -min(lsub, rsub) - sub + 1;

		}
	}
	//形がU字型に似ているかを評価する
	void evaluationShapeError(const StageArray& stage) {

		shapeError = 0;

		for (int x = 0; x < (int)blockTop.size(); x++)
		{
			const int h = ((x - StageWidth / 2)*(x - StageWidth / 2) / 5) + (blockHeightAverage - 2);
			const int sub = h - blockTop[x];
			shapeError += sub*sub;
		}

		shapeError = int(sqrt(shapeError));

	}

	//最大連鎖数を得る
	//ここがメイン評価になる
	void searchChain(const StageArray& stage) {

		chainScore = 0;
		triggerHeight = 0;

		Simulator simulator;

		const auto& blockContainPacks = Share::getBlockContainPacks();
		const auto find_blockTurn = [&](const int num) {
			for (const auto& val : blockContainPacks[num])
				if (val > turn) return val;
			return 1000;
		};

		//左上・右上・左・右・左下・下・右下
		const Point direction[] = { Point(-1,-1),Point(1,-1),Point(-1,0),Point(1,0),Point(-1,1),Point(0,1),Point(1,1) };

		//連鎖の検索
		StageArray maxStage;
		for (int x = 0; x < (int)blockTop.size(); x++)
		{
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

						for (int i = 0; i < y; i++)
							checkStage[stage.getHeight() - blockTop[x] - 1 - i][x] = ObstacleBlock;

						checkStage[point] = num;
						simulator.next(checkStage, s);

						const int nblockTurn = find_blockTurn(num);
						//const int turnSub = (nblockTurn - turn - 1) / 4;
						const double turnSub = (nblockTurn - turn - 1) / (s < 50 ? 12.0 : 4.0);

						s = (int)(s*exp(-turnSub));

						if (chainScore < s)
						{
							chainScore = s;
							triggerHeight = abs(blockTop[x] + y - blockHeightAverage);
							maxStage = move(checkStage);
							triggerBlock = num;
							triggerPoint = point;
						}
					}

				}
			}
		}

	}

	void setShotThreshold() {

		const int sendBlock = attackScore / 5;

		shotThreshold = 250;

		if (Share::getEnFreeSpace() < sendBlock)
			shotThreshold = 0;

		if (obstacleNumber > 0 && sendBlock > obstacleNumber)
			shotThreshold = 0;

	}
	void setShotThreshold(const Evaluation& enemy) {

		const auto& blockContainPacks = Share::getBlockContainPacks();
		const auto find_blockTurn = [&](const int num) {
			for (const auto& val : blockContainPacks[num])
				if (val > turn) return val;
			return 1000;
		};

		const int sendBlock = attackScore / 5;

		shotThreshold = 250;

		if (Share::getEnFreeSpace() < sendBlock)
		{
			if (enemy.chainScore < chainScore)
			{
				shotThreshold = 0;
				return;
			}
		}

		if (obstacleNumber > 0 && sendBlock > obstacleNumber)
		{
			shotThreshold = 0;
			return;
		}

		const int nblockTurn = find_blockTurn(enemy.triggerBlock);
		const double turnSub = nblockTurn - turn;

		if (turnSub == 1 && sendBlock > 10)
		{
			shotThreshold = 0;
			return;
		}

	}


	void setTotalScore() {

		totalScore -= obstacleHeightSum;
		totalScore += blockLink;

		totalScore -= blockFlatScore * 1000;
		totalScore -= shapeError * 5;
		totalScore -= triggerHeight * 100;

		totalScore -= obstacleNumber * 10;

		totalScore += chainScore * 10;

		//相手と自分の状況をみて閾値を決める

		totalScore += attackScore < shotThreshold ? attackScore * 10 : attackScore * 20;

		//totalScore -= chainScore < 250 ? 0 : trashNumber * 3;

	}

};
