#pragma once

#include "Share.hpp"

class Game {
public:

	Game() = default;
	Game(const Game& g) = default;
	Game(const StageArray& _stage, const int _obstacle) : stage(_stage), obstacle(_obstacle) {}
	Game(StageArray&& _stage, const int _obstacle) : obstacle(_obstacle) { stage = move(_stage); }

	void update(const int pos, const int rota, const Pack& pack) {

		dead = false;

		changedClear();

		for (int x = 0; x < StageWidth; x++)
		{
			for (int y = stage.getHeight() - 1; y >= 0; y--)
			{
				if (stage[y][x] == EmptyBlock)
				{
					blockTop[x] = y;
					break;
				}
			}
		}

		setPack(pos, rota, pack);

		score = 0;
		chain = 1;
		int count = 0;
		while ((count = disBlocks()) > 0)
		{
			score += (int)pow(1.3, chain)*(int)(count / 2);
			chain++;
			fallBlocks();
		}

		dead = false;
		for (int y = 0; y < 3; y++)
		{
			for (int x = 0; x < StageWidth; x++)
			{
				if (stage[y][x] != EmptyBlock)
				{
					dead = true;
					x = StageWidth;
					y = 3;
				}
			}
		}

	}

	const int getScore() const { return score; }
	const int getChain() const { return chain; }
	const int getObstacle() const { return obstacle; }

	const StageArray& getStage() const { return stage; }

	const bool isDead() const { return dead; }

private:

	StageArray stage;
	int obstacle = 0;

	int chain = 0;
	int score = 0;

	bool dead = false;

	array<int, StageWidth> blockTop;

	//水平
	array<bool, StageHeight + 3> verticalChanged;
	//垂直
	array<bool, StageWidth> horizontalChanged;
	//斜め下
	array<bool, StageWidth + StageHeight + 3 - 1> diagonalChanged1;
	//斜め上
	array<bool, StageWidth + StageHeight + 3 - 1> diagonalChanged2;

	void setPack(const int pos, const int rota, const Pack& pack) {

		const PackArray packArr = pack.getFullObstacle(obstacle).getArray(rota);

		for (int x = 0; x < PackSize; x++)
		{
			if (0 <= pos + x && pos + x < StageWidth)
			{
				int index = blockTop[pos + x];
				for (int y = PackSize - 1; y >= 0; y--)
				{
					if (packArr[y][x] != EmptyBlock)
					{
						stage[index][pos + x] = packArr[y][x];
						setChanged(pos + x, index);
						index--;
					}
				}
				blockTop[pos + x] = index;
			}
		}

	}

	void fallBlocks() {

		verticalChanged.fill(false);
		diagonalChanged1.fill(false);
		diagonalChanged2.fill(false);

		for (int x = 0; x < StageWidth; x++)
		{
			if (horizontalChanged[x])
			{
				int index = stage.getHeight() - 1;
				for (int y = stage.getHeight() - 1; y >= 0; y--)
				{
					if (stage[y][x] != EmptyBlock)
					{
						stage[index][x] = stage[y][x];
						if (index != y)
						{
							stage[y][x] = EmptyBlock;
							setChanged(x, index);
						}
						index--;
					}
				}
				blockTop[x] = index;
			}
		}

	}

