#include <bits/stdc++.h>
#include <omp.h>
using namespace std;

/*** Lightweight Graph Structure ***/
struct AdjGraph {
    int N = 0;
    vector<vector<int>> adj;
    void reset(int n){
        N = n;
        adj.assign(N, {});
    }
};

/*** Utility: Write/Read Simple Edge List Format ***/
// File format:
// Line 1: N M
// Next M lines: u v   (0-based, undirected)
static bool save_edges_txt(const string& path, int N, const vector<pair<int,int>>& edges){
    filesystem::create_directories(filesystem::path(path).parent_path());
    ofstream out(path);
    if(!out) return false;
    out << N << " " << edges.size() << "\n";
    for (auto &e: edges) out << e.first << " " << e.second << "\n";
    return true;
}

static bool load_edges_txt(AdjGraph& G, const string& path){
    if(!filesystem::exists(path)) return false;
    ifstream in(path);
    if(!in) return false;
    int N, M;
    if(!(in >> N >> M)) return false;            // Not our simple format: parse failed
    if(N <= 0 || M < 0) return false;
    G.reset(N);
    for(int i=0;i<M;i++){
        int u,v;
        if(!(in >> u >> v)) return false;
        if(u<0 || u>=N || v<0 || v>=N) return false;
        G.adj[u].push_back(v);
        G.adj[v].push_back(u);
    }
    return true;
}

/*** Generate Unit Disk Graph (Connected) and Save as Simple Edge List Format ***/
static bool gen_unit_disk_connected(AdjGraph& G, int N, int r, int range, uint32_t seed){
    mt19937 rng(seed);
    uniform_real_distribution<double> U(0.0, (double)range);

    for(int attempt=0; attempt<50; ++attempt){
        vector<pair<double,double>> pos(N);
        for(int i=0;i<N;i++) pos[i] = {U(rng), U(rng)};
        vector<pair<int,int>> edges;
        edges.reserve(N*10);

        const double rr = (double)r;
        for(int i=0;i<N;i++){
            for(int j=i+1;j<N;j++){
                double dx = pos[i].first  - pos[j].first;
                double dy = pos[i].second - pos[j].second;
                if (dx*dx + dy*dy <= rr*rr) edges.emplace_back(i,j);
            }
        }
        // Check connectivity using temporary adjacency list
        vector<vector<int>> adj(N);
        for(auto &e: edges){ adj[e.first].push_back(e.second); adj[e.second].push_back(e.first); }
        vector<char> vis(N,0);
        queue<int> q; q.push(0); vis[0]=1; int seen=1;
        while(!q.empty()){
            int u=q.front(); q.pop();
            for(int v: adj[u]) if(!vis[v]){ vis[v]=1; ++seen; q.push(v); }
        }
        if(seen!=N) continue;

        // Success
        G.reset(N);
        G.adj = move(adj);
        return true;
    }
    return false;
}

/*** Try to Load; if Failed, Generate and Overwrite Save (Using Simple Edge List Format) ***/
static bool load_or_build_graph(AdjGraph& G, int N, int r, int idx, int range=500){
    // Use absolute path to avoid working directory issues
    filesystem::path file_path(__FILE__);
    filesystem::path script_dir = filesystem::absolute(file_path).parent_path();
    filesystem::path path = script_dir / "output" / "graphs" / ("R=" + to_string(r))
                                      / ("N=" + to_string(N))
                                      / ("net" + to_string(idx) + ".txt");

    // Try to read our simple format
    if(load_edges_txt(G, path)) return true;

    // Not found or not simple format -> generate and overwrite as simple format
    cout << "[GenGraph] Generating and saving: " << path << "\n" << flush;
    mt19937 seeder((uint32_t)(N*131u + r*17u + idx*10007u));
    for(int tries=0; tries<50; ++tries){
        AdjGraph tmp;
        if(gen_unit_disk_connected(tmp, N, r, range, seeder())){
            // Derive edge list from tmp and save
            vector<pair<int,int>> edges;
            for(int u=0; u<tmp.N; ++u){
                for(int v: tmp.adj[u]) if(u<v) edges.emplace_back(u,v);
            }
            if(save_edges_txt(path, tmp.N, edges)){
                cout << "[GenGraph] Saved successfully\n" << flush;
                G = move(tmp);
                return true;
            }
        }
    }
    cout << "[GenGraph] Failed after 50 attempts\n" << flush;
    return false;
}

