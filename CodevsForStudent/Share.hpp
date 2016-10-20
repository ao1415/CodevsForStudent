#pragma once

#include "Utility.hpp"
#include "Grid.hpp"
#include "CGrid.hpp"
#include "Pack.hpp"

class Input;

using StageArray = CGrid<int, StageWidth, StageHeight + 3>;

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

	friend Input;

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

};
