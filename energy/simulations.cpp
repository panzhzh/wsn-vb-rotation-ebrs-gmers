#include"graph.hpp"
#include<iostream>
#include<time.h>
#include<random>
#include<filesystem>
#include<omp.h>

using namespace std;

struct NInfor
{
    int id;
    int ifdom;
};


vector<int> alg1(Graph G, double* energy)
{
    int N = G.number_of_nodes;
    int ifdom[N];
    double weight[N];
    for(int i = 0; i < N; i++)
    {
        weight[i] = energy[i];
    }
    for(int i = 0; i < N; i++)
    {
        ifdom[i] = -1;
    }

    while(1)
    {
        for(int i = 0; i < N; i++)
        {
            if(ifdom[i] != -1) continue;
            double maxwei;
            int flag = 1;
            for(int j:G.outneighbors(i))
            {
                if(ifdom[j] == 1)
                {
                    flag = 0;
                    ifdom[i] = 0;
                    break;
                }
                if(weight[j] > weight[i])
                {
                    if(ifdom[j] != 0)
                    {
                        flag = 0;
                        break;
                    }
                     
                }
            }
            if(flag == 1)
            {
                ifdom[i] = 1;
                // for(int j:G.outneighbors(i))
                // {
                //     if(ifdom[j] == -1) ifdom[i] = 0;
                // }
                //break;
            } 
        }
        int b = 1;
        for(int i = 0; i < N; i++)
        {
            if(ifdom[i] == -1)
            {
                b = 0;
                break;
            }
        }
        if(b == 1) break;
    }


    vector<int> D;
    int color[N];
    for(int i = 0; i < N; i++)
    {
        if(ifdom[i] == 1)
        {
            D.push_back(i);
            color[i] = 1;
        }
        else color[i] = 2;
    }

    // cout << D.size() << endl;
    // for(int i = 0; i < N; i++)
    // {
    //     if(ifdom[i] == 1) continue;
    //     if(v_inter(G.outneighbors(i), D).size() == 0)
    //     {
    //         cout << "error" << endl;
    //     }
    // }
    
    int component[N] = {0};
    int c_count = 0;

    for(int i:D)
    {
        if(component[i] == 0) component[i] = ++c_count;
        else
        {
            for(int j:G.outneighbors(i))
            {
                if(component[j] == 0) component[j] = component[i];
                else if(component[j] != component[i]) 
                {
                    for(int z:D)
                    {
                        if(component[z] == component[j]) component[z] = component[i];
                    }
                }
            }
        }

    }



    while(1)
    {
        vector<int> contain[N];
        for(int i = 0; i < N; i++)
        {
            if(color[i] == 2)
            {
                for(int j:G.outneighbors(i))
                {
                    if(color[j] == 1 || color[j] == 3) contain[i].push_back(component[j]);
                }
                
            }
            contain[i] = v_uni(contain[i]);
        }
        int maxadj = 1;
        int key = -1;
        for(int i = 0; i < N; i++)
        {
            if(contain[i].size() > maxadj)
            {
                maxadj = contain[i].size();
                key = i;
            }
        }
        if(maxadj > 1)
        {
            color[key] = 3;
            component[key] = contain[key][0];
            D.push_back(key);
            for(int i = 0; i < N; i++)
            {
                if(v_has(contain[key], component[i])) component[i] = contain[key][0];
            }
        }
        else
        {
            int key1 = -1;
            int key2 = -1;
            for(int i = 0; i < N; i++)
            {
                if(contain[i].size() != 1) continue;
                for(int j:G.outneighbors(i))
                {
                    if(contain[j].size() != 1) continue;
                    if(contain[i][0] != contain[j][0])
                    {
                        maxadj = 2;
                        key1 = i;
                        key2 = j;
                        break;
                    }
                }
                if(maxadj == 2) break;
            }
            if(maxadj == 2) 
            {
                color[key1] = 3;
                color[key2] = 3;
                D.push_back(key1);
                D.push_back(key2);
                component[key1] = contain[key1][0];
                component[key2] = contain[key1][0];
                for(int i = 0; i < N; i++)
                {
                    if(component[i] == contain[key2][0]) component[i] = contain[key1][0];
                }
            }
            else break;
        }
    }
    // cout << D.size() << endl;
    // Graph sub = G.subgraph(D);
    // if(sub.is_connected()) cout << 1 << endl;
    // else cout << 2 << endl;
    return D;
    // {

    //     vector<int> contain[N];
    //     for(int i = 0; i < N; i++)
    //         {
    //             if(color[i] == 2)
    //             {
    //                 for(int j:G.outneighbors(i))
    //                 {
    //                     if(color[j] == 1 || color[j] == 3) contain[i].push_back(component[j]);
    //                 }
                    
    //             }
    //             contain[i] = v_uni(contain[i]);
    //         }
    //     // for(int i = 0; i < N; i++)
    //     // {
    //     //     cout << component[i] << endl;
    //     //     if(contain[i].size() > 1) 
    //     //     {
    //     //         for(int j:contain[i]) cout << j << endl;
    //     //     }
    //     // }
    //     int visited[N] = {0};
    //     queue<int> q;
    //     q.push(D[0]);
    //     visited[D[0]] = 1;
    //     while(!q.empty())
    //     {
    //         int cur = q.front();
    //         q.pop();
    //         for(int j:G.outneighbors(cur))
    //         {
    //             if(visited[j] == 0)
    //             {
    //                 visited[j] = 1;
    //                 q.push(j);
    //             }
    //         }
    //     }
    //     for(int i:D)
    //     {
    //         if(visited[i] == 0) cout << 1 << endl;
    //     }
    //     cout << 2 << endl;
    // }
    


    // for(int i = 0; i < N; i++)
    // {
    //     if(ifdom[i] != 1) continue;
    //     D.push_back(i);
    //     for(int j = i+1; j < N; j++)
    //     {
    //         if(ifdom[j] != 1) continue;
    //         vector<int> p = G.path(i, j);
    //         if(p.size() < 5) D = v_cup(D, p);
    //     }
    // }

    // D = v_uni(D);



    // vector<int> temp;
    // for(int i = 0; i < N; i++)
    // {
    //     int flag = 0;
    //     if(ifdom[i] == 1)
    //     {
    //         for(int j:G.outneighbors(i))
    //         {
    //             for(int z:G.outneighbors(j))
    //             {
    //                 if(z == i) continue;
    //                 else if(ifdom[z] == 1)
    //                 {
    //                     flag = 1;
    //                     break;
    //                 } 
    //             }
    //             if(flag == 1) break;
    //         }
    //         if(flag == 0)
    //         {
    //             for(int j:G.outneighbors(i))
    //             {
    //                 for(int u:G.outneighbors(j))
    //                 {
    //                     if(u == i) continue;
    //                     for(int v:G.outneighbors(u))
    //                     {
    //                         if(v == i || v == j) continue;
    //                         else if(ifdom[v] == 1)
    //                         {
    //                             temp.push_back(u);
    //                             flag = 1;
    //                             break;
    //                         }
    //                     }
    //                     if(flag == 1) break;
    //                 }
    //                 if(flag == 1) break;
    //             }
    //         }
    //     }
    // }
    // for(int i:temp)
    // {
    //     ifdom[i] = 1;
    // }

    // int r;
    // for(int i = 0; i < N; i++)
    // {
    //     if(ifdom[i] == 1) 
    //     {
    //         r = i;
    //         break;
    //     }
    // }


    // while(1)
    // {
    //     if(ifdom[r] == 1)
    //     {
    //     }
    // }


    // return D;
}