/*** Component Labeling: Edge Only on D ***/
static int label_components_on_D(const AdjGraph& G,
                                 const vector<int>& D,
                                 vector<int>& comp,
                                 vector<char>& isInD)
{
    const int N = G.N;
    fill(isInD.begin(), isInD.end(), 0);
    for(int u: D) isInD[u]=1;
    fill(comp.begin(), comp.end(), 0);
    int cid=0;
    queue<int> q;
    for(int u: D){
        if(comp[u]) continue;
        ++cid;
        comp[u]=cid;
        q.push(u);
        while(!q.empty()){
            int x=q.front(); q.pop();
            for(int y: G.adj[x]){
                if(isInD[y] && !comp[y]){
                    comp[y]=cid;
                    q.push(y);
                }
            }
        }
    }
    return cid;
}

/*** Full Graph BFS Shortest Path ***/
static bool shortest_path(const AdjGraph& G, int s, int t, vector<int>& parent, vector<int>& path){
    const int N = G.N;
    fill(parent.begin(), parent.end(), -1);
    queue<int> q; q.push(s); parent[s]=-2;
    bool found=false;
    while(!q.empty()){
        int u=q.front(); q.pop();
        for(int v: G.adj[u]){
            if(parent[v]==-1){
                parent[v]=u;
                if(v==t){ found=true; break; }
                q.push(v);
            }
        }
        if(found) break;
    }
    path.clear();
    if(!found) return false;
    for(int cur=t; cur!=-2; cur=parent[cur]) path.push_back(cur);
    reverse(path.begin(), path.end());
    return true;
}

/*** GMERS on AdjGraph: α-Parameterized, Domination + Connectivity ***/
static vector<int> gmers_adj(const AdjGraph& G,
                             const vector<double>& energy,
                             double alpha,
                             mt19937& rng)
{
    const int N = G.N;
    vector<uint8_t> covered(N, 0);
    vector<int> D; D.reserve(N/2+8);

    auto all_covered = [&](){
        for(int i=0;i<N;i++) if(!covered[i]) return false;
        return true;
    };

    // Reusable scratch
    vector<int> mark(N, 0);
    int stamp = 1;

    uniform_real_distribution<double> U01(0.0, 1.0);

    // ===== 1) Domination Phase =====
    while(!all_covered()){
        int best = -1;
        double bestScore = -1.0;
        double bestTie = -1.0;

        for(int u=0; u<N; ++u){
            // Count uncovered nodes in "dominating closure" (self + neighbors) without duplication using stamp
            int cov = 0; double sumE = 0.0;

            auto consider = [&](int v){
                if(mark[v]==stamp) return;
                mark[v] = stamp;
                if(!covered[v]){
                    ++cov;
                    sumE += max(energy[v], 1e-12);
                }
            };

            consider(u);
            for(int v: G.adj[u]) consider(v);

            if(cov==0){ ++stamp; if(stamp==INT_MAX){ fill(mark.begin(), mark.end(), 0); stamp=1; } continue; }

            double e_u = max(energy[u], 1e-12);
            double e_norm = e_u / sumE;          // Normalized energy (relative to uncovered neighborhood)
            double cov_term = (double)cov;       // Coverage quantity
            double score = pow(e_norm, alpha) * pow(cov_term, 1.0 - alpha);
            double tie   = U01(rng);

            if(score > bestScore || (score==bestScore && tie>bestTie)){
                bestScore = score; bestTie = tie; best = u;
            }
            // Prepare for next round statistics
            ++stamp; if(stamp==INT_MAX){ fill(mark.begin(), mark.end(), 0); stamp=1; }
        }

        if(best<0){
            // Fallback: find an uncovered node
            for(int i=0;i<N;i++) if(!covered[i]){ best=i; break; }
            if(best<0) break; // Impossible, but defensive
        }
        D.push_back(best);
        covered[best]=1;
        for(int v: G.adj[best]) covered[v]=1;
    }

    if(D.empty()) return D;

    // ===== 2) Connectivity Phase: Add Points =====
    vector<int> comp(N,0);
    vector<char> isInD(N,0);
    vector<int> parent(N,-1), path;

    while(true){
        int cc = label_components_on_D(G, D, comp, isInD);
        if(cc<=1) break;
        // Pick representatives from two different components (any will do)
        int a=-1, b=-1, ca=-1;
        for(int u: D){ if(a==-1){ a=u; ca=comp[u]; break; } }
        for(int u: D){ if(comp[u]!=ca){ b=u; break; } }
        if(a==-1 || b==-1) break;

        if(!shortest_path(G, a, b, parent, path)) break; // Theoretically connected, so path should exist

        // Add intermediate points to D (endpoints a,b already in D)
        for(size_t k=1; k+1<path.size(); ++k) D.push_back(path[k]);
        sort(D.begin(), D.end());
        D.erase(unique(D.begin(), D.end()), D.end());
    }

    return D;
}

