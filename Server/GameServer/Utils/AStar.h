#pragma once

struct Node {
	int x, y;          // 노드 위치
	float gCost;       // 시작점에서 현재 노드까지의 거리
	float hCost;       // 현재 노드에서 목표점까지의 예상 거리 (휴리스틱)
	float fCost() const { return gCost + hCost; } // 총 비용

	Node* parent = nullptr; // 경로 추적을 위한 부모 노드

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