double minlevel(Graph G, double *energy)
{
    int N = G.number_of_nodes;
    double curlevel = 0;
    double result = 0;

    while(1)
    {
        vector<int> greater;
        vector<int> less;
        double nextlevel = 999999.0;
        for(int i = 0; i < N; i++)
        {
            if(energy[i] >= curlevel)
            {
                greater.push_back(i);
                if(energy[i] != curlevel && energy[i] < nextlevel)
                {
                    nextlevel = energy[i];
                }
            }     
            else less.push_back(i);    
        }
        Graph sub = G.subgraph(greater);
        if(sub.number_of_nodes == 0 || !sub.is_connected())
        {
            return result;
        }
        for(int i:less)
        {
            vector<int> cap = v_inter(G.outneighbors(i), greater);
            if(cap.size() == 0) return result;
        }
        result = curlevel;
        curlevel = nextlevel;
        if(nextlevel == 9999999) return result;
    }
}

vector<int> myalg(Graph G, double *energy)
{
    int N = G.number_of_nodes;
    double l = minlevel(G, energy);
    //cout << l << endl;
    vector<int> greater;
    vector<int> less;
    for(int i = 0; i < N; i++)
    {
        if(energy[i] >= l)
        {
            greater.push_back(i);
        }
        else less.push_back(i);
    }
    //int visited[N] = {0};
    // for(int i = 0; i < N; i++)
    // {
    //     if(v_has(i)) visited[i] = 1;
    //     else visited[i] = 0;
    // }
    //int dom[N];
    
    int color[N] = {0}; 
    vector<int> D;
    while(1)
    {
        int key;
        int maxdom = 0;
        for(int i:greater)
        {
            int idom = 0;
            if(color[i] == 1) continue;
            if(color[i] == 0) idom++;
            for(int j:G.outneighbors(i))
            {
                if(color[j] == 0) idom++;
            }
            if(idom > maxdom)
            {
                key = i;
                maxdom = idom;
            }
        }
        D.push_back(key);
        color[key] = 1;
        for(int j:G.outneighbors(key))
        {
            if(color[j] == 0) color[j] = 2;
        } 
        int flag = 1;
        for(int i = 0; i < N; i++)
        {
            if(color[i] == 0) 
            {
                // cout << i << endl;
                // if(v_has(greater, i)) cout << "you" << endl;
                // else cout << "wu" << endl;
                // cout << v_inter(G.outneighbors(i), greater).size() << endl;
                flag = 0;
                break;
            }
        }
        if(flag) break;
    }


    int component[N] = {0};
    int c_count = 0;
    

    for(int i:D)
    {
        if(component[i] == 0) component[i] = ++c_count;
        else
        {
            for(int j:G.outneighbors(i))
            {
                if(component[j] == 0) component[j] = component[i];
                else if(component[j] != component[i]) 
                {
                    for(int z:D)
                    {
                        if(component[z] == component[j]) component[z] = component[i];
                    }
                }
            }
        }

    }


    while(1)
    {
        vector<int> contain[N];
        for(int i = 0; i < N; i++)
        {
            if(color[i] == 2 && energy[i] >= l)
            {
                for(int j:G.outneighbors(i))
                {
                    if(color[j] == 1 || color[j] == 3) contain[i].push_back(component[j]);
                }
                
            }
            contain[i] = v_uni(contain[i]);
        }
        int maxadj = 1;
        int key = -1;
        for(int i = 0; i < N; i++)
        {
            if(contain[i].size() > maxadj)
            {
                maxadj = contain[i].size();
                key = i;
            }
        }
        if(maxadj > 1)
        {
            color[key] = 3;
            component[key] = contain[key][0];
            D.push_back(key);
            for(int i = 0; i < N; i++)
            {
                if(v_has(contain[key], component[i])) component[i] = contain[key][0];
            }
        }
        else
        {
            int key1 = -1;
            int key2 = -1;
            for(int i = 0; i < N; i++)
            {
                if(contain[i].size() != 1) continue;
                for(int j:G.outneighbors(i))
                {
                    if(contain[j].size() != 1) continue;
                    if(contain[i][0] != contain[j][0])
                    {
                        maxadj = 2;
                        key1 = i;
                        key2 = j;
                        break;
                    }
                }
                if(maxadj == 2) break;
            }
            if(maxadj == 2) 
            {
                color[key1] = 3;
                color[key2] = 3;
                D.push_back(key1);
                D.push_back(key2);
                component[key1] = contain[key1][0];
                component[key2] = contain[key1][0];
                for(int i = 0; i < N; i++)
                {
                    if(component[i] == contain[key2][0]) component[i] = contain[key1][0];
                }
            }
            else break;
        }
    }

    // Graph sub = G.subgraph(D);
    // if(sub.is_connected()) cout << 1 << endl;
    // else cout << 2 << endl;
    return D;
}