/*** Energy Cost Sampling (Normalized Parameters with E[c]=1) ***/
struct EnergyParams {
    double lognorm_sigma = 0.5;
    double weibull_k     = 1.5;
    double gamma_k       = 2.0;
};

static double sample_cost(const string& model,
                          mt19937& rng,
                          const EnergyParams& P = {})
{
    if(model=="Exp"){
        exponential_distribution<double> D(1.0);
        return D(rng);
    }else if(model=="Gamma"){
        double k=P.gamma_k; gamma_distribution<double> D(k, 1.0/k);
        return D(rng);
    }else if(model=="Lognormal"){
        double s=P.lognorm_sigma; double mu=-0.5*s*s;
        lognormal_distribution<double> D(mu, s);
        return D(rng);
    }else if(model=="Weibull"){
        double k=P.weibull_k; double lambda = 1.0 / tgamma(1.0 + 1.0/k);
        weibull_distribution<double> D(k, lambda);
        return D(rng);
    }
    return 1.0;
}

/*** Ex-Alpha-Sweep ***/
static void ex_alpha_sweep(){
    cout << "\n========== Ex-Alpha-Sweep ==========\n";
    filesystem::create_directories("./output/ex_alpha_sweep/");

    const vector<int> N_values = {50, 100, 150};
    const vector<int> r_values = {75, 100, 125};
    const int T = 100;

    vector<double> alphas(13);
    for(int i=0;i<13;i++) alphas[i] = 0.1*i;

    for(int N : N_values){
        for(int r : r_values){
            cout << "[Progress] N="<<N<<" r="<<r<<"\n" << flush;
            vector<double> sumL(13,0.0), sumC(13,0.0);
            vector<int> cnt(13,0);

            #pragma omp parallel for collapse(2) schedule(dynamic)
            for(int i=0;i<T;i++){
                for(int a=0;a<13;a++){
                    AdjGraph G;
                    if(!load_or_build_graph(G, N, r, i)) continue;

                    double alpha = alphas[a];
                    mt19937 rng((uint32_t)(N*100000 + r*1000 + i*13 + a));

                    vector<double> energy(G.N, 100.0);
                    long long cds_sum=0; int frames=0;

                    while(true){
                        bool dead=false;
                        for(double e: energy){ if(e<=0){ dead=true; break; } }
                        if(dead) break;

                        auto cds = gmers_adj(G, energy, alpha, rng);
                        if(cds.empty()) break;

                        cds_sum += (int)cds.size();
                        for(int u: cds) energy[u] -= 1.0;
                        ++frames;
                    }

                    #pragma omp critical
                    {
                        sumL[a] += frames;
                        sumC[a] += (frames? (double)cds_sum/frames : 0.0);
                        cnt[a]  += 1;
                    }
                }
            }

            string fn = "./output/ex_alpha_sweep/alpha_sweep_N"+to_string(N)
                      + "_r"+to_string(r)+".txt";
            ofstream out(fn);
            if(!out){ cout<<"write error: "<<fn<<"\n"; continue; }
            out << "# alpha  avg_lifetime  avg_cds\n";
            out.setf(ios::fixed); out<<setprecision(2);
            for(int a=0;a<13;a++){
                double L = cnt[a]? sumL[a]/cnt[a] : 0.0;
                double C = cnt[a]? sumC[a]/cnt[a] : 0.0;
                out << alphas[a] << " " << L << " " << C << "\n";
            }
        }
    }
    cout << "✓ Ex-Alpha-Sweep done\n";
}

