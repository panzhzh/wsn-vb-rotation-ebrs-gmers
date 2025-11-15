# Energy Optimization Experiments - Complete Guide

## 📋 Directory Structure

```
energy/
├── README.md                          # This document
│
├── Source Code Files
│   ├── graphgene.cpp                  # Graph generation program
│   ├── simulations.cpp                # Main paper experiments (ex1-ex7)
│   ├── simulations_supplement.cpp     # Supplementary experiments (4 experiments)
│   ├── decideab.cpp                   # Ex0-Alpha experiment (alpha scanning)
│   └── graph.hpp                      # Graph data structure definition
│
├── Execution Scripts
│   ├── run_experiments.sh             # ★ Main script: execute all experiments (ex0-ex7)
│   ├── run_supplement.sh              # Supplementary experiments dedicated script
│   └── plot*.py                       # Plotting scripts
│
└── Output Directories (generated after running scripts)
    ├── output/
    │   ├── graphs/                    # Generated graph data
    │   ├── ex0_alpha/                 # Ex0-Alpha results (alpha scanning)
    │   ├── alpha_scan/                # Ex0-Alpha results (original directory name)
    │   ├── ex1/ ~ ex7/                # Main experiment results
    │   └── exX/ (supplementary)       # Supplementary experiment results
    ├── figures/                       # Generated plots
    ├── supplement/                    # Supplementary experiment plots
    └── tables/                        # CSV data files
```

---

## 🔧 Source Code Description

### 1. **graphgene.cpp** - Graph Generation Program
**Function**: Generate unit disk random geometric graphs and graphs with specific degree distributions

**Compilation**:
```bash
g++ -o graphgene.exe graphgene.cpp -std=c++17
```

**Execution**:
```bash
./graphgene.exe
# Select mode:
#   1: Unit disk random graph (R=250, N=10-100, 100 samples/N)
#      Output: ./output/graphs/R=250/N=*/net{0..99}.txt
#   2: Specific degree graph (smaller/equal/greater)
#      Output: ./output/graphs/{smaller|equal|greater}/N={50,100}/net{0..99}.txt
```

---

### 2. **simulations.cpp** - Main Paper Experiments (Ex1-Ex7)
**Function**: Implement 7 main experiments to test GMERS algorithm performance

**Compilation**:
```bash
g++ -o simulations.exe simulations.cpp -std=c++17 -fopenmp
```

**Execution**:
```bash
export OMP_NUM_THREADS=<num>  # Recommend using all CPU cores
./simulations.exe
# Output: ./output/ex{1..7}/
```

**Thread Number Parameters**:
- Recommended: `export OMP_NUM_THREADS=$(nproc)` (use all cores)
- Default: System auto-allocation
- Minimum: 1 (single-threaded)
- Maximum: Number of CPU cores

---

### 3. **simulations_supplement.cpp** - Supplementary Experiments (4)
**Function**: Implement 4 supplementary experiments

**Compilation**:
```bash
g++ -o simulations_supplement.exe simulations_supplement.cpp -std=c++17 -fopenmp
```

**Execution**:
```bash
export OMP_NUM_THREADS=16
./simulations_supplement.exe [experiment_name]

# Optional parameters:
#   alpha_sweep       Ex-Alpha-Sweep (α parameter scanning)
#   large            Ex-Large (large-scale network)
#   stability        Ex-CDS-Stability (stability)
#   energy_model     Ex-Energy-Model (energy model)
#   all              Run all supplementary experiments
```

**Output**:
- `./output/ex_alpha_sweep/`
- `./output/ex_large/`
- `./output/ex_cds_stability/`
- `./output/ex_energy_model/`

---

### 4. **decideab.cpp** - Ex0-Alpha (Alpha Scanning)
**Function**: Alpha parameter scanning experiment to test the impact of α=0.0-1.0 on network lifetime

**Compilation**:
```bash
g++ -o decideab.exe decideab.cpp -std=c++17
```

**Execution**:
```bash
./decideab.exe
# Input data: ./output/graphs/{smaller|equal|greater}/N={50,100}/net*.txt
# Output data: ./output/alpha_scan/{smaller|equal|greater}/N=*.txt
```

**Parameters**:
- **α values**: 0.0, 0.1, 0.2, ..., 1.0 (11 points)
- **Degree types**: smaller, equal, greater
- **Node counts**: N=50, 100
- **Sample size**: 100 (per configuration)

---

### 5. **graph.hpp** - Graph Data Structure
**Function**: Define `Graph` class for graph storage and manipulation

