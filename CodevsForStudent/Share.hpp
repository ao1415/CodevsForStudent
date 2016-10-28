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

	/// <summary>�����̃X�e�[�W�̋󔒃}�X��Ԃ�</summary>
	inline static const int getMyFreeSpace() { return myFreeSpace; }
	/// <summary>�G�̃X�e�[�W�̋󔒃}�X��Ԃ�</summary>
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

	//vector��O��������Ă���
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