double consume(int type)
{
    if(type == 1) return 1;
    else
    {
        // int r = rand()%100;
        // if(r < 40) return 0.44;
        // else if(r < 72) return 0.75;
        // else if(r < 88) return 1.5;
        // else if(r < 97) return 2.0;
        // else return 4.0;

        double pV = 0.0;
        double lambda = 1;
        while(true)
        {
            pV = (double)rand()/(double)RAND_MAX;
            if (pV != 1)
            {
                break;
            }
        }
        pV = (-1.0/lambda)*log(1-pV);
        return pV;
    }
}


vector<int> minw(Graph G, double *energy)
{
    double a = 0.7;
    int N = G.number_of_nodes;
    int color[N] = {0};
    double weight[N];
    for(int i = 0; i < N; i++)
    {
        weight[i] = 1.0/energy[i];
    }
    vector<int> D;
    double minv = 9999999;
    int key = -1;
    for(int i = 0; i < N; i++)
    {
        double isum = weight[i];
        double v1 = 0; 
        double v2 = 0;
        for(int j:G.outneighbors(i))
        {
            if(color[j] == 0) isum += weight[j];
            if(color[j] == 0) v2 ++;            
        }
        v1 = weight[i]/isum;
        double value = pow(v1, a)/pow(v2, 1-a);
        if(value < minv)
        {
            key = i;
            minv = value;
        }
    }
    color[key] = 1;
    for(int i:G.outneighbors(key)) 
    {
        if(color[i] == 0) color[i] = 2;
    }
        
    while(1)
    {        
        int flag = 0;
        for(int i = 0; i < N; i++)
        {
            if(color[i] == 0) flag = 1;
        }
        if(!flag) break;
        double minv = 99999999;
        int key = -1; 
        for(int i = 0; i < N; i++)
        {
            if(color[i] == 2)
            {
                double isum = weight[i];
                double v1 = 0; 
                double v2 = 0;
                for(int j:G.outneighbors(i))
                {
                    if(color[j] == 0) isum += weight[j];
                    if(color[j] == 0) v2 ++;            
                }
                v1 = weight[i]/isum;
                double value = pow(v1, a)/pow(v2, 1-a);
                if(value < minv)
                {
                    key = i;
                    minv = value;
                }
            }
        }
        color[key] = 1;
        for(int i:G.outneighbors(key)) 
        {
            if(color[i] == 0) color[i] = 2;
        }

    }
    for(int i = 0; i < N; i++)
    {
        if(color[i] == 1) D.push_back(i);
    }
    return D;
}



