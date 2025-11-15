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
#include<string>
using namespace std;



struct separation
{
    vector<int> A1;
    vector<int> A2;
};

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
        delete[] nodes;
        delete[] edges;
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
    vector<int> all_nodes(); //返回所有节点的id集合
    int id_to_address(int i); //将id转换为地址
    bool is_neighbor(int i, int j); //节点i，j是否相邻
    bool is_neighbor_byid(int i, int j); //节点(id)i，j是否相邻
    bool is_connected(); //判断是否连通
    bool is_strong_connected(); //判断G是否强连通
    bool st_is_k_connected(int s, int t, int k); //节点i,j是否k强连通
    bool st_is_k_connected_byid(int s, int t, int k); //节点(id)i,j是否k强连通    
    bool is_k_connected(int k); //判断g是否k强连通
    int max_flow(int i, int j); //利用dinic算法计算(id)i,j最大流
    bool bfs(int s, int t); //dinic算法中的分层部分,若s不能到达t，返回false
    int dfs(int s, int t, int flow); //利用深度搜索查找增广路
    void write(string path);
    void read(string path);
    void copy(Graph G); 
    vector<int> st_minimum_cut(int i, int j);
    vector<vector<int>> find_all_k_block(int k); //找到所有k-block
    vector<int> find_k_block(int k); //找到一个k-block
    vector<int> find_k_block_contain(int k, vector<int> B); //找到包含B的k-block
    vector<int> G2(vector<int> separator, int i); //去除分离集后能连到i的集合
    vector<int> Tpath(vector<int> D, vector<int> U, 
    vector<int> separator, int t); //返回T-path的id集合
    vector<int> myTpath(vector<int> D, vector<int> U, 
    vector<int> separator, vector<int> C);
    vector<int> path(int s, int t); //返回最短路径的id集合，若不连通，则返回大小为1的集合（首节点）
    vector<int> find_k_path(int s, int t, int k); //返回k条不相交路径的集合
    Graph flow_graph(); //生成用来计算最大流的网络
    Graph subgraph(vector<int> C); //根据集合C生成子图
    Graph sepa_sub(vector<int> S); //删除集合S生成的子图
    Graph reverse(); //生成反向网络
    separation a_separation(vector<int> separator, int t); 
    double avepath_throu_T(vector<int> T); //返回节点间通过T进行路由的平均路径长度
    int degree(int i);
};


int Graph::degree(int i){
    return this->outneighbors(i).size();
}

bool Graph::is_neighbor(int i, int j)
{
    for(int e = nodes[i].head; e != -1; e = edges[e].next)
    {
        if(edges[e].y == j)
            return true;
    }
    return false;
}

bool Graph::is_neighbor_byid(int i, int j)
{
    int s = id_to_address(i); 
    int t = id_to_address(j); 
    return is_neighbor(s, t);
}

