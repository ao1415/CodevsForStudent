#include "Share.hpp"

int Share::turn = 0;
std::vector<Pack> Share::packs;

int Share::now;
int Share::time;
int Share::myObstacle;
int Share::enObstacle;

StageArray Share::myStage;
StageArray Share::enStage;

array<vector<int>, 10> Share::blockContainPacks;