vector<int> minw2(Graph G, double *energy)
{
    double a = 0.7;
    int N = G.number_of_nodes;
    int color[N] = {0};
    double weight[N];
    for(int i = 0; i < N; i++)
    {
        weight[i] = 1.0/energy[i];
    }
    vector<int> D;
    double minv = 9999999;
    int key = -1;
    for(int i = 0; i < N; i++)
    {
        double isum = weight[i];
        double v1 = 0; 
        double v2 = 0;
        for(int j:G.outneighbors(i))
        {
            if(color[j] == 0) isum += weight[j];
            if(color[j] == 0) v2 ++;            
        }
        v1 = weight[i]/isum;
        double value = pow(v1, a) * pow(v2, 1 - a);
        if(value < minv)
        {
            key = i;
            minv = value;
        }
    }
    color[key] = 1;
    for(int i:G.outneighbors(key)) 
    {
        if(color[i] == 0) color[i] = 2;
    }
        
    while(1)
    {        
        int flag = 0;
        for(int i = 0; i < N; i++)
        {
            if(color[i] == 0) flag = 1;
        }
        if(!flag) break;
        double minv = 99999999;
        int key = -1; 
        for(int i = 0; i < N; i++)
        {
            if(color[i] == 2)
            {
                double isum = weight[i];
                double v1 = 0; 
                double v2 = 0;
                for(int j:G.outneighbors(i))
                {
                    if(color[j] == 0) isum += weight[j];
                    if(color[j] == 0) v2 ++;            
                }
                v1 = weight[i]/isum;
                double value = pow(v1, a)/pow(v2, a);
                if(value < minv)
                {
                    key = i;
                    minv = value;
                }
            }
        }
        color[key] = 1;
        for(int i:G.outneighbors(key)) 
        {
            if(color[i] == 0) color[i] = 2;
        }

    }
    for(int i = 0; i < N; i++)
    {
        if(color[i] == 1) D.push_back(i);
    }
    return D;
}


bool isCDS(Graph G, vector<int> D)
{
    int N = G.number_of_nodes;
    for(int i = 0; i < N; i++)
    {
        if(v_inter(G.outneighbors(i), D).size() == 0) return false;
    }
    if(!G.subgraph(D).is_connected()) return false;
    return true;
}

int sim(Graph G, int alg, int type)
{
    int N = G.number_of_nodes;
    double energy[N];
    int sum = 0;
    int z = (int)time(NULL);
    srand(z);   
    for(int j = 0; j < N; j++)
    {
        energy[j] = 100;
    }
    int t = 0;
    int size = 0;
    while(1)
    {
        int flag = 1;
        for(int j = 0; j < N; j++)
        {
            if(energy[j] <= 0) 
            {
                //cout << i << endl;
                flag = 0;
            }
        }
        if(!flag) break;
        vector<int> D;
        if(alg == 0) D = myalg(G, energy);
        if(alg == 1) D = alg1(G, energy);
        if(alg == 2) D = minw(G, energy);
        //cout << D.size() << endl;
        for(int j:D)
        {
            energy[j] -= consume(type);
        }
        t++;
        size += D.size();
        //cout << t << endl;
        // if(t%30 == 0) 
        // {
        //     cout << t << endl;
        // }
    }
    sum += t;
    size /= sum;
    // cout << sum << endl;
    // getchar();
    return size;
    // return size;
    // int t = 0; //时间
    // while(1)
    // {
    //     t += 1;
    //     vector<int> D = alg1(G);
    //     for(int i = 0; i < N; i++)
    //     {
    //         if(v_has(D, i))
    //         {
    //             energy[i] -= consume();
    //         }
    //         else energy[i] -= 1;
    //         if(energy[i] <= 0) break;
    //     }
    // }
    // cout << t << endl;
}


int simulate(int N, int t1, int t2, int type)
{
    int result[3] = {0};
    for(int i = 0; i < t1; i++)
    {
        int iresult[3] = {0};
        cout << "i" << i << "start" << endl;
        string readpath = "./graph/N=" + to_string(N) + "/net" + to_string(i) + ".txt";
        Graph G;
        G.read(readpath);
        //G.read("net2.txt");
        for(int j = 0; j < t2; j++)
        {   
            cout << "j" << j << "start" << endl;
            iresult[0] += sim(G, 0, type); cout << iresult[0] << endl;
            iresult[1] += sim(G, 1, type); cout << iresult[1] << endl;
            iresult[2] += sim(G, 2, type); cout << iresult[2] << endl;
        }
        for(int j = 0; j < 3; j++)
        {
            iresult[j] /= t2;
            result[j] += iresult[j];
        }
    }
    for(int j = 0; j < 3; j++)
    {
        result[j] /= t1;
        cout << result[j] << endl;
    }
    return 0;
}