/*** Ex-Large ***/
static void ex_large(){
    cout << "\n========== Ex-Large ==========\n";
    filesystem::create_directories("./output/ex_large/");

    const vector<int> N_values = {50, 100, 150, 200, 250, 300};
    const vector<int> r_values = {75, 100, 125};
    const int T = 100;
    const double alpha = 0.7;

    for(int N: N_values){
        for(int r: r_values){
            cout << "[Progress] N="<<N<<" r="<<r<<"\n" << flush;

            vector<double> L(T,0.0), C(T,0.0), build_ms_per_frame(T,0.0);
            #pragma omp parallel for schedule(dynamic)
            for(int i=0;i<T;i++){
                AdjGraph G;
                if(!load_or_build_graph(G, N, r, i)) continue;

                vector<double> energy(G.N, 100.0);
                long long cds_sum=0; int frames=0;
                chrono::duration<double, milli> build_ms(0);
                mt19937 rng((uint32_t)(N*100000 + r*1000 + i));

                while(true){
                    bool dead=false;
                    for(double e: energy){ if(e<=0){ dead=true; break; } }
                    if(dead) break;

                    auto t0=chrono::high_resolution_clock::now();
                    auto cds = gmers_adj(G, energy, alpha, rng);
                    auto t1=chrono::high_resolution_clock::now();
                    if(cds.empty()) break;

                    build_ms += (t1 - t0);
                    cds_sum += (int)cds.size();
                    for(int u: cds) energy[u] -= 1.0;
                    ++frames;
                }
                L[i]=frames;
                C[i]=frames? (double)cds_sum/frames : 0.0;
                build_ms_per_frame[i]= frames? build_ms.count()/frames : 0.0;
            }

            string fn = "./output/ex_large/large_N"+to_string(N)+"_r"+to_string(r)+".txt";
            ofstream out(fn);
            if(!out){ cout<<"write error: "<<fn<<"\n"; continue; }
            out << "# lifetime  cds_avg  build_ms_per_frame\n";
            out.setf(ios::fixed); out<<setprecision(2);
            for(int i=0;i<T;i++)
                out << L[i] << " " << C[i] << " " << build_ms_per_frame[i] << "\n";
        }
    }
    cout << "✓ Ex-Large done\n";
}

/*** Ex-CDS-Stability ***/
static void ex_cds_stability(){
    cout << "\n========== Ex-CDS-Stability ==========\n";
    filesystem::create_directories("./output/ex_cds_stability/");

    const vector<int> N_values={50, 100, 150};
    const vector<int> r_values={100, 125};
    const vector<double> alphas={0.3, 0.5, 0.7};
    const int T=30;

    for(int N: N_values){
        for(int r: r_values){
            cout << "[Progress] N="<<N<<" r="<<r<<"\n" << flush;
            for(double alpha : alphas){
                vector<double> results_J(T), results_Tau(T);
                vector<int> results_measures(T);

                #pragma omp parallel for schedule(dynamic)
                for(int i=0;i<T;i++){
                    AdjGraph G;
                    if(!load_or_build_graph(G, N, r, i)) continue;

                    mt19937 rng((uint32_t)(N*100000 + r*1000 + i));
                    vector<double> energy(G.N, 100.0);

                    vector<int> prev; bool has_prev=false;
                    double accJ=0.0, accTau=0.0; int measures=0;

                    while(true){
                        bool dead=false;
                        for(double e: energy){ if(e<=0){ dead=true; break; } }
                        if(dead) break;

                        auto curr = gmers_adj(G, energy, alpha, rng);
                        if(curr.empty()) break;

                        if(has_prev){
                            unordered_set<int> A(prev.begin(), prev.end());
                            unordered_set<int> B(curr.begin(), curr.end());
                            int inter=0;
                            for(int u: A) if(B.count(u)) ++inter;
                            int uni = (int)A.size() + (int)B.size() - inter;
                            double J   = uni? (double)inter/uni : 0.0;
                            int sym    = (int)A.size() + (int)B.size() - 2*inter;
                            double Tau = uni? (double)sym/uni : 0.0; // 1 - J (symmetric difference ratio)

                            accJ += J; accTau += Tau; ++measures;
                        }
                        prev.swap(curr);
                        has_prev=true;
                        for(int u: prev) energy[u]-=1.0;
                    }
                    if(measures>0){
                        results_J[i] = accJ/measures;
                        results_Tau[i] = accTau/measures;
                        results_measures[i] = 1;
                    }
                }

                double sumJ=0.0, sumTau=0.0; int cnt=0;
                for(int i=0;i<T;i++){
                    if(results_measures[i]>0){
                        sumJ += results_J[i];
                        sumTau += results_Tau[i];
                        ++cnt;
                    }
                }

                double avgJ = cnt? sumJ/cnt : 0.0;
                double avgT = cnt? sumTau/cnt: 0.0;

                string fn = "./output/ex_cds_stability/stability_N"+to_string(N)
                          + "_r"+to_string(r)
                          + "_alpha"+to_string((int)round(alpha*10)) + ".txt";
                ofstream out(fn);
                if(!out){ cout<<"write error: "<<fn<<"\n"; continue; }
                out.setf(ios::fixed); out<<setprecision(4);
                out << avgJ << " " << avgT << "\n";
            }
        }
    }
    cout << "✓ Ex-CDS-Stability done\n";
}

