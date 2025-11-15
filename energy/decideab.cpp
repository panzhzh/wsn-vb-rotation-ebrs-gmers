#include"graph.hpp"
#include<iostream>
#include<time.h>
#include<random>
#include<filesystem>



vector<int> minw(Graph G, int *energy, double a)
{
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
            isum += weight[j];
            v2++;
        }
        v1 = weight[i]/isum;
        double value = pow(v1, a) * pow(v2, 1-a);
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
                double value = pow(v1, a) * pow(v2, 1-a);
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

int consume(int type)
{
    if(type == 1) return 1;
    else
    {
        int r = rand()%100;
        if(r < 50) return 1;
        else if(r < 75) return 2;
        else if(r < 90) return 3;
        else return 4;
    }
}


int sim(Graph G, double a)
{
    int N = G.number_of_nodes;
    int energy[N];
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
        D = minw(G, energy, a);
        for(int j:D)
        {
            energy[j] -= consume(1);
        }
        t++;
        size += D.size();
    }
    sum += t;
    size /= sum;

    return sum;

}



int main()
{
    // 图的类型：0=smaller, 1=equal, 2=greater
    string degree_types[3] = {"smaller", "equal", "greater"};

    cout << "====================================================" << endl;
    cout << "Alpha 扫描实验 - 生成 Figure 3 数据" << endl;
    cout << "====================================================" << endl;

    // N=50 和 N=100
    for(int N = 50; N <= 100; N+=50)
    {
        cout << "\n处理 N=" << N << "..." << endl;

        // 遍历三种度数分档
        for(int k = 0; k < 3; k++)
        {
            cout << "  处理 " << degree_types[k] << " 度数..." << endl;

            string graphpath = "./output/graphs/" + degree_types[k] + "/N=" + to_string(N) + "/";

            int t = 100;
            double result[11] = {0};

            // 遍历 100 个图样本
            for(int i = 0; i < t; i++)
            {
                string readpath = graphpath + "net" + to_string(i) + ".txt";

                // 检查文件是否存在
                ifstream check_file(readpath);
                if(!check_file.good())
                {
                    cerr << "  ⚠ 文件不存在: " << readpath << endl;
                    continue;
                }

                Graph G;
                G.read(readpath);

                // α = 0 (接近 0)
                result[0] += sim(G, 0.00001);

                // α = 0.1 ~ 1.0 (10个点)
                int count = 1;
                for(double a = 0.1; a <= 1.0; a += 0.1)
                {
                    int r = sim(G, a);
                    result[count++] += r;
                }

                cout << "    ✓ N=" << N << " " << degree_types[k] << " net" << i << endl;
            }

            // 计算平均值并保存结果
            string output_dir = "./output/alpha_scan/" + degree_types[k] + "/";
            filesystem::create_directories(output_dir);

            string result_path = output_dir + "N=" + to_string(N) + "_result.txt";
            ofstream ofile(result_path, ios::out);

            if(!ofile)
            {
                cerr << "  ✗ 写入错误: " << result_path << endl;
                continue;
            }

            ofile << "# Alpha 扫描结果 (N=" << N << ", " << degree_types[k] << " 度数)" << endl;
            ofile << "# alpha_value  average_lifetime" << endl;

            // α = 0
            ofile << "0.0 " << result[0]/t << endl;

            // α = 0.1 ~ 1.0
            int count = 1;
            for(double a = 0.1; a <= 1.0; a += 0.1)
            {
                ofile << a << " " << result[count++]/t << endl;
            }

            ofile.close();
            cout << "  ✓ 结果保存: " << result_path << endl;
        }

        cout << "N=" << N << ": 完成！" << endl;
    }

    cout << "\n====================================================" << endl;
    cout << "✓ Alpha 扫描完成！" << endl;
    cout << "====================================================" << endl;
    cout << "\n结果保存位置: ./output/alpha_scan/" << endl;

    return 0;

}