void ex1(int Nmin, int Nmax, int t, int R) //t即某种节点数量的图数量
{
    string resultpath = "./output/ex1/"; //存储结果的文件夹
    for(int mod = 0; mod <= 1; mod++) //两种耗能模式
    {
        for(int N = Nmin; N <= Nmax; N+=10) //节点数，即横坐标
        {
            cout << "N=" << N << "start" << endl;
            int result[2][t]; //存储两个算法在某个图下的结果
            #pragma omp parallel for
            for(int i = 0; i < t; i++) //循环次数，即一个节点的图的数量
            {
                string readpath = "./output/graphs/R=" + to_string(R) + "/N=" + to_string(N) + "/net" + to_string(i) + ".txt"; //读图地址
                Graph G;
                G.read(readpath);
                for(int alg = 0; alg < 2; alg++) //三种算法
                {
                    double energy[N];
                    int sum = 0;
                    int z = (int)time(NULL);
                    srand(z);   
                    for(int j = 0; j < N; j++)
                    {
                        energy[j] = 100; //设定初始能量
                    }
                    //int size = 0;
                    int livetime = 0; //生存时间
                    while(1)
                    {
                        int flag = 1;
                        for(int j = 0; j < N; j++)
                        {
                            if(energy[j] <= 0) 
                            {
                                //cout << i << endl;
                                flag = 0;
                            }
                        }
                        if(!flag) break;
                        vector<int> D;
                        if(alg == 0) D = myalg(G, energy);
                        //if(alg == 1) D = alg1(G, energy);
                        if(alg == 1) D = minw(G, energy);
                        //cout << D.size() << endl;
                        for(int j:D)
                        {
                            energy[j] -= consume(mod); //按不同模式消耗能量
                        }
                        livetime++;
                    }
                    result[alg][i] = livetime;
                }
            }
            string savepath = resultpath + "R" + to_string(R) +  "mod" + to_string(mod) + "N" + to_string(N) + ".txt";
            ofstream f(savepath, ios::out);
            if(!f) cout << "write error" << endl;
            for(int i = 0; i < t; i++)
            {
                f << result[0][i] << " " << result[1][i] << endl; //输出结果，一个mode下的一个N占具一个txt
            }
            f.close();
        }
    }
}

//实验2：
//两种算法在不同初始能量下的生存时间
void ex2(int Nmin, int Nmax, int t, int R) //t即某种节点数量的图数量
{
    string resultpath = "./output/ex2/"; //存储结果的文件夹
    for(int mod = 0; mod <= 1; mod++) //1种耗能模式
    {
        for(int N = Nmin; N <= Nmax; N+=10) //节点数，即横坐标
        {
            cout << "N=" << N << "start" << endl;
            int result[2][t]; //存储两个算法在某个图下的结果
            #pragma omp parallel for
            for(int i = 0; i < t; i++) //循环次数，即一个节点的图的数量
            {
                string readpath = "./output/graphs/R=" + to_string(R) + "/N=" + to_string(N) + "/net" + to_string(i) + ".txt"; //读图地址
                Graph G;
                G.read(readpath);
                for(int alg = 0; alg < 2; alg++) //三种算法
                {
                    double energy[N];
                    int sum = 0;
                    int z = (int)time(NULL);
                    srand(z);   
                    for(int j = 0; j < N; j++)
                    {
                        energy[j] = rand()%50 + 50; //设定初始能量, 在[50, 100]
                    }
                    //int size = 0;
                    int livetime = 0; //生存时间
                    while(1)
                    {
                        int flag = 1;
                        for(int j = 0; j < N; j++)
                        {
                            if(energy[j] <= 0) 
                            {
                                //cout << i << endl;
                                flag = 0;
                            }
                        }
                        if(!flag) break;
                        vector<int> D;
                        if(alg == 0) D = myalg(G, energy);
                        //if(alg == 1) D = alg1(G, energy);
                        if(alg == 1) D = minw(G, energy);
                        //cout << D.size() << endl;
                        for(int j:D)
                        {
                            energy[j] -= consume(mod); //按不同模式消耗能量
                        }
                        livetime++;
                    }
                    result[alg][i] = livetime;
                }
            }
            string savepath = resultpath + "R" + to_string(R) + "mod" + to_string(mod) + "N" + to_string(N) + ".txt";
            ofstream f(savepath, ios::out);
            if(!f) cout << "write error" << endl;
            for(int i = 0; i < t; i++)
            {
                f << result[0][i] << " " << result[1][i] << endl; //输出结果，一个mode下的一个N占具一个txt
            }
            f.close();
        }
    }
}


