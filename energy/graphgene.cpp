#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<time.h>
#include<queue>
#include<string.h>
#include<vector>
#include<algorithm>
#include"vectoruse.hpp"
#include<random>
#include<math.h>
#include<fstream>
#include<thread>
#include<filesystem>
#include<unistd.h>
#include<chrono>


using namespace std;

struct Node
{
    double x_pos;
    double y_pos;
    double z_pos;
    double radii;
    int id;
    int head;
};

struct Edge
{
    int x;
    int y;
    int next;
    int capacity;
};

class Graph
{
public:
    int inf = 9999999;
    int number_of_nodes;
    int number_of_edges;
    Node* nodes = new Node[1000];
    Edge* edges = new Edge[100000];
    int* depth;
    Graph()
    {
        number_of_nodes = 0;
        number_of_edges = 0;
    }
    Graph(int N, int M) //生成N个节点，M条边的网络
    {
        number_of_nodes = N;
        number_of_edges = M;
    }
    Graph(int N)
    {
        number_of_nodes = N;
        number_of_edges = 0;
        for(int i = 0; i < N; i++)
        {
            nodes[i].head = -1;
            nodes[i].id = i;
        }
    }
    Graph(const Graph &G)
    {
        number_of_nodes = G.number_of_nodes;
        number_of_edges = G.number_of_edges;
        nodes = new Node[1000];
        edges = new Edge[100000];
        for(int i=0;i<number_of_nodes;i++)
        {
            nodes[i] = G.nodes[i];
        }
        for(int i=0;i<number_of_edges;i++)
        {
            edges[i] = G.edges[i];
        }
    }
    void add_edge(int i, int j)
    {
        int x = id_to_address(i);
        int y = id_to_address(j);
        // for(int i = 0; i < number_of_edges; i++)
        // {
        //     if(edges[i].x == x && edges[i].y == y)
        //         return;
        // }
        edges[number_of_edges].x = x;
        edges[number_of_edges].y = y;
        edges[number_of_edges].next = nodes[x].head;
        nodes[x].head = number_of_edges;
        number_of_edges++;
    }
    void add_edges(int edge_list[][2], int m)
    {
        for(int i = 0; i < m; i++)
        {
            add_edge(edge_list[i][0], edge_list[i][1]);
        }
    }
    ~Graph()
    {
        delete[] nodes;
        delete[] edges;
    }
    //无特殊说明，输入输出都是节点的地址，不是id
    vector<int> outneighbors(int i); //输出出节点的集合
    vector<int> inneighbors(int i); //输出入邻居的集合
    vector<int> bineighbors(int i); //双向邻居
    int id_to_address(int i); //将id转换为地址
    bool is_neighbor(int i, int j); //节点(id)i，j是否相邻
    bool is_strong_connected(); //判断G是否强连通
    bool is_connected(); //判断G是否连通
    bool st_is_k_connected(int s, int t, int k); //节点i,j是否k强连通
    bool is_k_connected(int k); //判断g是否k强连通
    int max_flow(int i, int j); //利用dinic算法计算(id)i,j最大流
    bool bfs(int s, int t); //dinic算法中的分层部分,若s不能到达t，返回false
    int dfs(int s, int t, int flow); //利用深度搜索查找增广路
    void write(string path);
    void read(string path);
    vector<int> st_minimum_cut(int i, int j);
    Graph flow_graph(); //生成用来计算最大流的网络
    Graph subgraph(vector<int> C); //根据集合C生成子图
    Graph reverse(); //生成反向网络
    int degree(int i);
    vector<int> all_nodes();
};

vector<int> Graph::all_nodes()
{
    vector<int> S;
    for(int i = 0; i < number_of_nodes; i++)
    {
        S.push_back(nodes[i].id);
    }
    return S;
}

int Graph::degree(int i){
    return outneighbors(i).size();
}

vector<int> Graph::outneighbors(int i)
{
    vector<int> S;
    for(int e = nodes[i].head; e != -1; e = edges[e].next)
    {
        S.push_back(edges[e].y);
    }
    return S;
}

bool Graph::is_neighbor(int i, int j)
{
    int s = id_to_address(i);
    int t = id_to_address(j);
    for(int e = nodes[s].head; e != -1; e = edges[e].next)
    {
        if(edges[e].y == t)
            return true;
    }
    return false;
}

int Graph::id_to_address(int i)
{
    for(int n = 0; n < number_of_nodes; n++)
    {
        if(nodes[n].id == i)
            return n;
    }
    printf("not such node");
    return -1;
}