/*** Ex-Energy-Model ***/
static void ex_energy_model(){
    cout << "\n========== Ex-Energy-Model ==========\n";
    filesystem::create_directories("./output/ex_energy_model/");

    const int N=100, r=100, T=30;
    const double alpha=0.7;
    const vector<string> models={"Exp","Gamma","Lognormal","Weibull"};
    EnergyParams P;

    for(const string& model : models){
        cout << "[Progress] model="<<model<<"\n" << flush;
        vector<double> L(T,0.0), C(T,0.0), Var(T,0.0);

        #pragma omp parallel for schedule(dynamic)
        for(int i=0;i<T;i++){
            AdjGraph G;
            if(!load_or_build_graph(G, N, r, i)) continue;

            mt19937 rng((uint32_t)(i*1315423911u));
            vector<double> energy(G.N, 100.0);

            long long cds_sum=0; int frames=0;
            vector<double> frame_costs; frame_costs.reserve(256);

            while(true){
                bool dead=false;
                for(double e: energy){ if(e<=0){ dead=true; break; } }
                if(dead) break;

                auto cds = gmers_adj(G, energy, alpha, rng);
                if(cds.empty()) break;

                double frame=0.0;
                for(int u: cds){ double c=sample_cost(model, rng, P); energy[u]-=c; frame+=c; }
                cds_sum += (int)cds.size();
                frame_costs.push_back(frame);
                ++frames;
            }
            L[i]=frames;
            C[i]=frames? (double)cds_sum/frames : 0.0;

            if(!frame_costs.empty()){
                double avg=accumulate(frame_costs.begin(), frame_costs.end(), 0.0)/frame_costs.size();
                double var=0.0;
                for(double x: frame_costs){ double d=x-avg; var += d*d; }
                var /= frame_costs.size();
                Var[i]=var;
            }else Var[i]=0.0;
        }

        auto mean = [&](const vector<double>& v){
            double s=0.0; for(double x: v) s+=x; return v.empty()? 0.0 : s/v.size();
        };
        string fn="./output/ex_energy_model/energy_model_"+model+".txt";
        ofstream out(fn);
        if(!out){ cout<<"write error: "<<fn<<"\n"; continue; }
        out.setf(ios::fixed); out<<setprecision(2);
        out << mean(L) << " " << mean(C) << " " << mean(Var) << "\n";
    }
    cout << "✓ Ex-Energy-Model done\n";
}

/*** main ***/
int main(int argc, char** argv){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string mode = (argc>1)? string(argv[1]) : "help";
    if(mode=="alpha_sweep")          ex_alpha_sweep();
    else if(mode=="large")           ex_large();
    else if(mode=="stability")       ex_cds_stability();
    else if(mode=="energy_model")    ex_energy_model();
    else if(mode=="all"){ ex_alpha_sweep(); ex_large(); ex_cds_stability(); ex_energy_model(); }
    else {
        cout << "Supplement Experiments\n\n"
             << "Usage: simulations_supplement [mode]\n\n"
             << "Modes:\n"
             << "  alpha_sweep   - Alpha-Sweep experiment\n"
             << "  large         - Scalability (N up to 300)\n"
             << "  stability     - CDS stability metrics\n"
             << "  energy_model  - Energy distribution models (E[c]=1)\n"
             << "  all           - Run all\n";
    }
    return 0;
}