**Includes**:
- `number_of_nodes`: Number of nodes
- `outneighbors()`: Get neighbor list
- `read()`: Read graph from file
- `write()`: Write graph to file

---

## 🚀 Execution Scripts Description

### ★ Main Script: `run_experiments.sh`

**Function**: One-click execution of complete ex0-ex7 experiment workflow

**Usage**:
```bash
cd energy/
bash run_experiments.sh
```

**Execution Flow**:

| STEP | Operation | Command | Output Location |
|------|-----------|---------|-----------------|
| 1 | Check/Compile | Auto compile graphgene, simulations, decideab | - |
| 2 | Create directories | mkdir output/graphs etc. | - |
| 3 | Generate graphs Mode 1 | `echo 1 \| graphgene.exe` | ./output/graphs/R=250/ |
| 4 | Generate graphs Mode 2 | `echo 2 \| graphgene.exe` | ./output/graphs/{smaller\|equal\|greater}/ |
| 5 | Ex0-Alpha | `./decideab.exe` | ./output/alpha_scan/ |
| 6 | Ex1-Ex7 | `./simulations.exe` | ./output/ex{1..7}/ |
| 7 | Verification | Count output files | - |

**Expected Runtime**:
- STEP 1-2: < 1 minute
- STEP 3: 10-15 minutes (graph generation)
- STEP 4: 5-10 minutes (graph generation)
- STEP 5: 10-15 minutes (alpha scanning)
- STEP 6: 30 minutes - 2 hours (main experiments, depends on CPU)

**Total Runtime**: Approximately 2-3 hours (using multi-core CPU)

---

### Supplementary Script: `run_supplement.sh`

**Function**: Execute 4 supplementary experiments

**Usage**:
```bash
cd energy/
bash run_supplement.sh
```

**Or run a specific supplementary experiment individually**:
```bash
export OMP_NUM_THREADS=16
./simulations_supplement.exe alpha_sweep
./simulations_supplement.exe large
./simulations_supplement.exe stability
./simulations_supplement.exe energy_model
```

---

## 📊 Parameter Details

### OpenMP Thread Count Setting

**Main Experiments Recommendation**:
```bash
export OMP_NUM_THREADS=$(nproc)  # Use all CPU cores
./simulations.exe
```

**Supplementary Experiments Recommendation**:
```bash
export OMP_NUM_THREADS=16
./simulations_supplement.exe all
```

**Ex0-Alpha**:
```bash
./decideab.exe  # Single-threaded (original code does not support parallelism)
```

**Check CPU Core Count**:
```bash
nproc                    # View physical core count
lscpu | grep "CPU(s)"    # Detailed information
```

### Experiment Parameters

#### Ex0-Alpha (Alpha Scanning)
- **α range**: 0.0 → 1.0 (11 points, step 0.1)
- **Degree types**: 3 types (smaller/equal/greater)
- **Node counts**: 2 types (N=50, 100)
- **Sample size**: 100 (per configuration)
- **Total processing**: 6×11 = 66 experiment groups

#### Ex1-Ex7 (Main Experiments)
- **Topology**: Unit disk random graph (R=250)
- **Node counts**: N=10, 20, 30, ..., 100 (10 types)
- **Sample size**: 100 (per N value)
- **Total processing**: 1000 graphs

#### Supplementary Experiments
- **Alpha Sweep**: α=0.1-0.9 (9 points)
- **Large**: N=100-1000 (scalability testing)
- **Stability**: CDS member change analysis
- **Energy Model**: 4 energy distributions (Exp, Gamma, Lognormal, Weibull)

---

## 📈 Output Data Format

### Experiment Results (.txt)

**Ex0-Alpha Result Format**:
```
# Alpha scanning results (N=50, smaller degree)
# alpha_value  average_lifetime
0.0 XXXXX
0.1 XXXXX
...
1.0 XXXXX
```

**Main Experiment Result Format**:
```
N=10
...
<metric1> <metric2> <metric3> ...
```

### CSV Data (tables/)

**Supplementary Experiments Exported CSV**:
- `alpha_sweep_lifetime.csv` - Alpha scanning network lifetime
- `alpha_sweep_cds_size.csv` - Alpha scanning CDS size
- `large_scalability.csv` - Large-scale scalability
- `stability_metrics.csv` - Stability metrics
- `energy_model_comparison.csv` - Energy model comparison
- `README.md` - CSV field description

---

## 🎯 Complete Usage Examples

