#pragma once

#include "Utility.hpp"
#include "Grid.hpp"
#include "CGrid.hpp"
#include "Timer.hpp"

#include "Pack.hpp"

class Input;

using StageArray = CGrid<char, StageWidth, StageHeight + 3>;

class Share {
public:

	/// <summary>最大ターン数を返す(500)</summary>
	inline static const int getTurn() { return turn; }
	/// <summary>パックの配列を返す</summary>
	inline static const std::vector<Pack>& getPacks() { return packs; }

	/// <summary>現在のターン数を返す</summary>
	inline static const int getNow() { return now; }
	/// <summary>残り時間を返す</summary>
	inline static const int getTime() { return time; }
	/// <summary>自分のお邪魔ブロック数を返す</summary>
	inline static const int getMyObstacle() { return myObstacle; }
	/// <summary>敵のお邪魔ブロック数を返す</summary>
	inline static const int getEnObstacle() { return enObstacle; }

	/// <summary>自分のステージ状況を返す</summary>
	inline static const StageArray& getMyStage() { return myStage; }
	/// <summary>敵のステージ状況を返す</summary>
	inline static const StageArray& getEnStage() { return enStage; }

	/// <summary>数字がどのパックに含まれているかの配列を返す</summary>
	inline static const array<vector<int>, 10>& getBlockContainPacks() { return blockContainPacks; }
	/// <summary>指定された数字がどのパックに含まれているかの配列を返す</summary>
	inline static const vector<int>& getBlockContainPacks(const size_t n) { return blockContainPacks[n]; }

	/// <summary>自分のステージの空白マスを返す</summary>
	inline static const int getMyFreeSpace() { return myFreeSpace; }
	/// <summary>敵のステージの空白マスを返す</summary>
	inline static const int getEnFreeSpace() { return enFreeSpace; }

	friend Input;

	static void update() {
		updateBlockContainPacks();
		updateFreeSpace();
	}

private:

	static int turn;
	static std::vector<Pack> packs;

	static int now;
	static int time;
	static int myObstacle;
	static int enObstacle;
	static StageArray myStage;
	static StageArray enStage;

	static array<vector<int>, 10> blockContainPacks;

	static int myFreeSpace;
	static int enFreeSpace;

	//vectorを前から消している
	static void updateBlockContainPacks() {

		for (size_t i = 0; i < blockContainPacks.size(); i++)
		{
			while (!blockContainPacks[i].empty())
			{
				if (blockContainPacks[i][0] >= now) break;
				blockContainPacks[i].erase(blockContainPacks[i].begin());
			}
		}

	}

	static void updateFreeSpace() {

		myFreeSpace = enFreeSpace = 0;

		for (int y = 0; y < myStage.getHeight(); y++)
		{
			for (int x = 0; x < myStage.getWidth(); x++)
			{
				if (myStage[y][x] == EmptyBlock) myFreeSpace++;
				if (enStage[y][x] == EmptyBlock) enFreeSpace++;
			}
		}

	}

};
