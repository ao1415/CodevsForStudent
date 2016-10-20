#pragma once

#include "Share.hpp"

class Input {
public:

	static void first() {
		int w, h, t, s;
		cin >> w >> h >> t >> s >> Share::turn;

		for (int i = 0; i < Share::turn; i++)
		{
			Share::packs.emplace_back(Pack::input());
			const auto& blocks = Share::packs.back().getArray(0);

			for (int y = 0; y < blocks.getHeight(); y++)
			{
				for (int x = 0; x < blocks.getWidth(); x++)
				{
					if (blocks[y][x] != EmptyBlock)
					{
						Share::blockContainPacks[blocks[y][x] - 1].push_back(i);
					}
				}
			}

		}
	}

	static bool loop() {
		string endStr;

		cin >> Share::now >> Share::time;

		cin >> Share::myObstacle;
		for (int y = 0; y < StageHeight; y++)
		{
			for (int x = 0; x < StageWidth; x++)
			{
				cin >> Share::myStage[y + 3][x];
			}
		}
		cin >> endStr;

		cin >> Share::enObstacle;
		for (int y = 0; y < StageHeight; y++)
		{
			for (int x = 0; x < StageWidth; x++)
			{
				cin >> Share::enStage[y + 3][x];
			}
		}
		cin >> endStr;

		return endStr == "END";
	}

};