### Scenario 1: Quick Test (Complete Workflow)
```bash
cd energy/
bash run_experiments.sh
```
Execution time: ~2-3 hours

### Scenario 2: Run Only Main Experiments
```bash
cd energy/

# 1. Generate graphs needed for main experiments (Mode 1)
echo "1" | ./graphgene.exe

# 2. Run main experiments
export OMP_NUM_THREADS=$(nproc)
./simulations.exe

# 3. Generate plots
python3 plot.py
```
Execution time: ~1 hour

### Scenario 3: Run Only Ex0-Alpha
```bash
cd energy/

# 1. Generate graphs needed for Ex0-Alpha (Mode 2)
echo "2" | ./graphgene.exe

# 2. Run alpha scanning
./decideab.exe

# 3. View results
cat ./output/alpha_scan/smaller/N=50_result.txt
```
Execution time: ~20 minutes

### Scenario 4: Run Only Supplementary Experiments
```bash
cd energy/

# 1. Compile
g++ -o simulations_supplement.exe simulations_supplement.cpp -std=c++17 -fopenmp

# 2. Run all supplementary experiments
export OMP_NUM_THREADS=16
./simulations_supplement.exe all

# 3. Generate plots and CSV
python3 plot_all_supplement.py
```
Execution time: ~30 minutes

---

## ⚙️ Advanced Usage

### Modify Parameters

#### Modify Ex0-Alpha α value range
Edit `decideab.cpp` lines 189-191:
```cpp
for(double a = 0.1; a <= 1.0; a += 0.1)  // Modify step size
```

#### Modify Ex1-Ex7 network scale
Edit parameter declarations in `simulations.cpp`

#### Modify supplementary experiments thread count
```bash
export OMP_NUM_THREADS=32  # Change to 32 threads
./simulations_supplement.exe all
```

### Run Multiple Experiments in Parallel
```bash
# Run graphgene and simulations in parallel
echo "1" | ./graphgene.exe &
GRAPH_PID=$!
wait $GRAPH_PID

export OMP_NUM_THREADS=$(nproc)
./simulations.exe &
SIM_PID=$!

# Run supplementary experiments simultaneously
export OMP_NUM_THREADS=8
./simulations_supplement.exe all

wait $SIM_PID
```

---

## 🔍 Troubleshooting

### Problem 1: Compilation Error
**Symptom**: `error: ... -fopenmp: command not found`

**Solution**:
```bash
# Install OpenMP
sudo apt install libomp-dev  # Ubuntu/Debian
brew install libomp          # macOS
```

### Problem 2: Out of Memory
**Symptom**: `std::bad_alloc` or process killed

**Solution**:
- Reduce sample size (modify source code)
- Reduce thread count: `export OMP_NUM_THREADS=4`
- Use smaller N values for testing

### Problem 3: Graph File Not Found
**Symptom**: "File not found" error

**Solution**:
```bash
# Ensure graph generation is run first
echo "1" | ./graphgene.exe
echo "2" | ./graphgene.exe

# Check generated files
ls output/graphs/R=250/N=10/
ls output/graphs/smaller/N=50/
```

### Problem 4: Script Permission Denied
**Symptom**: `Permission denied: ./run_experiments.sh`

**Solution**:
```bash
chmod +x run_experiments.sh
chmod +x run_supplement.sh
bash run_experiments.sh  # Execute with bash
```

---

## 📝 Quick Reference

| Task | Command | Execution Time |
|------|---------|-----------------|
| Run all experiments (ex0-ex7) | `bash run_experiments.sh` | ~2-3 hours |
| Generate graphs Mode 1 only | `echo 1 \| ./graphgene.exe` | ~10 minutes |
| Generate graphs Mode 2 only | `echo 2 \| ./graphgene.exe` | ~5 minutes |
| Run Ex0-Alpha only | `./decideab.exe` | ~10 minutes |
| Run main experiments only | `export OMP_NUM_THREADS=$(nproc); ./simulations.exe` | ~1 hour |
| Run supplementary experiments only | `export OMP_NUM_THREADS=16; ./simulations_supplement.exe all` | ~30 minutes |
| Compile all | `g++ -o *.exe *.cpp -std=c++17 -fopenmp` | ~30 seconds |

---

## 📞 Contact and Feedback

For questions or suggestions, please refer to:
- Comments in each script file
- Detailed documentation in source code
- tables/README.md (CSV data description)

---

**Last Updated**: 2025-10-26
**Version**: 2.0
**Status**: ✅ Complete