Graph Graph::flow_graph()
{
    Graph g(2*number_of_nodes, 2*(number_of_nodes + number_of_edges));
    for(int n = 0; n < number_of_nodes; n++) //拆分节点,2n是入节点，2n+1是出节点
    {
        g.nodes[2*n].id = nodes[n].id;
        g.nodes[2*n].head = -1;
        g.nodes[2*n+1].id = nodes[n].id;
        g.nodes[2*n+1].head = -1;
    }
    for(int i = 0; i < number_of_edges; i++) //拆分边
    {
        g.edges[2*i].x = 2*edges[i].x + 1;
        g.edges[2*i].y = 2*edges[i].y;
        g.edges[2*i].capacity = inf;
        g.edges[2*i].next = g.nodes[g.edges[2*i].x].head;
        g.nodes[g.edges[2*i].x].head = 2*i;

        g.edges[2*i + 1].x = 2*edges[i].y;
        g.edges[2*i + 1].y = 2*edges[i].x + 1;
        g.edges[2*i + 1].capacity = 0;
        g.edges[2*i + 1].next = g.nodes[g.edges[2*i + 1].x].head;
        g.nodes[g.edges[2*i + 1].x].head = 2*i + 1;
    }
    for(int n = 0; n < number_of_nodes; n++) //添加节点内的边
    {
        int i = number_of_edges + n;
        g.edges[2*i].x = 2*n;
        g.edges[2*i].y = 2*n + 1;
        g.edges[2*i].capacity = 1;
        g.edges[2*i].next = g.nodes[g.edges[2*i].x].head;
        g.nodes[g.edges[2*i].x].head = 2*i;

        g.edges[2*i + 1].x = 2*n + 1;
        g.edges[2*i + 1].y = 2*n;
        g.edges[2*i + 1].capacity = 0;
        g.edges[2*i + 1].next = g.nodes[g.edges[2*i + 1].x].head;
        g.nodes[g.edges[2*i + 1].x].head = 2*i + 1;
    }
    g.depth = new int[g.number_of_nodes];
    return g;
}

bool Graph::bfs(int s, int t) //s为源点, t为汇点
{
    queue<int> Q;
    while (!Q.empty())
        Q.pop();
    //memset(depth, 0, sizeof(depth));
    for(int i = 0; i < number_of_nodes; i++) //设置深度，不能用memset因为是指针
    {
        depth[i] = 0;
    }
    //cout << depth[t] << endl;
    depth[s] = 1;
    Q.push(s);
    do
    {
    int u=Q.front();
    Q.pop();
    for (int i=nodes[u].head;i!=-1;i=edges[i].next)
        if ((depth[edges[i].y] == 0)&&(edges[i].capacity > 0))
            {
                depth[edges[i].y] = depth[u] + 1;
                Q.push(edges[i].y);
            }
    }
    while(!Q.empty());
    if(depth[t] > 0)
        return true;
    //cout << depth[t] << endl;
    return false;
}

int Graph::dfs(int u, int t, int flow)
{
    if(u == t) //到达汇点， 返回
        return flow;
    for(int i = nodes[u].head; i != -1; i = edges[i].next)
    {
        if ((depth[edges[i].y]==depth[u]+1)&&(edges[i].capacity != 0))//注意这里要满足分层图和残量不为0两个条件
        {
            int di = dfs(edges[i].y, t, min(flow, edges[i].capacity));//向下增广
            if (di>0)//若增广成功
            {
                edges[i].capacity -= di;//正向边减
                edges[i^1].capacity += di; //反向边加
                return di;//向上传递
            }
        }
    }
    return 0;//否则说明没有增广路，返回0
}

int Graph::max_flow(int i, int j)
{
    int s = id_to_address(i);
    int t = id_to_address(j);
    Graph g = flow_graph();
    int Ans = 0;
    while(g.bfs(2*s + 1, 2*t))
    {
        Ans += g.dfs(2*s + 1, 2*t, inf);
    }
    return Ans;
}

bool Graph::is_strong_connected()
{
    return is_k_connected(1);
}

bool Graph::st_is_k_connected(int i, int j, int k)
{
    int s = id_to_address(i);
    int t = id_to_address(j);
    int fl = max_flow(s, t);
    if(fl < k){
        return false;
    }
    return true;
}

bool Graph::is_k_connected(int k)
{
    for(int i = 0; i < number_of_nodes; i++)
    {
        for(int j = 0; j < number_of_nodes; j++)
        {
            if(i == j || is_neighbor(i, j))
                continue;
            if(!st_is_k_connected(i, j, k))
                return false;
        }
    }
    return true;
}



