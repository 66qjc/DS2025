#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <climits>
#include <algorithm>
#include <set>
using namespace std;

// 图类：支持无向图，包含权重（默认权重为1，无边为0）
class Graph
{
private:
    int vertexNum;                 // 顶点数
    vector<char> vertices;         // 顶点名称（如A、B、C...）
    vector<vector<int>> adjMatrix; // 邻接矩阵（0表示无边，>0表示权重）

    // Tarjan算法辅助变量（双连通分量+关节点）
    vector<int> dfn;                                      // 发现时间
    vector<int> low;                                      // 能到达的最早发现时间的顶点
    vector<bool> isArticulation;                          // 是否为关节点
    vector<vector<pair<int, int>>> biconnectedComponents; // 双连通分量（存储边）
    stack<pair<int, int>> edgeStack;                      // 存储当前路径的边
    int timeStamp;                                        // 时间戳

public:
    // 构造函数：传入顶点列表
    Graph(const vector<char> &vs) : vertexNum(vs.size()), vertices(vs)
    {
        adjMatrix.resize(vertexNum, vector<int>(vertexNum, 0));
    }

    // 添加无向边（u, v为顶点索引，weight为权重）
    void addEdge(int u, int v, int weight = 1)
    {
        if (u >= 0 && u < vertexNum && v >= 0 && v < vertexNum && u != v)
        {
            adjMatrix[u][v] = weight;
            adjMatrix[v][u] = weight;
        }
    }

    // 输出邻接矩阵
    void printAdjMatrix()
    {
        cout << "邻接矩阵（行/列：";
        for (int i = 0; i < vertexNum; i++)
        {
            if (i > 0)
                cout << " ";
            cout << vertices[i];
        }
        cout << "）：\n";
        for (int i = 0; i < vertexNum; i++)
        {
            cout << vertices[i] << " ";
            for (int j = 0; j < vertexNum; j++)
            {
                cout << adjMatrix[i][j] << " ";
            }
            cout << "\n";
        }
    }

    // 辅助函数：根据顶点名称获取索引
    int getVertexIndex(char c)
    {
        auto it = find(vertices.begin(), vertices.end(), c);
        return it != vertices.end() ? it - vertices.begin() : -1;
    }

    // 1. BFS遍历（从起点startName出发）
    void BFS(char startName)
    {
        int start = getVertexIndex(startName);
        if (start == -1)
        {
            cout << "起点不存在！\n";
            return;
        }

        vector<bool> visited(vertexNum, false);
        queue<int> q;
        visited[start] = true;
        q.push(start);

        cout << "BFS遍历（从" << startName << "出发）：";
        while (!q.empty())
        {
            int u = q.front();
            q.pop();
            cout << vertices[u] << " ";
            for (int v = 0; v < vertexNum; v++)
            {
                if (adjMatrix[u][v] != 0 && !visited[v])
                {
                    visited[v] = true;
                    q.push(v);
                }
            }
        }
        cout << "\n";
    }