	const int disBlocks() {

		array<bitset<StageWidth>, StageHeight + 3> disFlag;
		for (auto& b : disFlag) b.reset();

		int count = 0;

		//横のブロック探査
		const auto disV = [&]() {

			for (int y = 0; y < stage.getHeight(); y++)
			{
				if (verticalChanged[y])
				{
					for (int x = 0; x < StageWidth; x++)
					{
						if (stage[y][x] != EmptyBlock && stage[y][x] != ObstacleBlock)
						{
							int add = stage[y][x];
							int dx = 1;
							while (add < AddScore)
							{
								if (x + dx < StageWidth)
								{
									if (stage[y][x + dx] == EmptyBlock || stage[y][x + dx] == ObstacleBlock)
										break;
									add += stage[y][x + dx];
								}
								else
									break;
								dx++;
							}
							if (add == AddScore)
							{
								count += dx;
								for (int i = 0; i < dx; i++)
									disFlag[y][x + i] = true;
							}
						}
					}
				}
			}

		};

		//縦のブロック探査
		const auto disH = [&]() {

			for (int x = 0; x < StageWidth; x++)
			{
				if (horizontalChanged[x])
				{
					for (int y = stage.getHeight() - 1; y > blockTop[x]; y--)
					{
						if (stage[y][x] != ObstacleBlock)
						{
							int add = stage[y][x];
							int dy = 1;
							while (add < AddScore)
							{
								if (y - dy > blockTop[x])
								{
									if (stage[y - dy][x] == EmptyBlock || stage[y - dy][x] == ObstacleBlock)
										break;
									add += stage[y - dy][x];
								}
								else
									break;
								dy++;
							}
							if (add == AddScore)
							{
								count += dy;
								for (int i = 0; i < dy; i++)
									disFlag[y - i][x] = true;
							}
						}
					}
				}
			}

		};

		//斜め1のブロック探査
		const auto disD1 = [&]() {

			for (int i = 0; i < (int)diagonalChanged1.size(); i++)
			{
				if (diagonalChanged1[i])
				{
					Point pos(0, i - StageWidth + 1);
					for (int j = 0; j < StageWidth; j++)
					{
						if (inside(pos))
						{
							if (stage[pos] != EmptyBlock && stage[pos] != ObstacleBlock)
							{
								int add = stage[pos];
								int d = 1;
								Point p = pos + Point(1, 1);
								while (add < AddScore)
								{
									if (inside(p))
									{
										if (stage[p] == EmptyBlock || stage[p] == ObstacleBlock)
											break;
										add += stage[p];
									}
									else
										break;
									p += Point(1, 1);
									d++;
								}
								if (add == AddScore)
								{
									count += d;
									p = pos;
									for (int i = 0; i < d; i++)
									{
										disFlag[p.y][p.x] = true;
										p += Point(1, 1);
									}
								}
							}
						}
						pos += Point(1, 1);
					}
				}
			}

		};

		//斜め2のブロック探査
		const auto disD2 = [&]() {

			for (int i = 0; i < (int)diagonalChanged2.size(); i++)
			{
				if (diagonalChanged2[i])
				{
					Point pos(0, i);
					for (int j = 0; j < StageWidth; j++)
					{
						if (inside(pos))
						{
							if (stage[pos] != EmptyBlock && stage[pos] != ObstacleBlock)
							{
								int add = stage[pos];
								int d = 1;
								Point p = pos + Point(1, -1);
								while (add < AddScore)
								{
									if (inside(p))
									{
										if (stage[p] == EmptyBlock || stage[p] == ObstacleBlock)
											break;
										add += stage[p];
									}
									else
										break;
									p += Point(1, -1);
									d++;
								}
								if (add == AddScore)
								{
									count += d;
									p = pos;
									for (int i = 0; i < d; i++)
									{
										disFlag[p.y][p.x] = true;
										p += Point(1, -1);
									}
								}
							}
						}
						pos += Point(1, -1);
					}
				}
			}

		};

		disV();
		disH();
		disD1();
		disD2();

		horizontalChanged.fill(false);
		for (int y = 0; y < (int)disFlag.size(); y++)
		{
			if (disFlag[y].any())
			{
				for (int x = 0; x < StageWidth; x++)
				{
					if (disFlag[y][x])
					{
						stage[y][x] = EmptyBlock;
						horizontalChanged[x] = true;
					}
				}
			}
		}

		return count;
	}

	void changedClear() {
		verticalChanged.fill(false);
		horizontalChanged.fill(false);
		diagonalChanged1.fill(false);
		diagonalChanged2.fill(false);
	}

	void setChanged(const int x, const int y) {
		verticalChanged[y] = true;
		horizontalChanged[x] = true;
		diagonalChanged1[y + (StageWidth - x - 1)] = true;
		diagonalChanged2[y + x] = true;
	}

};
