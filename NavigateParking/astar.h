#ifndef ASTAR_H
#define ASTAR_H

#include <QVector>
#include <QSet>

struct Node
{
    int G, H;
    int position;
    Node* parent;

    Node(int pos, Node *parent_ = nullptr);
    int GetScore();
};

using NodeSet = QSet<Node*>;
class AStar
{
public:
    static void FindPath(int start, int end, QVector<int>& path);

private:
    static int getDistance(int start, int end);
    static Node* findNodeOnList(NodeSet& nodes, int position);
    static void releaseNodes(NodeSet& nodes);
    static bool isRoad(int position);
    static int direction(int position, int dir);
};

#endif // ASTAR_H
