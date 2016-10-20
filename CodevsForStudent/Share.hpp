#pragma once

#include "Utility.hpp"
#include "Grid.hpp"
#include "CGrid.hpp"
#include "Pack.hpp"

class Input;

using StageArray = CGrid<int, StageWidth, StageHeight + 3>;

class Share {
public:

	/// <summary>�ő�^�[������Ԃ�(500)</summary>
	inline static const int getTurn() { return turn; }
	/// <summary>�p�b�N�̔z���Ԃ�</summary>
	inline static const std::vector<Pack>& getPacks() { return packs; }

	/// <summary>���݂̃^�[������Ԃ�</summary>
	inline static const int getNow() { return now; }
	/// <summary>�c�莞�Ԃ�Ԃ�</summary>
	inline static const int getTime() { return time; }
	/// <summary>�����̂��ז��u���b�N����Ԃ�</summary>
	inline static const int getMyObstacle() { return myObstacle; }
	/// <summary>�G�̂��ז��u���b�N����Ԃ�</summary>
	inline static const int getEnObstacle() { return enObstacle; }

	/// <summary>�����̃X�e�[�W�󋵂�Ԃ�</summary>
	inline static const StageArray& getMyStage() { return myStage; }
	/// <summary>�G�̃X�e�[�W�󋵂�Ԃ�</summary>
	inline static const StageArray& getEnStage() { return enStage; }

	/// <summary>�������ǂ̃p�b�N�Ɋ܂܂�Ă��邩�̔z���Ԃ�</summary>
	inline static const array<vector<int>, 10>& getBlockContainPacks() { return blockContainPacks; }
	/// <summary>�w�肳�ꂽ�������ǂ̃p�b�N�Ɋ܂܂�Ă��邩�̔z���Ԃ�</summary>
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