int Graph::id_to_address(int i)
{
    for(int n = 0; n < number_of_nodes; n++)
    {
        if(nodes[n].id == i)
            return n;
    }
    cout << "not nodes:" << i << endl;
    cout << number_of_nodes << endl;
    getchar();
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

vector<int> Graph::find_k_path(int i, int j, int k)
{   
    int s = id_to_address(i);
    int t = id_to_address(j);
    vector<int> D;
    Graph g = flow_graph();
    int Ans = 0;
    while(g.bfs(2*s + 1, 2*t))
    {
        Ans += g.dfs(2*s + 1, 2*t, inf);
        if(Ans == k) break;
    }
    if(Ans < k) cout << "not k path" << endl;
    for(int i = 2*number_of_edges; i < g.number_of_edges; i += 2)
    {
        if(g.edges[i].capacity == 0) D.push_back(g.nodes[g.edges[i].x].id);
    }
    D = v_uni(D);
    return D;
}

bool Graph::is_connected()
{
    int visited[number_of_nodes] = {0};
    queue<int> q;
    q.push(0);
    visited[0] = 1;
    int cur = 0;
    while(!q.empty())
    {
        cur = q.front();
        q.pop();
        for(int i = nodes[cur].head; i != -1; i = edges[i].next)
        {
            int y = edges[i].y;
            if(visited[y] == 0)
            {
                q.push(y);
                visited[y] = 1;
            }
        }
    }
    // for(int i = 0; i < number_of_nodes; i++)
    // {
    //     if(visited[i] == 0) 
    //     {
    //         for(int j = 0; j < number_of_edges; j++)
    //         {
    //             cout << edges[j].x << endl;
    //             cout << edges[j].y << endl;
    //         }
    //         return false;
    //     }
    // }
    return true;
}

bool Graph::is_strong_connected()
{
    return is_k_connected(1);
}

bool Graph::st_is_k_connected(int i, int j, int k)
{
    int fl = max_flow(i, j);
    if(fl < k){
        return false;
    }
    return true;
}

bool Graph::st_is_k_connected_byid(int i, int j, int k)
{
    return st_is_k_connected(i, j, k);
}

bool Graph::is_k_connected(int k)
{
    for(int i = 0; i < number_of_nodes; i++)
    {
        for(int j = 0; j < number_of_nodes; j++)
        {
            if(i == j || is_neighbor(i, j))
                continue;
            if(!st_is_k_connected(nodes[i].id, nodes[j].id, k))
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

void Graph::copy(Graph g)
{
    number_of_nodes = g.number_of_nodes;
    number_of_edges = g.number_of_edges;
    for(int i=0; i < number_of_nodes; i++)
    {
        nodes[i] = g.nodes[i];
    }
    for(int i=0; i < number_of_edges; i++)
    {
        edges[i] = g.edges[i];
    }
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


Graph rdgraph(int N, int k, int rmin, int rmax, int range, int i) //在range的范围内产生节点为N连通度为k的图
{
    while(1)
    {
        int z = (int)time(0);
        srand(i * z);
        Graph G(N);
        for(int i = 0; i < N; i++)
        {
            G.nodes[i].x_pos = (double)rand()/(double)RAND_MAX * range;
            G.nodes[i].y_pos = (double)rand()/(double)RAND_MAX * range;
            G.nodes[i].radii = rmin + (double)rand()/(double)RAND_MAX * (rmax - rmin);
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
                    G.add_edge(i, j);
                if(dis < G.nodes[j].radii)
                    G.add_edge(j, i);
            }
        }
        if(G.is_k_connected(k))
            return G;
    }
}

void Graph::write(string path)
{
    ofstream net(path, ios::out);
    net << number_of_nodes << " " << number_of_edges << endl;
    for(int i = 0; i < number_of_nodes; i++)
        net << nodes[i].id << " " << nodes[i].head << " " << nodes[i].x_pos << " " << nodes[i].y_pos << " " << nodes[i].radii << endl;
    for(int i = 0; i < number_of_edges; i++)
        net << edges[i].x << " " << edges[i].y << " " << edges[i].next << endl;
    net.close();
}

void Graph::read(string path)
{
    ifstream net(path, ios::in);
    if(!net) cout << "open error" << endl;
    int N; //节点数
    int M; //边数
    net >> N;
    net >> M;
    number_of_nodes = N;
    number_of_edges = M;
    for(int i = 0; i < N; i++)
    {
        float t;
        net >> nodes[i].id;
        net >> nodes[i].head;
        net >> t;
        net >> t;
        net >> t;
        //net >> t;
    }
    for(int i = 0; i < M; i++)
    {
        net >> edges[i].x;
        net >> edges[i].y;
        net >> edges[i].next;
    }
    net.close();
}


void genegraph(int N, int k, int rmin, int rmax, int range, int i)
{
    string savepath = "./graphdata/r" + to_string(rmin) + "_" + to_string(rmax) + "/k=" + to_string(k) + "/N=" + to_string(N) + "/";
    int start = time(0);
    string graphpath = savepath + "net" + to_string(i) + ".txt";
    Graph G = rdgraph(N, k, rmin, rmax, range, i);
    G.write(graphpath);
    int end = time(0);
    cout << "time:" << end - start << endl;
    // if (0 != access(savepath.c_str(), 0))
    // {
    //     cout << 1 << endl;
    //     cout << mkdir(savepath.c_str()) << endl;   // 返回 0 表示创建成功，-1 表示失败
    // }
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

vector<int> Graph::inneighbors(int i)
{
    Graph Gr = reverse();
    return Gr.outneighbors(i);
}

vector<int> Graph::bineighbors(int i)
{
    vector<int> s = v_inter(inneighbors(i), outneighbors(i));
    return s;
}

vector<int> Graph::all_nodes()
{
    vector<int> S;
    for(int i = 0; i < number_of_nodes; i++)
    {
        S.push_back(nodes[i].id);
    }
    return S;
}

vector<int> Graph::path(int s, int t)
{
    int i = id_to_address(s);
    int j = id_to_address(t);
    int dis[number_of_nodes];
    int parent[number_of_nodes];
    int vis[number_of_nodes];
    for(int z = 0; z < number_of_nodes; z++)
    {
        dis[z] = inf;
        parent[z] = -1;
        vis[z] = 0;
    }
    for(int z = nodes[i].head; z != -1; z = edges[z].next)
    {
        dis[edges[z].y] = 1;
        parent[edges[z].y] = i;
    }
    dis[i] = 0;
    vis[i] = 1;
    for(int z = 1; z < number_of_nodes; z++)
    {
        int minn=inf;
        int temp=-1;
		for(int q=0;q<number_of_nodes;q++)
		{
			if(!vis[q]&&dis[q]<minn)
			{
				minn=dis[q];
				temp=q;
			}
		}  
        if(temp == -1) break;   
        vis[temp] = 1;
        for(int e=nodes[temp].head;e!=-1;e = edges[e].next)
		{
			if(dis[temp] + 1 < dis[edges[e].y])
			{
				dis[edges[e].y] = dis[temp] + 1;
                parent[edges[e].y] = temp;
			}
		}
    }
    vector<int> temp;
    vector<int> path;
    if(parent[j] == -1)
    {
        path.push_back(s);
        return path;
    }
    else
    {
        for(int z = j; z != -1; z = parent[z])
        {
            temp.push_back(z);
        }
        for(int z = temp.size() - 1; z >= 0; z--)
        {
            path.push_back(nodes[temp[z]].id);
        }
        return path;
    }

}


double Graph::avepath_throu_T(vector<int> T)
{
    double sum = 0.0;
    for(int i = 0; i < number_of_nodes; i++)
    {
        for(int j = 0; j < number_of_nodes; j++)
        {
            if(i == j) continue;
            int dis[number_of_nodes];
            int parent[number_of_nodes];
            int vis[number_of_nodes];
            for(int z = 0; z < number_of_nodes; z++)
            {
                dis[z] = inf;
                parent[z] = -1;
                vis[z] = 0;
            }
            vector<int> _T = v_diff(all_nodes(), T);
            for(int t:_T)
            {
                int tt = id_to_address(t);
                vis[tt] = 1;
            }
            vis[j] = 1;
            for(int z = nodes[i].head; z != -1; z = edges[z].next)
            {
                dis[edges[z].y] = 1;
                parent[edges[z].y] = i;
            }
            dis[i] = 0;
            vis[i] = 1;
            for(int z = 1; z < number_of_nodes; z++)
            {
                int minn=inf;
                int temp=-1;
                for(int q=0;q<number_of_nodes;q++)
                {
                    if(!vis[q]&&dis[q]<minn)
                    {
                        minn=dis[q];
                        temp=q;
                    }
                }  
                if(temp == -1) break;   
                vis[temp] = 1;
                for(int e=nodes[temp].head;e!=-1;e = edges[e].next)
                {
                    if(dis[temp] + 1 < dis[edges[e].y])
                    {
                        dis[edges[e].y] = dis[temp] + 1;
                        parent[edges[e].y] = temp;
                    }
                }
            }
            vector<int> path;
            if(parent[j] == -1)
            {
                cout << "not connected" << endl;
            }
            else
            {
                for(int z = j; z != -1; z = parent[z])
                {
                    path.push_back(z);
                }
            }
            sum += (double)path.size();
        }
    }
    double ave = sum/(number_of_nodes * (number_of_nodes - 1));
    return ave;
}

separation Graph::a_separation(vector<int> s, int t)
{

    Graph tempG = subgraph(v_diff(all_nodes(), s));
    vector<int> G2; //连到t的
    G2.push_back(t);
    vector<int> G1; //连不到t的
    for(int i = 0; i < tempG.number_of_nodes; i++)
    {
        if(tempG.nodes[i].id == t) continue;
        vector<int> p = tempG.path(tempG.nodes[i].id, t);
        if(p.size() == 1)
        {
            G1.push_back(tempG.nodes[i].id);
        }
        else
        {
            G2.push_back(tempG.nodes[i].id);
        }
    }
    separation s_temp;
    s_temp.A1 = G1;
    s_temp.A2 = G2;
    return s_temp;
}

vector<int> Graph::Tpath(vector<int> D, vector<int> U, vector<int> separator, int t)
{   
    Graph D_U_induced = subgraph(v_cup(D, U));
    separation s1 = D_U_induced.a_separation(separator, t);
    Graph tempG = subgraph(v_diff(all_nodes(), separator));
    vector<int> src = v_inter(D, s1.A1);
    vector<int> tag = v_inter(D, s1.A2);
    vector<int> tpath;
    int maxx = inf;
    for(int srcnode:src)
    {
        for(int tagnode:tag)
        {
            vector<int> a_path = tempG.path(srcnode, tagnode);
            if(a_path.size() < maxx && a_path.size() != 1)
            {
                tpath = a_path;
                maxx = a_path.size();
            }
        }
    }
    return tpath; 
}

vector<int> Graph::myTpath(vector<int> D, vector<int> U, vector<int> separator, vector<int> C)
{   
    Graph tempG = subgraph(v_diff(all_nodes(), separator));
    vector<int> src = v_diff(D, C);
    src = v_diff(src, separator);
    vector<int> tag = C;
    vector<int> tpath;
    int maxx = inf;
    for(int srcnode:src)
    {
        for(int tagnode:tag)
        {
            vector<int> a_path = tempG.path(srcnode, tagnode);
            if(a_path.size() < maxx && a_path.size() != 1)
            {
                tpath = a_path;
                maxx = a_path.size();
            }
        }
    }
    return tpath; 
}



vector<vector<int>> Graph::find_all_k_block(int k)
{
    int flow[number_of_nodes][number_of_nodes];
    int s_adress[number_of_nodes][number_of_nodes]; //指向separation的地址
    int cnt=0;
    vector<separation> all_s;
    for(int i=0; i < number_of_nodes; i++)
    {
        for(int j = 0; j < number_of_nodes; j++)
        {
            if(i == j || is_neighbor(i, j))
            {
                flow[i][j] = 1;
            }
            else //构造一个separation
            {
                vector<int> cut = st_minimum_cut(nodes[i].id, nodes[j].id);
                if(cut.size() >= k) flow[i][j] = 1;
                else
                {
                    flow[i][j] = 0;
                    separation s_temp = a_separation(cut, nodes[j].id);
                    separation s_cur;
                    s_temp.A1 = v_cup(s_temp.A1, cut);
                    s_temp.A2 = v_cup(s_temp.A2, cut);
                    for(int z:s_temp.A1) s_cur.A1.push_back(id_to_address(z));
                    for(int z:s_temp.A2) s_cur.A2.push_back(id_to_address(z));
                    all_s.push_back(s_cur);
                    s_adress[i][j] = cnt;
                    cnt ++;
                }
            }
        }
    }
    vector<vector<int>> all_b;
    queue<vector<int>> Q;
    vector<int> X;
    for(int i=0; i<number_of_nodes; i++)
    {
        X.push_back(i);
    }
    Q.push(X);
    while(!Q.empty())
    {
        X = Q.front();
        Q.pop();
        if(X.size() < k) continue;
        bool flag = true;
        int a, b;
        for(int i:X)
        {
            if(!flag) break;
            for(int j:X)
            {
                if(!flow[i][j])
                {
                    flag = false;
                    a = i;
                    b = j;
                    break;
                }
            }
        }

        if(flag == true)
        {
            vector<int> b_temp;
            for(int i:X)
            {
                b_temp.push_back(nodes[i].id);
            }
            all_b.push_back(b_temp);
        }
        else
        {
            separation s0 = all_s[s_adress[a][b]];
            vector<int> A1 = v_inter(s0.A1, X);
            vector<int> A2 = v_inter(s0.A2, X);
            Q.push(A1);
            Q.push(A2);
        }
    }
    return all_b;
}



vector<int> Graph::find_k_block(int k)
{
    vector<vector<int>> all_b = find_all_k_block(k);
    int maxsize = 0;
    vector<int> block;
    for(vector<int> b:all_b)
    {
        if(b.size() > maxsize) 
        {
            block = b;
            maxsize = b.size();
        }
    }
    return block;
}

vector<int> Graph::find_k_block_contain(int k, vector<int> B)
{
    vector<vector<int>> all_b = find_all_k_block(k);
    vector<int> block = B;
    int maxsize = B.size();
    for(vector<int> b:all_b)
    {
        if(b.size() > maxsize) 
        {
            vector<int> temp = v_inter(b, B);
            if(temp.size() == B.size())
            {
                maxsize = b.size();
                block = b;
            }
        }
    }
    return block;
}
// int main(){
//     for(int k = 8; k > 2; k--)
//     {
//         for(int i=0; i<2; i++)
//         {
//             thread threads[50];
//             for (int j=0; j<50; j++)
//             {
//                 int t = i*50 + j; 
//                 threads[j] = thread(genegraph, 200, k, 300, 450, 1000, t);
//             }
//             for(int j=0; j<50; j++)
//             {
//                 threads[j].join();
//             }
//         }
//     }

//     for(int k = 6; k > 3; k--)
//     {
//         for(int N = 50; N <= 300; N += 50)
//         {
//             if(N == 200); continue;
//             for(int i=0; i<2; i++)
//             {
//                 thread threads[50];
//                 for (int j=0; j<50; j++)
//                 {
//                     int t = i*50 + j; 
//                     threads[j] = thread(genegraph, N, k, 300, 450, 1000, t);
//                 }
//                 for(int j=0; j<50; j++)
//                 {
//                     threads[j].join();
//                 }
//             }
//         }
//     }

//     for(int rmax = 375; rmax <= 675; rmax += 75)
//     {
//         if(rmax == 450) continue;
//         for(int k = 5; k > 2; k--)
//         {
//                 for(int i=0; i<2; i++)
//                 {
//                     thread threads[50];
//                     for (int j=0; j<50; j++)
//                     {
//                         int t = i*50 + j; 
//                         threads[j] = thread(genegraph, 200, k, 300, rmax, 1000, t);
//                     }
//                     for(int j=0; j<50; j++)
//                     {
//                         threads[j].join();
//                     }
//                 }
//         }
//     }
//     return 0;
// }