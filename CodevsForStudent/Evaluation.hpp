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

		searchBlockNumber(_stage);
		searchAdjacentBlock(_stage);
		searchLinkNumber(_stage);

		evaluationBlockFlat(_stage);
		evaluationShapeError(_stage);

		searchChain(_stage);
		searchTrashNumber();

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

		searchBlockNumber(_stage);
		searchAdjacentBlock(_stage);
		searchLinkNumber(_stage);

		evaluationBlockFlat(_stage);
		evaluationShapeError(_stage);

		searchChain(_stage);
		searchTrashNumber();

		setShotThreshold(_enemy);

		setTotalScore();

		return totalScore;
	}

	void set(const int _score) { score = _score; }
	const int get() const { return score; }

	void show() const {
		cerr << "スコア\t\t:" << score << endl << endl;

		cerr << "お邪魔配置スコア\t:" << obstacleHeightSum << endl << endl;

		cerr << "ブロック数\t\t:" << blockNumber << endl;
		cerr << "結合スコア\t\t:" << blockLink << endl;
		cerr << "5以下の結合\t\t:" << lowerEqualNumber << endl;
		cerr << "6以上の結合\t\t:" << upperEqualNumber << endl << endl;

		cerr << "凹凸スコア\t\t:" << blockFlatScore << endl;
		cerr << "ブロックの高さ平均\t:" << blockHeightAverage << endl;
		cerr << "U字形状スコア\t\t:" << shapeError << endl;
		cerr << "発火位置\t\t:" << triggerHeight << endl << endl;

		cerr << "お邪魔ブロック数\t:" << obstacleNumber << endl << endl;

		cerr << "想定連鎖数\t\t:" << chainNumber << endl;
		cerr << "想定連鎖スコア\t\t:" << chainScore << endl;
		cerr << "発火連鎖スコア\t\t:" << attackScore << endl << endl;

		cerr << "発火閾値\t\t:" << shotThreshold << endl;
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
	//現在のスコア
	int score = 0;

	//ステージ上のお邪魔ブロックの高さの総和
	int obstacleHeightSum = 0;

	//ブロックの数
	int blockNumber = 0;
	//10未満で繋がったブロック数
	int blockLink = 0;
	//5以下の同じ数字が隣り合っている数
	int lowerEqualNumber;
	//6以上の同じ数字が隣り合っている数
	int upperEqualNumber;

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

	//組まれている連鎖の数
	int chainNumber = 0;
	//組まれている連鎖のスコア
	int chainScore = 0;
	//発火した時のスコア
	int attackScore = 0;

	//最高連鎖発火後のステージ
	StageArray chinShotStage;

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

	//ブロックの数をカウントする
	void searchBlockNumber(const StageArray& stage) {
		blockNumber = 0;

		for (int x = 0; x < stage.getWidth(); x++)
		{
			for (int y = 0; y < blockTop[x]; y++)
			{
				if (stage[stage.getHeight() - y - 1][x] != ObstacleBlock)
					blockNumber++;
			}
		}

	}
	//10未満で繋がったブロック数を得る
	void searchLinkNumber(const StageArray& stage) {

		blockLink = 0;

		//上, 右上, 右, 右下
		const Point direction[] = { Point(0,-1),Point(1,-1),Point(1,0),Point(1,1) };

		for (int x = 0; x < stage.getWidth(); x++)
		{
			for (int y = stage.getHeight() - 1; y >= 0 && stage[y][x] != EmptyBlock; y--)
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
	//隣り合っている数字を調べる
	void searchAdjacentBlock(const StageArray& stage) {

		lowerEqualNumber = 0;
		upperEqualNumber = 0;

		const Point direction[] = { Point(-1,-1),Point(0,-1),Point(1,-1),Point(-1,0),Point(1,0),Point(-1,1),Point(0,1),Point(1,1) };

		for (int x = 0; x < stage.getWidth(); x++)
		{
			for (int y = 0; y < blockTop[x]; y++)
			{
				const Point pos(x, stage.getHeight() - y - 1);

				for (const auto& dire : direction)
				{
					const Point p = pos + dire;
					if (inside(p) && stage[p] == stage[pos])
					{
						if (stage[pos] > 5) upperEqualNumber++;
						else lowerEqualNumber++;
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

		chainNumber = 0;
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

		int count = 0;

		//連鎖の検索
		for (int x = 0; x < (int)blockTop.size(); x++)
		{
			const int lh = x - 1 < 0 ? 0 : blockTop[x - 1];
			const int rh = x + 1 >= (int)blockTop.size() ? 0 : blockTop[x + 1];

			const int subh = max(1, max(lh, rh) - blockTop[x]);

			for (int y = 0; y < subh; y++)
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
						const int chain = simulator.next(checkStage, s);

						const int nblockTurn = find_blockTurn(num);
						//const int turnSub = (nblockTurn - turn - 1) / 4;
						const double turnSub = (nblockTurn - turn - 1) / (s < 50 ? 12.0 : 4.0);

						s = (int)(s*exp(-turnSub));

						//chainNumber = max(chainNumber, chain);
						//chainScore = max(chainScore, s);
						//triggerHeight = max(triggerHeight, abs(blockTop[x] + y - blockHeightAverage));
						//count++;

						//*
						if (chainNumber < chain)
						{
							chainNumber = chain;
							chainScore = s;
							triggerHeight = abs(blockTop[x] + y - blockHeightAverage);
							chinShotStage = move(checkStage);
							triggerBlock = num;
							triggerPoint = point;
						}
						else if (chainNumber == chain && chainScore < s)
						{
							chainNumber = chain;
							chainScore = s;
							triggerHeight = abs(blockTop[x] + y - blockHeightAverage);
							chinShotStage = move(checkStage);
							triggerBlock = num;
							triggerPoint = point;
						}
						//*/
					}

				}
			}
		}

		//chainNumber /= count;
		//chainScore /= count;
		//triggerHeight /= count;

	}

	void searchTrashNumber() {

		for (int x = 0; x < chinShotStage.getWidth(); x++)
		{
			for (int y = 0; y < blockTop[x]; y++)
			{
				const Point pos(x, chinShotStage.getHeight() - y - 1);
				if (chinShotStage[pos] != ObstacleBlock)
					trashNumber++;
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

		//totalScore += blockNumber;
		totalScore += blockLink;
		totalScore -= lowerEqualNumber * 1;
		totalScore -= upperEqualNumber * 10;

		totalScore -= blockFlatScore * 300;
		totalScore -= shapeError * 10;
		totalScore -= triggerHeight * 300;

		totalScore -= obstacleNumber * 10;

		totalScore += chainNumber * 1000;

		totalScore += chainScore * 10;
		totalScore += attackScore < shotThreshold ? -attackScore : attackScore * 20;
		//totalScore -= attackScore <= 10 ? attackScore * 10 : 0;

		//totalScore -= chainScore < 250 ? 0 : trashNumber * 3;

		totalScore += score * 10;

	}

};
