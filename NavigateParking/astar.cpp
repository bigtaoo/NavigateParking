#include "astar.h"
#include "parkmapgridinfo.h"
#include <QtMath>

Node::Node(int pos, Node* par) : G(0), H(0), position(pos), parent(par)
{
}

int Node::GetScore()
{
    return G + H;
}

int AStar::getDistance(int start, int end)
{
    int startX = start % MAP_WIDTH;
    int startY = start / MAP_WIDTH;
    int endX = end % MAP_WIDTH;
    int endY = end / MAP_WIDTH;
    int delta = qAbs(startX - endX) + qAbs(startY - endY);

    return delta * 10;
}

Node* AStar::findNodeOnList(NodeSet& nodes, int pos)
{
    for (auto node : nodes) {
        if (node->position == pos) {
            return node;
        }
    }
    return nullptr;
}

void AStar::releaseNodes(NodeSet& nodes)
{
    for (auto it = nodes.begin(); it != nodes.end();) {
        delete *it;
        it = nodes.erase(it);
    }
}

bool AStar::isRoad(int position)
{
    int x = position % MAP_WIDTH;
    int y = position / MAP_HEIGHT;
    if(ParkMapGridInfo::GetIns()->GetGrid(MAP_WIDTH * y + x) == MapGrid::MG_Road_Left
            || ParkMapGridInfo::GetIns()->GetGrid(MAP_WIDTH * y + x) == MapGrid::MG_Road_Right)
    {
        return true;
    }
    return false;
}

int AStar::direction(int position, int dir)
{
    int x = position % MAP_WIDTH;
    int y = position / MAP_WIDTH;
    if(dir == 0){
        --x;
    }else if(dir == 1){
        ++x;
    }else if(dir == 2){
        --y;
    }else if(dir == 3){
        ++y;
    }
    return x + y * MAP_WIDTH;
}

void AStar::FindPath(int start, int end, QVector<int> &path)
{
    Node *current = nullptr;
    NodeSet openSet, closedSet;
    openSet.insert(new Node(start));

    while (!openSet.empty()) {
        current = *openSet.begin();
        for (auto node : openSet) {
            if (node->GetScore() <= current->GetScore()) {
                current = node;
            }
        }
        if (current->position == end) {
            break;
        }

        closedSet.insert(current);
        openSet.erase(std::find(openSet.begin(), openSet.end(), current));

        for (int i = 0; i < 4; ++i) {
            int newCoordinates(direction( current->position, i));
            if (!isRoad(newCoordinates) ||
                findNodeOnList(closedSet, newCoordinates)) {
                continue;
            }

            int totalCost = current->G + ((i < 4) ? 10 : 14);

            Node *successor = findNodeOnList(openSet, newCoordinates);
            if (successor == nullptr) {
                successor = new Node(newCoordinates, current);
                successor->G = totalCost;
                successor->H = getDistance(successor->position, end);
                openSet.insert(successor);
            }
            else if (totalCost < successor->G) {
                successor->parent = current;
                successor->G = totalCost;
            }
        }
    }

    while (current != nullptr) {
        path.push_back(current->position);
        current = current->parent;
    }

    releaseNodes(openSet);
    releaseNodes(closedSet);
}