    // 2. DFS遍历（递归实现）
    void DFS(char startName)
    {
        int start = getVertexIndex(startName);
        if (start == -1)
        {
            cout << "起点不存在！\n";
            return;
        }

        vector<bool> visited(vertexNum, false);
        cout << "DFS遍历（从" << startName << "出发）：";
        DFSRecursive(start, visited);
        cout << "\n";
    }

private:
    void DFSRecursive(int u, vector<bool> &visited)
    {
        visited[u] = true;
        cout << vertices[u] << " ";
        for (int v = 0; v < vertexNum; v++)
        {
            if (adjMatrix[u][v] != 0 && !visited[v])
            {
                DFSRecursive(v, visited);
            }
        }
    }

public:
    // 3. Dijkstra最短路径
    void Dijkstra(char startName)
    {
        int start = getVertexIndex(startName);
        if (start == -1)
        {
            cout << "起点不存在！\n";
            return;
        }

        vector<int> dist(vertexNum, INT_MAX);
        vector<bool> visited(vertexNum, false);
        vector<int> prev(vertexNum, -1);
        dist[start] = 0;

        for (int i = 0; i < vertexNum; i++)
        {
            int u = -1;
            for (int v = 0; v < vertexNum; v++)
            {
                if (!visited[v] && (u == -1 || dist[v] < dist[u]))
                {
                    u = v;
                }
            }
            if (dist[u] == INT_MAX)
                break;
            visited[u] = true;

            for (int v = 0; v < vertexNum; v++)
            {
                if (adjMatrix[u][v] != 0 && !visited[v])
                {
                    int newDist = dist[u] + adjMatrix[u][v];
                    if (newDist < dist[v])
                    {
                        dist[v] = newDist;
                        prev[v] = u;
                    }
                }
            }
        }

        cout << "Dijkstra最短路径（从" << startName << "出发）：\n";
        for (int i = 0; i < vertexNum; i++)
        {
            cout << startName << "到" << vertices[i] << "：";
            if (dist[i] == INT_MAX)
            {
                cout << "不可达\n";
            }
            else
            {
                cout << "距离=" << dist[i] << "，路径：";
                stack<char> path;
                for (int v = i; v != -1; v = prev[v])
                {
                    path.push(vertices[v]);
                }
                while (!path.empty())
                {
                    cout << path.top();
                    path.pop();
                    if (!path.empty())
                        cout << "->";
                }
                cout << "\n";
            }
        }
    }

    // 4. Prim最小生成树
    void Prim(char startName)
    {
        int start = getVertexIndex(startName);
        if (start == -1)
        {
            cout << "起点不存在！\n";
            return;
        }

        vector<int> key(vertexNum, INT_MAX);
        vector<int> parent(vertexNum, -1);
        vector<bool> inMST(vertexNum, false);
        key[start] = 0;

        for (int i = 0; i < vertexNum; i++)
        {
            int u = -1;
            for (int v = 0; v < vertexNum; v++)
            {
                if (!inMST[v] && (u == -1 || key[v] < key[u]))
                {
                    u = v;
                }
            }

            if (key[u] == INT_MAX)
            {
                cout << "图不连通，无法生成最小支撑树！\n";
                return;
            }
            inMST[u] = true;

            for (int v = 0; v < vertexNum; v++)
            {
                if (adjMatrix[u][v] != 0 && !inMST[v] && adjMatrix[u][v] < key[v])
                {
                    key[v] = adjMatrix[u][v];
                    parent[v] = u;
                }
            }
        }

        cout << "Prim最小支撑树（从" << startName << "出发）：\n";
        int totalWeight = 0;
        for (int i = 0; i < vertexNum; i++)
        {
            if (parent[i] != -1)
            {
                cout << vertices[parent[i]] << "-" << vertices[i] << "（权重：" << adjMatrix[parent[i]][i] << "）\n";
                totalWeight += adjMatrix[parent[i]][i];
            }
        }
        cout << "MST总权重：" << totalWeight << "\n";
    }