vector<int> Graph::st_minimum_cut(int i, int j)
{
    int s = id_to_address(i);
    int t = id_to_address(j);
    Graph g = flow_graph();
    s = 2*s + 1;
    t = 2*t;
    int Ans = 0;
    while(g.bfs(s, t))
    {
        Ans += g.dfs(s, t, inf);
    }
    // for(int i = 0; i < g.number_of_edges; i++)
    // {
    //     cout << g.edges[i].x << g.edges[i].y << g.edges[i].capacity << endl;
    // }
    vector<int> G2; //在残流网络能连到汇点的
    vector<int> G1; //在残流网络不能连到汇点的
    vector<int> temp; //临时储存
    vector<int> cut; //最小点割集
    for(int i = 0; i < g.number_of_nodes; i++){
        if(g.bfs(i, t))
            G2.push_back(i);
    }
    for(int i = 0; i < g.number_of_nodes; i++)
    {
        G1.push_back(i);
    }
    G1 = v_diff(G1, G2);
    for(int i = 2*number_of_edges; i < g.number_of_edges; i += 2)
    {
        int x = g.edges[i].x;
        int y = g.edges[i].y;
        if(v_has(G1, x) && v_has(G2, y))
        {
            temp.push_back(x);
            //cout << x << y << endl;
        }
    }
    for(int i : temp)
    {
        cut.push_back(g.nodes[i].id);
    }
    return cut;
}

Graph Graph::subgraph(vector<int> C)
{
    Graph g;
    g.number_of_nodes = C.size();
    g.number_of_edges = 0;
    //生成节点
    for(int i = 0; i < g.number_of_nodes; i++)
    {
        g.nodes[i].id = C[i];
        g.nodes[i].head = -1;
    }
    //生成边
    for(int i = 0; i < number_of_edges; i++)
    {
        int x = nodes[edges[i].x].id;
        int y = nodes[edges[i].y].id;
        if(v_has(C, x) && v_has(C, y)) //边的两端都在C中
        {
            g.add_edge(x, y);
        }
    }
    return g;
}

bool Graph::is_connected()
{
    int visited[number_of_nodes] = {0};
    int r = 0;
    visited[r] = 1;
    queue<int> Q;
    Q.push(r);
    while(!Q.empty())
    {
        int v = Q.front();
        Q.pop();
        for(int i = nodes[v].head; i != -1; i = edges[i].next)
        {
            int u = edges[i].y;
            if(!visited[u])
            {
                Q.push(u);
                visited[u] = 1;
            }
        }
    }
    for(int i = 0; i < number_of_nodes; i++)
    {
        if(visited[i] == 0)
        {
            //cout << i << endl;
            return false;
        } 
    }
    return true;
}   

Graph Graph::reverse()
{
    Graph g;
    g.number_of_nodes = number_of_nodes;
    g.number_of_edges = number_of_edges;
    for(int i = 0; i < number_of_nodes; i++)
    {
        g.nodes[i].id = nodes[i].id;
        g.nodes[i].head = -1;
    }
    //反转边
    for(int i = 0; i < number_of_edges; i++)
    {
        int x = edges[i].x;
        int y = edges[i].y;
        g.edges[i].x = y;
        g.edges[i].y = x;
        g.edges[i].next = g.nodes[y].head;
        g.nodes[y].head = i;
    }
    return g;
}





template<class T, size_t N>
inline size_t lengthof(const T (& arr)[N][2])
{
    return N;
}


Graph rdgraph(int N, int r, int range, int i) //在range的范围内产生节点为N连通度为k的图
{
    while(1)
    {
        int z = time(0); 
        srand((i+1) * z);
        Graph G(N);
        for(int i = 0; i < N; i++)
        {
            G.nodes[i].x_pos = (double)rand()/(double)RAND_MAX * range;
            G.nodes[i].y_pos = (double)rand()/(double)RAND_MAX * range;
            G.nodes[i].z_pos = 0;
            G.nodes[i].radii = r;
            //if(G.nodes[i].x_pos > range) cout << G.nodes[i].x_pos << endl;
        }
        int cnt = 0;
        for(int i = 0; i < N; i++)
        {
            for(int j = i+1; j < N; j++)
            {   
                //cout << cnt++ << endl;
                //cnt ++;
                if(i == j) continue;
                double dis = (double)sqrt((double)pow((G.nodes[i].x_pos - G.nodes[j].x_pos), 2) + (double)pow((G.nodes[i].y_pos - G.nodes[j].y_pos), 2));
                //cout << dis << endl;
                // if(dis > range)
                // {
                //     //cout << dis;
                //     cout << G.nodes[i].x_pos << endl;
                //     cout << G.nodes[i].y_pos << endl;
                //     cout << G.nodes[j].x_pos << endl;
                //     cout << G.nodes[j].y_pos << endl;
                //     getchar();
                // }
                if(dis < G.nodes[i].radii)
                {
                    G.add_edge(i, j);
                    G.add_edge(j, i);
                }
            }
        }
        if(G.is_connected())
        {
            cout << "N = " << N << endl;
            cout << "average degree = " << G.number_of_edges*2/N << endl;
            return G;
        }
    }
}

