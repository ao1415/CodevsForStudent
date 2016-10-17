#pragma once

#include "Utility.hpp"
#include "Grid.hpp"
#include "CGrid.hpp"

class Input;

using StageArray = CGrid<int, StageWidth, StageHeight + 3>;

class Share {
public:

	inline static const int getTurn() { return turn; }
	inline static const std::vector<Pack>& getPacks() { return packs; }

	inline static const int getNow() { return now; }
	inline static const int getTime() { return time; }
	inline static const int getMyObstacle() { return myObstacle; }
	inline static const int getEnObstacle() { return enObstacle; }

	inline static const StageArray& getMyStage() { return myStage; }
	inline static const StageArray& getEnStage() { return enStage; }

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

};