//实验3：
//CDS size 对比
void ex3(int Nmin, int Nmax, int t, int R) //t即某种节点数量的图数量
{
    string resultpath = "./output/ex3/"; //存储结果的文件夹
    for(int mod = 0; mod <= 1; mod++) //1种耗能模式
    {
        for(int N = Nmin; N <= Nmax; N+=10) //节点数，即横坐标
        {
            cout << "N=" << N << "start" << endl;
            int result[2][t]; //存储两个算法在某个图下的结果
            #pragma omp parallel for
            for(int i = 0; i < t; i++) //循环次数，即一个节点的图的数量
            {
                string readpath = "./output/graphs/R=" + to_string(R) + "/N=" + to_string(N) + "/net" + to_string(i) + ".txt"; //读图地址
                Graph G;
                G.read(readpath);
                for(int alg = 0; alg < 2; alg++) //三种算法
                {
                    double energy[N];
                    int sum = 0;
                    int z = (int)time(NULL);
                    srand(z);   
                    for(int j = 0; j < N; j++)
                    {
                        energy[j] = 100; //设定初始能量
                    }
                    int size = 0;
                    int livetime = 0; //生存时间
                    while(1)
                    {
                        int flag = 1;
                        for(int j = 0; j < N; j++)
                        {
                            if(energy[j] <= 0) 
                            {
                                //cout << i << endl;
                                flag = 0;
                            }
                        }
                        if(!flag) break;
                        vector<int> D;
                        if(alg == 0) D = myalg(G, energy);
                        //if(alg == 1) D = alg1(G, energy);
                        if(alg == 1) D = minw(G, energy);
                        //cout << D.size() << endl;
                        size += D.size();
                        for(int j:D)
                        {
                            energy[j] -= consume(mod); //按不同模式消耗能量
                        }
                        livetime++;
                    }
                    result[alg][i] = size /= livetime;
                }
            }
            string savepath = resultpath + "R" + to_string(R) + "mod" + to_string(mod) + "N" + to_string(N) + ".txt";
            ofstream f(savepath, ios::out);
            if(!f) cout << "write error" << endl;
            for(int i = 0; i < t; i++)
            {
                f << result[0][i] << " " << result[1][i] << endl; //输出结果，一个mode下的一个N占具一个txt
            }
            f.close();
        }
    }
}




//实验4：
//time
void ex4(int Nmin, int Nmax, int t, int R) //t即某种节点数量的图数量
{
    string resultpath = "./output/ex4/"; //存储结果的文件夹
    for(int mod = 0; mod <= 0; mod++) //1种耗能模式
    {
        for(int N = Nmin; N <= Nmax; N+=10) //节点数，即横坐标
        {
            cout << "N=" << N << "start" << endl;
            int result[2][t]; //存储两个算法在某个图下的结果
            #pragma omp parallel for
            for(int i = 0; i < t; i++) //循环次数，即一个节点的图的数量
            {
                string readpath = "./output/graphs/R=" + to_string(R) + "/N=" + to_string(N) + "/net" + to_string(i) + ".txt"; //读图地址
                Graph G;
                G.read(readpath);
                for(int alg = 0; alg < 2; alg++) //三种算法
                {
                    double energy[N];
                    int sum = 0;
                    int z = (int)time(NULL);
                    srand(z);   
                    for(int j = 0; j < N; j++)
                    {
                        energy[j] = 100; //设定初始能量
                    }
                    int runtime = 0;
                    int size = 0;
                    int livetime = 0; //生存时间
                    time_t starttime = time(0);
                    while(1)
                    {
                        int flag = 1;
                        for(int j = 0; j < N; j++)
                        {
                            if(energy[j] <= 0) 
                            {
                                //cout << i << endl;
                                flag = 0;
                            }
                        }
                        if(!flag) break;
                        vector<int> D;
                        if(alg == 0) D = myalg(G, energy);
                        //if(alg == 1) D = alg1(G, energy);
                        if(alg == 1) D = minw(G, energy);
                        //cout << D.size() << endl;
                        
                        size += D.size();
                        for(int j:D)
                        {
                            energy[j] -= consume(mod); //按不同模式消耗能量
                        }
                        livetime++;
                    }
                    time_t endtime = time(0);
                    runtime += endtime - starttime;
                    result[alg][i] = runtime /= livetime;
                }
            }
            string savepath = resultpath + "R" + to_string(R) + "mod" + to_string(mod) + "N" + to_string(N) + ".txt";
            ofstream f(savepath, ios::out);
            if(!f) cout << "write error" << endl;
            for(int i = 0; i < t; i++)
            {
                f << result[0][i] << " " << result[1][i] << endl; //输出结果，一个mode下的一个N占具一个txt
            }
            f.close();
        }
    }
}