    // 5. 双连通分量与关节点（Tarjan算法）
    void findBiconnectedComponentsAndArticulation()
    {
        dfn.assign(vertexNum, -1);
        low.assign(vertexNum, -1);
        isArticulation.assign(vertexNum, false);
        biconnectedComponents.clear();

        // 清空边栈
        while (!edgeStack.empty())
            edgeStack.pop();

        timeStamp = 0;

        for (int i = 0; i < vertexNum; i++)
        {
            if (dfn[i] == -1)
            {
                Tarjan(i, -1);
            }
        }

        // 输出关节点
        cout << "关节点（割点）：";
        bool hasArticulation = false;
        for (int i = 0; i < vertexNum; i++)
        {
            if (isArticulation[i])
            {
                cout << vertices[i] << " ";
                hasArticulation = true;
            }
        }
        if (!hasArticulation)
            cout << "无";
        cout << "\n";

        // 去重并输出双连通分量（避免同一分量内边重复）
        cout << "双连通分量（共" << biconnectedComponents.size() << "个）：\n";
        for (int idx = 0; idx < biconnectedComponents.size(); idx++)
        {
            auto &comp = biconnectedComponents[idx];
            set<pair<int, int>> uniqueEdges;
            for (auto &e : comp)
            {
                uniqueEdges.insert({min(e.first, e.second), max(e.first, e.second)});
            }
            cout << "分量" << idx + 1 << "：";
            for (auto &e : uniqueEdges)
            {
                cout << vertices[e.first] << "-" << vertices[e.second] << " ";
            }
            cout << "\n";
        }
    }

private:
    void Tarjan(int u, int parent)
    {
        dfn[u] = low[u] = ++timeStamp;
        int children = 0;

        for (int v = 0; v < vertexNum; v++)
        {
            if (adjMatrix[u][v] == 0)
                continue; // 无边
            if (v == parent)
                continue; // 跳过父节点

            if (dfn[v] == -1)
            {
                children++;
                edgeStack.push({u, v});
                Tarjan(v, u);
                low[u] = min(low[u], low[v]);

                // 判断割点（非根）
                if (parent != -1 && low[v] >= dfn[u])
                {
                    isArticulation[u] = true;
                    vector<pair<int, int>> component;
                    while (true)
                    {
                        auto top = edgeStack.top();
                        edgeStack.pop();
                        component.push_back(top);
                        if (top.first == u && top.second == v)
                            break;
                    }
                    biconnectedComponents.push_back(component);
                }
            }
            else if (dfn[v] < dfn[u])
            { // 回边（且不是父节点）
                edgeStack.push({u, v});
                low[u] = min(low[u], dfn[v]);
            }
        }

        // 根节点：若子树 >= 2，则为割点
        if (parent == -1 && children >= 2)
        {
            isArticulation[u] = true;
        }
    }
};

// 构建图1（带权重，9个顶点 A-I）
Graph buildGraph1()
{
    vector<char> vertices = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I'};
    Graph g(vertices);

    auto add = [&](char a, char b, int w)
    {
        int u = g.getVertexIndex(a);
        int v = g.getVertexIndex(b);
        g.addEdge(u, v, w);
    };

    add('A', 'B', 2);
    add('A', 'C', 4);
    add('B', 'E', 9);
    add('B', 'F', 12);
    add('C', 'D', 6);
    add('C', 'E', 13);
    add('D', 'G', 2);
    add('E', 'F', 1);
    add('E', 'G', 11);
    add('E', 'H', 14);
    add('F', 'H', 3);
    add('G', 'H', 5);
    add('H', 'I', 10);

    return g;
}

// 构建图2（12个顶点 A-L，无权）
Graph buildGraph2()
{
    vector<char> vertices = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L'};
    Graph g(vertices);

    auto add = [&](char a, char b)
    {
        int u = g.getVertexIndex(a);
        int v = g.getVertexIndex(b);
        g.addEdge(u, v, 1);
    };

    add('A', 'B');
    add('A', 'C');
    add('B', 'D');
    add('B', 'E');
    add('C', 'E');
    add('C', 'F');
    add('D', 'G');
    add('E', 'G');
    add('E', 'H');
    add('F', 'H');
    add('G', 'I');
    add('G', 'J');
    add('H', 'K');
    add('H', 'L');
    add('I', 'J');
    add('K', 'L');

    return g;
}

int main()
{
    // -------------------------- 任务1：图1邻接矩阵 --------------------------
    cout << "==================== 任务1：图1邻接矩阵 ====================\n";
    Graph g1 = buildGraph1();
    g1.printAdjMatrix();
    cout << "\n";

    // -------------------------- 任务2：图1 BFS+DFS --------------------------
    cout << "==================== 任务2：图1 BFS+DFS ====================\n";
    g1.BFS('A');
    g1.DFS('A');
    cout << "\n";

    // -------------------------- 任务3：图1 最短路径+最小支撑树 --------------------------
    cout << "==================== 任务3：图1 最短路径+MST ====================\n";
    g1.Dijkstra('A');
    cout << "\n";
    g1.Prim('A');
    cout << "\n";

    // -------------------------- 任务4：图2 双连通分量+关节点 --------------------------
    cout << "==================== 任务4：图2 双连通分量+关节点 ====================\n";
    Graph g2 = buildGraph2();
    g2.findBiconnectedComponentsAndArticulation();

    // 注意：Tarjan结果与起点无关，无需多次调用验证
    // 若需验证一致性，可构建相同图再调用，但结果必然一致

    return 0;
}
