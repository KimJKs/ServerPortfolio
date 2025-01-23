#include "pch.h"
#include "AStar.h"
#include <cmath>
#include <limits>
#include <queue>

AStar::AStar(int mapWidth, int mapHeight, const std::vector<string>& mapData)
	: _mapWidth(mapWidth), _mapHeight(mapHeight), _mapData(mapData) {
}

bool AStar::IsWalkable(int x, int y) const {
	if (x < 0 || x >= _mapWidth || y < 0 || y >= _mapHeight)
		return false;
	return _mapData[y][x] == '0'; // '0'은 이동 가능, '1'은 장애물
}

std::vector<Node> AStar::GetNeighbors(const Node& node) const {
	static const std::vector<std::pair<int, int>> directions = {
		{0, 1}, {1, 0}, {0, -1}, {-1, 0} // 상하좌우
	};

	std::vector<Node> neighbors;
	for (const auto& [dx, dy] : directions) {
		int nx = node.x + dx;
		int ny = node.y + dy;

		if (IsWalkable(nx, ny)) {
			neighbors.push_back({ nx, ny });
		}
	}
	return neighbors;
}

AStar::Path AStar::FindPath(int startX, int startY, int goalX, int goalY) {
    // 좌표 변환 (Unity 좌표계에서 A* 좌표계로)
    startX += _mapWidth / 2;
    startY += _mapHeight / 2;
    goalX += _mapWidth / 2;
    goalY += _mapHeight / 2;

    // 목표 위치가 이동 불가능한 경우 주변에서 가장 가까운 이동 가능한 위치를 탐색
    if (!IsWalkable(goalX, goalY)) {
        bool foundAlternativeGoal = false;
        const int maxSearchRadius = 5; // 탐색 범위
        for (int radius = 1; radius <= maxSearchRadius && !foundAlternativeGoal; ++radius) {
            for (int dx = -radius; dx <= radius; ++dx) {
                for (int dy = -radius; dy <= radius; ++dy) {
                    int newGoalX = goalX + dx;
                    int newGoalY = goalY + dy;

                    if (IsWalkable(newGoalX, newGoalY)) {
                        goalX = newGoalX;
                        goalY = newGoalY;
                        foundAlternativeGoal = true;
                        break;
                    }
                }
                if (foundAlternativeGoal) break;
            }
        }

        if (!foundAlternativeGoal) {
            // 주변에서도 이동 가능한 위치를 찾지 못한 경우 빈 경로 반환
            return {};
        }
    }

    // A* 알고리즘 시작
    auto CompareNodes = [](const Node& a, const Node& b) {
        return a.fCost() > b.fCost(); // fCost가 작은 것을 우선 처리
        };
    std::priority_queue<Node, std::vector<Node>, decltype(CompareNodes)> openSet(CompareNodes);

    std::unordered_set<Node, Node::HashFunction> closedSet;
    std::unordered_map<Node, float, Node::HashFunction> gScores;

    Node startNode = { startX, startY, 0.0f, 0.0f };
    Node goalNode = { goalX, goalY, 0.0f, 0.0f };

    openSet.push(startNode);
    gScores[startNode] = 0.0f;

    while (!openSet.empty()) {
        Node current = openSet.top();
        openSet.pop();

        if (current == goalNode) {
            Path path;
            while (current.parent != nullptr) {
                int unityX = current.x - (_mapWidth / 2);
                int unityY = current.y - (_mapHeight / 2);
                path.emplace_back(unityX, unityY);
                current = *current.parent;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        closedSet.insert(current);

        for (Node& neighbor : GetNeighbors(current)) {
            if (closedSet.find(neighbor) != closedSet.end())
                continue;

            float tentativeG = gScores[current] + 1.0f;

            if (gScores.find(neighbor) == gScores.end() || tentativeG < gScores[neighbor]) {
                gScores[neighbor] = tentativeG;
                neighbor.gCost = tentativeG;
                neighbor.hCost = std::hypot(goalX - neighbor.x, goalY - neighbor.y);
                neighbor.parent = new Node(current);

                openSet.push(neighbor);
            }
        }
    }

    return {}; // 경로를 찾지 못한 경우 빈 벡터 반환
}