//实验5：
//能量均衡性
void ex5(int Nmin, int Nmax, int t, int R) //t即某种节点数量的图数量
{
    string resultpath = "./output/ex5/"; //存储结果的文件夹
    for(int mod = 0; mod <= 0; mod++) //1种耗能模式
    {
        for(int N = Nmin; N <= Nmax; N+=10) //节点数，即横坐标
        {
            cout << "N=" << N << "start" << endl;
            int result[2][t]; //存储两个算法在某个图下的结果
            #pragma omp parallel for
            for(int i = 0; i < t; i++) //循环次数，即一个节点的图的数量
            {
                string readpath = "./output/graphs/R=" + to_string(R) + "/N=" + to_string(N) + "/net" + to_string(i) + ".txt"; //读图地址
                Graph G;
                G.read(readpath);
                for(int alg = 0; alg < 2; alg++) //两种算法
                {
                    double energy[N];
                    int sum = 0;
                    int z = (int)time(NULL);
                    srand(z);   
                    for(int j = 0; j < N; j++)
                    {
                        energy[j] = 100; //设定初始能量
                    }
                    int runtime = 0;
                    int size = 0;
                    int livetime = 0; //生存时间
                    double totalEnergy = 0; //能量总和
                    double averEnergy = 0; //平均能量
                    double varEnergy = 0; //标准方差
                    vector<double> vars; //储存各个时间帧的方差
                    time_t starttime = time(0);
                    while(1)
                    {
                        totalEnergy = 0;
                        int flag = 1;
                        for(int j = 0; j < N; j++)
                        {
                            totalEnergy += energy[j]; //计算能量综合
                            if(energy[j] <= 0) 
                            {
                                //cout << i << endl;
                                flag = 0;
                                break;
                            }
                        }

                        if(!flag) break;

                        averEnergy = totalEnergy / N; //计算平均能量

                        //计算标准方差
                        for(int j = 0; j < N; j++){
                            varEnergy += ((double)energy[j] - averEnergy) * ((double)energy[j] - averEnergy);
                        }
                        varEnergy /= (N - 1);
                        varEnergy = sqrt(varEnergy);
                        vars.push_back(varEnergy);

                        vector<int> D;
                        if(alg == 0) D = myalg(G, energy);
                        //if(alg == 1) D = alg1(G, energy);
                        if(alg == 1) D = minw(G, energy);
                        //cout << D.size() << endl;
                        
                        size += D.size();
                        for(int j:D)
                        {
                            energy[j] -= consume(mod); //按不同模式消耗能量
                        }                        
                        livetime++;
                    }
                    time_t endtime = time(0);
                    runtime += endtime - starttime;
                    result[alg][i] = runtime /= livetime;
                    string savepath = resultpath + "R" +  to_string(R) +  "alg" + to_string(alg) + "mod" + to_string(mod) +  "N" + to_string(N) + "net" + to_string(i) + ".txt";
                    ofstream f(savepath, ios::out);
                    if(!f) cout << "write error" << endl;
                    for(int i = 0; i < vars.size(); i++){
                        f << vars[i] << endl;
                    }
                    f.close();
                }
            }
            //string savepath = resultpath + "R" + to_string(R) + "mod" + to_string(mod) + "N" + to_string(N) + ".txt";
            //ofstream f(savepath, ios::out);
            // if(!f) cout << "write error" << endl;
            // for(int i = 0; i < t; i++)
            // {
            //     f << result[0][i] << " " << result[1][i] << endl; //输出结果，一个mode下的一个N占具一个txt
            // }
            // f.close();
        }
    }
}


//实验6：
//CDS size 对比 基于不均能量分布
void ex6(int Nmin, int Nmax, int t, int R) //t即某种节点数量的图数量
{
    string resultpath = "./output/ex6/"; //存储结果的文件夹
    for(int mod = 0; mod <= 1; mod++) //1种耗能模式
    {
        for(int N = Nmin; N <= Nmax; N+=10) //节点数，即横坐标
        {
            cout << "N=" << N << "start" << endl;
            int result[2][t]; //存储两个算法在某个图下的结果
            #pragma omp parallel for
            for(int i = 0; i < t; i++) //循环次数，即一个节点的图的数量
            {
                string readpath = "./output/graphs/R=" + to_string(R) + "/N=" + to_string(N) + "/net" + to_string(i) + ".txt"; //读图地址
                Graph G;
                G.read(readpath);
                for(int alg = 0; alg < 2; alg++) //三种算法
                {
                    double energy[N];
                    int sum = 0;
                    int z = (int)time(NULL);
                    srand(z);   
                    for(int j = 0; j < N; j++)
                    {
                        energy[j] = rand()%50 + 50; //设定初始能量, 在[50, 100]
                    }
                    int size = 0;
                    int livetime = 0; //生存时间
                    while(1)
                    {
                        int flag = 1;
                        for(int j = 0; j < N; j++)
                        {
                            if(energy[j] <= 0) 
                            {
                                //cout << i << endl;
                                flag = 0;
                            }
                        }
                        if(!flag) break;
                        vector<int> D;
                        if(alg == 0) D = myalg(G, energy);
                        //if(alg == 1) D = alg1(G, energy);
                        if(alg == 1) D = minw(G, energy);
                        //cout << D.size() << endl;
                        size += D.size();
                        for(int j:D)
                        {
                            energy[j] -= consume(mod); //按不同模式消耗能量
                        }
                        livetime++;
                    }
                    result[alg][i] = size /= livetime;
                }
            }
            string savepath = resultpath + "R" + to_string(R) + "mod" + to_string(mod) + "N" + to_string(N) + ".txt";
            ofstream f(savepath, ios::out);
            if(!f) cout << "write error" << endl;
            for(int i = 0; i < t; i++)
            {
                f << result[0][i] << " " << result[1][i] << endl; //输出结果，一个mode下的一个N占具一个txt
            }
            f.close();
        }
    }
}


