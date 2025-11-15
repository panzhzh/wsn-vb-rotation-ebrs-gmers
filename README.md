# GMERS: Energy-Efficient Connected Dominating Set Algorithm for Wireless Sensor Networks

## Project Overview

This project implements the GMERS (Greedy Minimum Energy Ratio Selection) algorithm for constructing Connected Dominating Sets (CDS) in wireless sensor networks. By finding an optimal balance between energy consumption and coverage rate, it significantly extends network lifetime.

### Key Innovations
- **Alpha Parameter Balancing Mechanism**: The α∈[0,1] parameter balances energy efficiency (energy weight) and coverage performance (neighborhood degree)
- **Greedy Selection Strategy**: Dynamically computes node energy ratios and prioritizes nodes with sufficient energy
- **Connectivity Guarantee**: Ensures the network remains connected throughout the CDS construction process

## Directory Structure

```
code_cecs/
├── energy/                           Core code directory
│   ├── *.cpp, *.hpp                  Source code (C++17)
│   │   ├── graph.hpp                 Graph data structure
│   │   ├── graphgene.cpp             Graph generation program
│   │   ├── simulations.cpp           Main experiments (Ex1-Ex7)
│   │   ├── simulations_supplement.cpp Supplementary experiments
│   │   ├── decideab.cpp              Ex0-Alpha experiment
│   │   └── vectoruse.hpp             Vector utility functions
│   │
│   ├── run_experiments.sh            Execute all experiments
│   ├── run_supplement.sh             Execute supplementary experiments
│   ├── plot_figure.py                Plotting script for main experiments
│   └── plot_all_supplement.py        Plotting script for supplementary experiments
│
├── docs/                             Documentation
├── tables/                           CSV data output
├── figures/                          Generated plots (output)
├── output/                           Experiment results (output)
└── README.md                         This file
```

## Experiment Description

### Ex0-Alpha: Alpha Parameter Scanning Experiment

**Objective**: Verify the impact of α parameter on network lifetime and find the optimal trade-off point

**Parameter Range**:
- α ∈ [0.1, 1.0], step 0.1
- Graph types: smaller (degree=N/4), equal (degree=N/2), greater (degree=3N/4)
- Number of nodes: N=50, 100
- 100 sample graphs per configuration

**Output**:
- Network lifetime curves as α varies
- Dominating set size trends as α varies
- Detailed data in CSV format

### Ex1-Ex7: Main Paper Experiments

**Graph Generation**: Unit disk random geometric graphs (R=250, N=10-100, 100 samples per N)

Each experiment compares GMERS with baseline algorithms under different conditions:
- **Ex1**: Uniform initial energy (100 units)
- **Ex2**: Random initial energy [50, 100]
- **Ex3**: Average CDS size (uniform energy)
- **Ex4**: Average runtime per frame
- **Ex5**: Energy balance analysis
- **Ex6**: Average CDS size (random energy)
- **Ex7**: CDS member change frequency

### Supplementary Experiments

Four additional experiments for robustness and scalability validation:
- **Ex-Alpha-Sweep**: Extended α range [0, 1.2] scanning
- **Ex-Large**: Scalability test up to 300 nodes
- **Ex-CDS-Stability**: Jaccard similarity and member change rate
- **Ex-Energy-Model**: Robustness across energy distributions

## Quick Start

### Run All Experiments

```bash
cd energy/
bash run_experiments.sh
```

This script automatically executes:
1. **Compilation Check**: Ensure all executable files are compiled
2. **Directory Creation**: Build output directory structure
3. **Graph Generation**: Mode 1 (for main experiments) and Mode 2 (for Ex0-Alpha)
4. **Ex0-Alpha Execution**: α parameter scanning experiment
5. **Main Experiments**: All Ex1-Ex7 experiments
6. **Results Verification**: Check output file completeness

Estimated Time: **2-3 hours** (using multi-core CPU)

### Run Supplementary Experiments

```bash
cd energy/
bash run_supplement.sh
```

Run a specific supplementary experiment:
```bash
export OMP_NUM_THREADS=16
./simulations_supplement.exe alpha_sweep    # or large, stability, energy_model
```

## Output Description

**figures/**: Generated plots (ex1-ex7 and alpha scanning results)

**tables/**: CSV data files from supplementary experiments

**output/**: Raw experiment data (graphs, result files)

## Parameter Configuration

### OpenMP Thread Count Setting

```bash
# Main experiments: Use all CPU cores
export OMP_NUM_THREADS=$(nproc)
./simulations.exe

# Ex0-Alpha: Recommend 16 threads
export OMP_NUM_THREADS=16
./decideab.exe

# Supplementary experiments: Recommend 16 threads
export OMP_NUM_THREADS=16
./simulations_supplement.exe all
```

### Experiment Parameter Modification

Modify in source code:

**graphgene.cpp**:
- `degree = N/4, N/2, N*3/4` modify degree ratio
- `r = 250` modify communication radius

**decideab.cpp**:
- `for(double a = 0.1; a <= 1.0; a += 0.1)` modify α range and step size
- `N=50, 100` modify test node counts

**simulations_supplement.cpp**:
- Modify parameter ranges for each supplementary experiment

## System Requirements

### Compilation Environment
- **C++ Standard**: C++17 or higher
- **Compiler**: g++ 7.0+ or clang++ 5.0+
- **OpenMP**: Parallel computing support

### System Environment
- **Memory**: At least 2GB
- **Disk**: At least 10GB (for output results)
- **CPU**: Multi-core processor (4 cores or more recommended)

### Dependencies
- Standard C++ library (built-in)
- OpenMP (built-in with most compilers)

## Compilation Commands

```bash
cd energy/

# Individual compilation
g++ -o graphgene.exe graphgene.cpp -std=c++17
g++ -o simulations.exe simulations.cpp -std=c++17 -fopenmp
g++ -o decideab.exe decideab.cpp -std=c++17
g++ -o simulations_supplement.exe simulations_supplement.cpp -std=c++17 -fopenmp

# Or automatic compilation via script
bash run_experiments.sh
```

## FAQ

**Q: Why are the Ex0-Alpha results not exactly consistent with the paper figures?**
A: This is usually due to minor differences in random number generation or graph topology. Ensure you use the original graph data files.

**Q: The program runs slowly. How to speed it up?**
A: Increase OMP_NUM_THREADS value (not exceeding CPU core count) or reduce sample size.

**Q: How to run only a specific experiment?**
A: Run the corresponding executable directly, e.g., `./decideab.exe` or `./simulations_supplement.exe alpha_sweep`

## Detailed Documentation

- Parameter details: see `energy/README.md`
- Algorithm pseudocode: see `docs/` directory
- Data format: see `tables/README.md`

## Paper Citation

If using this project, please cite the corresponding academic paper. See project documentation for details.

## License

Research use only
