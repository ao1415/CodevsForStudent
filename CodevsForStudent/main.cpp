#include "Grid.hpp"

using namespace std;

class Pack {
public:

	Pack() = default;
	Pack(PackArray&& p) { blocks = move(p); }
	Pack(Pack&& p) { blocks = move(p.blocks); }

	static Pack&& input() {
		PackArray data;

		for (int y = 0; y < PackSize; y++)
		{
			for (int x = 0; x < PackSize; x++)
			{
				int block;
				cin >> block;
				data[0][y][x] = block;
			}
		}

		string endStr;
		cin >> endStr;

		for (int r = 1; r < 4; r++)
		{
			for (int y = 0; y < PackSize; y++)
			{
				for (int x = 0; x < PackSize; x++)
				{
					data[r][x][PackSize - 1 - y] = data[r - 1][y][x];
				}
			}
		}

		return Pack(move(data));
	}

	void show() const {
		for (int i = 0; i < 4; i++)
		{
			for (int y = 0; y < PackSize; y++)
			{
				for (int x = 0; x < PackSize; x++)
				{

				}
			}
		}
	}

private:

	PackArray blocks;

};

int main() {

	cout << "Hello World" << endl;

	return 0;
}
