#pragma once
#include "pch.h"
#include "BoundingBox.h"  // BoundingBox만 포함
#include "Creature.h"
#include "Player.h"
#include "Monster.h"

template <typename T>
class QuadTreeNode {
public:

    BoundingBox bounds;
    std::vector<std::shared_ptr<T>> objects;
    std::unique_ptr<QuadTreeNode> children[4];
    int maxObjects = 5;

    QuadTreeNode(BoundingBox bounds) : bounds(bounds) {}

    bool Insert(std::shared_ptr<T> object) {
        if (!bounds.Contains(object->GetPosition()))
            return false;

        if (objects.size() < maxObjects && IsLeaf()) {
            objects.push_back(object);
            return true;
        }

        if (IsLeaf())
            Split();

        for (auto& child : children) {
            if (child->Insert(object))
                return true;
        }

        return false;
    }

    bool Remove(std::shared_ptr<T> object) {
        if (!bounds.Contains(object->GetPosition()))
            return false;

        // 현재 노드에서 객체 제거
        auto it = std::find(objects.begin(), objects.end(), object);
        if (it != objects.end()) {
            objects.erase(it);
            return true;
        }

        // 자식 노드에서 제거 시도
        if (!IsLeaf()) {
            for (auto& child : children) {
                if (child->Remove(object))
                    return true;
            }
        }

        return false;
    }

    void QueryRange(const BoundingBox& range, std::vector<std::shared_ptr<T>>& result) const {
        if (!bounds.Intersects(range))
            return;

        for (auto& object : objects) {
            if (range.Contains(object->GetPosition()))
                result.push_back(object);
        }

        if (!IsLeaf()) {
            for (auto& child : children) {
                child->QueryRange(range, result);
            }
        }
    }

    void Split() {
        Vector3 center((bounds.minPoint.x + bounds.maxPoint.x) / 2, 0, (bounds.minPoint.z + bounds.maxPoint.z) / 2);

        children[0] = std::make_unique<QuadTreeNode>(BoundingBox(bounds.minPoint, center));
        children[1] = std::make_unique<QuadTreeNode>(BoundingBox(Vector3(center.x, 0, bounds.minPoint.z),
            Vector3(bounds.maxPoint.x, 0, center.z)));
        children[2] = std::make_unique<QuadTreeNode>(BoundingBox(Vector3(bounds.minPoint.x, 0, center.z),
            Vector3(center.x, 0, bounds.maxPoint.z)));
        children[3] = std::make_unique<QuadTreeNode>(BoundingBox(center, bounds.maxPoint));
    }

    bool IsLeaf() const { return !children[0]; }
};

// Generic QuadTree
template <typename T>
class QuadTree {
public:
    std::unique_ptr<QuadTreeNode<T>> root;

    QuadTree(BoundingBox bounds) { root = std::make_unique<QuadTreeNode<T>>(bounds); }

    bool Insert(std::shared_ptr<T> object) { return root->Insert(object); }
    bool Remove(std::shared_ptr<T> object) { return root->Remove(object); }

    std::vector<std::shared_ptr<T>> QueryRange(const BoundingBox& range) const {
        std::vector<std::shared_ptr<T>> result;
        root->QueryRange(range, result);
        return result;
    }
};

class QuadTreeManager {
public:
    static QuadTreeManager& GetInstance() {
        static QuadTreeManager instance;
        return instance;
    }

    void InitializeRoom(int roomId, BoundingBox playerBounds, BoundingBox monsterBounds) {
        if (_roomQuadTrees.find(roomId) != _roomQuadTrees.end()) {
            LOG_EVENT(Color::RED, L"Room ID: %d already initialized.\n", roomId);
            return;
        }

        RoomQuadTrees roomQuadTrees;
        roomQuadTrees.playerQuadTree = std::make_unique<QuadTree<Player>>(playerBounds);
        roomQuadTrees.monsterQuadTree = std::make_unique<QuadTree<Monster>>(monsterBounds);

        _roomQuadTrees[roomId] = std::move(roomQuadTrees);

        LOG_EVENT(Color::GREEN, L"Room ID: %d QuadTree initialized.\n", roomId);
    }

    QuadTree<Player>* GetPlayerQuadTree(int roomId) {
        if (_roomQuadTrees.find(roomId) != _roomQuadTrees.end()) {
            return _roomQuadTrees[roomId].playerQuadTree.get();
        }
        return nullptr;
    }

    QuadTree<Monster>* GetMonsterQuadTree(int roomId) {
        if (_roomQuadTrees.find(roomId) != _roomQuadTrees.end()) {
            return _roomQuadTrees[roomId].monsterQuadTree.get();
        }
        return nullptr;
    }

    void RemoveRoom(int roomId) {
        _roomQuadTrees.erase(roomId);
        LOG_EVENT(Color::YELLOW, L"Room ID: %d QuadTree removed.\n", roomId);
    }

private:
    struct RoomQuadTrees {
        std::unique_ptr<QuadTree<Player>> playerQuadTree;
        std::unique_ptr<QuadTree<Monster>> monsterQuadTree;
    };

    std::unordered_map<int, RoomQuadTrees> _roomQuadTrees;

    QuadTreeManager() {}
    ~QuadTreeManager() {}
};
