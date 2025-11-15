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
    Graph(int N, int M) // Generate network with N nodes and M edges
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
    // Unless otherwise specified, input and output are node addresses, not IDs
    vector<int> outneighbors(int i); // Output outgoing neighbors of node
    vector<int> inneighbors(int i); // Output incoming neighbors
    vector<int> bineighbors(int i); // Bidirectional neighbors
    int id_to_address(int i); // Convert ID to address
    bool is_neighbor(int i, int j); // Check if nodes i, j are adjacent
    bool is_strong_connected(); // Check if G is strongly connected
    bool is_connected(); // Check if G is connected
    bool st_is_k_connected(int s, int t, int k); // Check if nodes s, t are k-strongly connected
    bool is_k_connected(int k); // Check if G is k-strongly connected
    int max_flow(int i, int j); // Calculate max flow from i to j using Dinic algorithm
    bool bfs(int s, int t); // Layering phase of Dinic algorithm, returns false if s cannot reach t
    int dfs(int s, int t, int flow); // Use DFS to find augmenting path
    void write(string path);
    void read(string path);
    vector<int> st_minimum_cut(int i, int j);
    Graph flow_graph(); // Generate network for max flow calculation
    Graph subgraph(vector<int> C); // Generate subgraph based on set C
    Graph reverse(); // Generate reversed network
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
    for(int n = 0; n < number_of_nodes; n++) // Split nodes: 2n is input node, 2n+1 is output node
    {
        g.nodes[2*n].id = nodes[n].id;
        g.nodes[2*n].head = -1;
        g.nodes[2*n+1].id = nodes[n].id;
        g.nodes[2*n+1].head = -1;
    }
    for(int i = 0; i < number_of_edges; i++) // Split edges
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
    for(int n = 0; n < number_of_nodes; n++) // Add edges within nodes
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

bool Graph::bfs(int s, int t) // s is source, t is sink
{
    queue<int> Q;
    while (!Q.empty())
        Q.pop();
    //memset(depth, 0, sizeof(depth));
    for(int i = 0; i < number_of_nodes; i++) // Set depth, cannot use memset as it's a pointer
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
    if(u == t) // Reached sink, return
        return flow;
    for(int i = nodes[u].head; i != -1; i = edges[i].next)
    {
        if ((depth[edges[i].y]==depth[u]+1)&&(edges[i].capacity != 0)) // Must satisfy both layered graph and residual capacity > 0
        {
            int di = dfs(edges[i].y, t, min(flow, edges[i].capacity)); // Augment downward
            if (di>0) // If augmentation successful
            {
                edges[i].capacity -= di; // Decrease forward edge
                edges[i^1].capacity += di; // Increase reverse edge
                return di; // Propagate upward
            }
        }
    }
    return 0; // No augmenting path found, return 0
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
    vector<int> G2; // Nodes that can reach sink in residual network
    vector<int> G1; // Nodes that cannot reach sink in residual network
    vector<int> temp; // Temporary storage
    vector<int> cut; // Minimum vertex cut set
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
    // Generate nodes
    for(int i = 0; i < g.number_of_nodes; i++)
    {
        g.nodes[i].id = C[i];
        g.nodes[i].head = -1;
    }
    // Generate edges
    for(int i = 0; i < number_of_edges; i++)
    {
        int x = nodes[edges[i].x].id;
        int y = nodes[edges[i].y].id;
        if(v_has(C, x) && v_has(C, y)) // Both ends of edge are in C
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
    // Reverse edges
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


Graph rdgraph(int N, int r, int range, int i) // Generate connected graph with N nodes within range
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


void genegraphWithDegree(int N, int type){ // type: 0=smaller, 1=equal, 2=greater

    string graphType;
    int degree;
    switch(type){
        case 0: graphType = "smaller"; degree = N/4; break;
        case 1: graphType = "equal";  degree = N/2; break;
        case 2: graphType = "greater";  degree = N*3/4; break;
    }

    // Use new output path (relative to project root)
    string savepath = "./output/graphs/" + graphType + "/N=" + to_string(N) + "/";
    filesystem::create_directories(savepath);

    int t = 100;
    for(int i = 0; i < t; i++) // Generate t graphs
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
            if(tmp.size() == 0) continue; // Node cannot add more edges
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
    cout << "Graph Data Generation Program" << endl;
    cout << "====================================================" << endl;

    // Mode selection: 1=Unit disk random graph (R=250), 2=Specific degree graph (smaller/equal/greater)
    int mode = 1;

    cout << "\nSelect generation mode:" << endl;
    cout << "  1: Unit disk random graph (ex1~ex7)" << endl;
    cout << "  2: Specific degree graph (alpha scanning/Figure3)" << endl;
    cout << "Enter mode (default 1): ";

    string input;
    getline(cin, input);
    if (!input.empty()) {
        mode = stoi(input);
    }

    const int RANGE = 500;      // 500×500 plane

    if (mode == 1) {
        // ===== Mode 1: Generate Unit Disk Random Geometric Graph =====
        cout << "\n[Mode 1] Generate Unit Disk Random Geometric Graph (R=250)" << endl;
        cout << "Parameters: N=10~100, 100 samples per N" << endl;

        const int R = 250;          // Communication radius
        const int T_FULL = 100;     // 100 samples

        for (int N = 10; N <= 100; N += 10) {
            string base = "./output/graphs/R=" + to_string(R) + "/N=" + to_string(N) + "/";
            filesystem::create_directories(base);

            for (int i = 0; i < T_FULL; ++i) {
                genegraph(N, R, RANGE, i);
                cout << "  ✓ N=" << N << " net" << i << endl;
            }
            cout << "N=" << N << ": Complete" << endl;
        }

    } else if (mode == 2) {
        genegraphWithDegree(50, 0);
        genegraphWithDegree(50, 1);
        genegraphWithDegree(50, 2);
        genegraphWithDegree(100, 0);
        genegraphWithDegree(100, 1);
        genegraphWithDegree(100, 2);
    } else {
        cout << "Unknown mode: " << mode << endl;
        return 1;
    }

    cout << "\n====================================================" << endl;
    cout << "✓ Graph Generation Complete!" << endl;
    cout << "====================================================" << endl;
    return 0;
}