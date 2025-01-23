#pragma once

struct Node {
	int x, y;          // ��� ��ġ
	float gCost;       // ���������� ���� �������� �Ÿ�
	float hCost;       // ���� ��忡�� ��ǥ�������� ���� �Ÿ� (�޸���ƽ)
	float fCost() const { return gCost + hCost; } // �� ���

	Node* parent = nullptr; // ��� ������ ���� �θ� ���

	bool operator==(const Node& other) const {
		return x == other.x && y == other.y;
	}

	struct HashFunction {
		size_t operator()(const Node& node) const {
			return std::hash<int>()(node.x) ^ std::hash<int>()(node.y);
		}
	};
};

class AStar {
public:
	using Path = std::vector<std::pair<int, int>>;

	AStar(int mapWidth, int mapHeight, const std::vector<std::string>& mapData);

	Path FindPath(int startX, int startY, int goalX, int goalY);

private:
	bool IsWalkable(int x, int y) const;
	std::vector<Node> GetNeighbors(const Node& node)const;

	int _mapWidth;
	int _mapHeight;
	std::vector<std::string> _mapData;
};