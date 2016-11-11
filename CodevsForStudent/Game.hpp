#pragma once

#include "Share.hpp"

class Game {
public:

	Game() = default;
	Game(const Game& g) = default;
	Game(Game&& g) {
		stage = move(g.stage);
		obstacle = g.obstacle;
		chain = g.chain;
		score = g.score;
		dead = g.dead;
		g.obstacle = g.chain = g.score = 0;
		g.dead = false;
	}
	Game(const StageArray& _stage, const int _obstacle) : stage(_stage), obstacle(_obstacle) {}
	Game(StageArray&& _stage, const int _obstacle) : obstacle(_obstacle) { stage = move(_stage); }

	void update(const int pos, const PackArray& pack) {
		score = 0;
		chain = 0;
		dead = false;



	}

	const int getScore() const { return score; }
	const int getChain() const { return chain; }

	const bool isDead() const { return dead; }

private:

	StageArray stage;
	int obstacle = 0;

	int chain = 0;
	int score = 0;

	bool dead = false;

};