void Graph::write(string path)
{
    ofstream net(path, ios::out);
    if(!net) cout << "write error" << endl;
    net << number_of_nodes << " " << number_of_edges << endl;
    for(int i = 0; i < number_of_nodes; i++)
        net << nodes[i].id << " " << nodes[i].head << " " << nodes[i].x_pos << " " << nodes[i].y_pos << " "  << nodes[i].radii << endl;
    for(int i = 0; i < number_of_edges; i++)
        net << edges[i].x << " " << edges[i].y << " " << edges[i].next << endl;
    net.close();
}

void genegraph(int N, int r, int range, int i)
{
    string savepath = "./output/graphdata/N=" + to_string(N) + "R=" + to_string(r) + "/";
    filesystem::create_directories(savepath);
    int start = time(0);
    cout << i << "start" << endl;
    string graphpath = savepath + "net" + to_string(i) + ".txt";
    Graph G = rdgraph(N, r, range, i);
    G.write(graphpath);
    int end = time(0);
    cout << i << "time:" << end - start << endl;
}


void genegraphWithDegree(int N, int type){ //type分三类， 0 是smaller， 1是equal， 2是greater

    string graphType;
    int degree;
    switch(type){
        case 0: graphType = "smaller"; degree = N/4; break;
        case 1: graphType = "equal";  degree = N/2; break;
        case 2: graphType = "greater";  degree = N*3/4; break;
    }

    // 使用新的输出路径（相对于项目根目录）
    string savepath = "./output/graphs/" + graphType + "/N=" + to_string(N) + "/";
    filesystem::create_directories(savepath);

    int t = 100;
    for(int i = 0; i < t; i++) //生成t张图
    {
        int z = time(0);
        srand((i+1) * z);
        Graph G(N);
        vector<int> allNodes = G.all_nodes();
        for(int j = 0; j < degree * N / 2; j++){
            int x = rand()%N;
            vector<int> outNeighbor = G.outneighbors(x);
            outNeighbor.push_back(x);
            vector<int> tmp = v_diff(allNodes, outNeighbor);
            if(tmp.size() == 0) continue; //该节点不能再加边了
            int y = rand()%(tmp.size());
            y = tmp[y];
            G.add_edge(x, y);
            G.add_edge(y, x);
        }
        string graphpath = savepath + "net" + to_string(i) + ".txt";
        G.write(graphpath);
        if(!G.is_connected()) i--;
        cout << "  ✓ " << graphType << " N=" << N << " net" << i << endl;
    }
}


int main(){
    cout << "====================================================" << endl;
    cout << "图数据生成程序" << endl;
    cout << "====================================================" << endl;

    // 模式选择：1=单位圆盘随机图 (R=250), 2=特定度数图 (smaller/equal/greater)
    int mode = 1;

    cout << "\n选择生成模式:" << endl;
    cout << "  1: 单位圆盘随机图 (ex1~ex7)" << endl;
    cout << "  2: 特定度数图 (alpha扫描/Figure3)" << endl;
    cout << "输入模式 (默认1): ";

    string input;
    getline(cin, input);
    if (!input.empty()) {
        mode = stoi(input);
    }

    const int RANGE = 500;      // 500×500 平面

    if (mode == 1) {
        // ===== 模式1: 生成单位圆盘随机几何图 =====
        cout << "\n[模式1] 生成单位圆盘随机几何图 (R=250)" << endl;
        cout << "参数: N=10~100, 每个N生成100个样本" << endl;

        const int R = 250;          // 通信半径
        const int T_FULL = 100;     // 100 samples

        for (int N = 10; N <= 100; N += 10) {
            string base = "./output/graphs/R=" + to_string(R) + "/N=" + to_string(N) + "/";
            filesystem::create_directories(base);

            for (int i = 0; i < T_FULL; ++i) {
                genegraph(N, R, RANGE, i);
                cout << "  ✓ N=" << N << " net" << i << endl;
            }
            cout << "N=" << N << ": 完成" << endl;
        }

    } else if (mode == 2) {
        genegraphWithDegree(50, 0);
        genegraphWithDegree(50, 1);
        genegraphWithDegree(50, 2);
        genegraphWithDegree(100, 0);
        genegraphWithDegree(100, 1);
        genegraphWithDegree(100, 2);
    } else {
        cout << "未知模式: " << mode << endl;
        return 1;
    }

    cout << "\n====================================================" << endl;
    cout << "✓ 图生成完成！" << endl;
    cout << "====================================================" << endl;
    return 0;
}