//实验7：
//每个时间窗口变化的节点个数
void ex7(int Nmin, int Nmax, int t, int R) //t即某种节点数量的图数量
{
    string resultpath = "./output/ex7/"; //存储结果的文件夹
    for(int mod = 1; mod <= 1; mod++) //1种耗能模式
    {
        for(int N = Nmin; N <= Nmax; N+=10) //节点数，即横坐标
        {
            cout << "N=" << N << "start" << endl;
            double result[2][t]; //存储两个算法在某个图下的结果
            #pragma omp parallel for
            for(int i = 0; i < t; i++) //循环次数，即一个节点的图的数量
            {
                string readpath = "./output/graphs/R=" + to_string(R) + "/N=" + to_string(N) + "/net" + to_string(i) + ".txt"; //读图地址
                Graph G;
                G.read(readpath);
                for(int alg = 0; alg < 2; alg++) //三种算法
                {
                    double energy[N];
                    int sum = 0;
                    int z = (int)time(NULL);
                    srand(z);   
                    for(int j = 0; j < N; j++)
                    {
                        energy[j] = rand()%50 + 50; //设定初始能量, 在[50, 100]
                    }
                    int size = 0;
                    int livetime = 0; //生存时间
                    vector<int> pre;
                    double diffSum = 0.0;
                    while(1)
                    {
                        int flag = 1;
                        for(int j = 0; j < N; j++)
                        {
                            if(energy[j] <= 0) 
                            {
                                //cout << i << endl;
                                flag = 0;
                            }
                        }
                        if(!flag) break;
                        vector<int> D;
                        if(alg == 0) D = myalg(G, energy);
                        //if(alg == 1) D = alg1(G, energy);
                        if(alg == 1) D = minw(G, energy);
                        //cout << D.size() << endl;
                        size += D.size();
                        for(int j:D)
                        {
                            energy[j] -= consume(mod); //按不同模式消耗能量
                        }
                        vector<int> diff1 = v_diff(pre, D);
                        vector<int> diff2 = v_diff(D, pre);
                        diffSum += diff1.size() + diff2.size();
                        pre = D;
                        livetime++;
                        //cout << diffSum / livetime << endl;
                    }
                    result[alg][i] = diffSum / livetime;
                }
            }
            string savepath = resultpath + "R" + to_string(R) + "mod" + to_string(mod) + "N" + to_string(N) + ".txt";
            ofstream f(savepath, ios::out);
            if(!f) cout << "write error" << endl;
            for(int i = 0; i < t; i++)
            {
                f << result[0][i] << " " << result[1][i] << endl; //输出结果，一个mode下的一个N占具一个txt
            }
            f.close();
        }
    }
}

int main()
{
    for(int R = 250; R <= 250; R += 5)
    {
        cout << "R" << R << "start" << endl;

        // 确保输出目录存在
        for(int k = 1; k <= 7; ++k)
            filesystem::create_directories("./output/ex" + to_string(k) + "/");

        // 1) 寿命：均匀能量 100
        ex1(10, 100, 100, R);

        // 2) 寿命：初始能量随机 [50,100]
        ex2(10, 100, 100, R);

        // 3) 平均 CDS 大小（均匀能量）
        ex3(10, 100, 100, R);

        // 4) 平均运行时间/帧（粗粒度计时）
        ex4(10, 100, 100, R);

        // 5) 能量均衡性：逐帧标准差序列
        ex5(10, 100, 100, R);

        // 6) 平均 CDS 大小（初始能量随机）
        ex6(10, 100, 100, R);

        // 7) 邻帧 CDS 成员变动（只跑 Mod-R）
        ex7(10, 60, 50, R);
    }
    cout << "MISSION COMPLETED" << endl;
    return 0;
}