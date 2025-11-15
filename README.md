# GMERS: Energy-Efficient Connected Dominating Set Algorithm for Wireless Sensor Networks

## Project Overview

This project implements the GMERS (Greedy Minimum Energy Ratio Selection) algorithm for constructing Connected Dominating Sets (CDS) in wireless sensor networks. By finding an optimal balance between energy consumption and coverage rate, it significantly extends network lifetime.

### Key Innovations
- **Alpha Parameter Balancing Mechanism**: The α∈[0,1] parameter balances energy efficiency (energy weight) and coverage performance (neighborhood degree)
- **Greedy Selection Strategy**: Dynamically computes node energy ratios and prioritizes nodes with sufficient energy
- **Connectivity Guarantee**: Ensures the network remains connected throughout the CDS construction process

## Directory Structure

```
cecs/
├── energy/                       Core experiment code directory
│   ├── Source Code
│   │   ├── graph.hpp            Graph data structure definition
│   │   ├── graphgene.cpp        Graph generation program (two modes)
│   │   ├── simulations.cpp      Main experiment executor (Ex1-Ex7)
│   │   ├── simulations_supplement.cpp  Supplementary experiment executor (4 experiments)
│   │   └── decideab.cpp         Ex0-Alpha experiment (alpha scanning)
│   │
│   ├── Execution Scripts
│   │   ├── run_experiments.sh   One-click run all experiments
│   │   └── run_supplement.sh    Run supplementary experiments
│   │
│   ├── Plotting Scripts
│   │   ├── plot.py              Main experiment plotting
│   │   └── plot_all_supplement.py  Supplementary experiment plotting
│   │
│   ├── Data
│   │   ├── smaller/             Graph data (small degree)
│   │   ├── equal/               Graph data (medium degree)
│   │   ├── greater/             Graph data (large degree)
│   │   └── output/              Generated results output
│   │       ├── graphs/          Generated graph files
│   │       ├── ex0_alpha/       Ex0-Alpha results
│   │       ├── ex1-ex7/         Main experiment results
│   │       └── exX/             Supplementary experiment results
│   │
│   └── Documentation
│       └── README.md            Detailed usage guide
│
├── docs/                        Project documentation
├── figures/                     Generated plots
├── tables/                      Exported CSV data
└── README.md                    This file
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

**Ex1**: Performance comparison of GMERS with traditional methods
- Compare lifetime, convergence time, computational cost

**Ex2**: Impact of different initial energy allocation schemes
- Uniform vs. non-uniform allocation

**Ex3**: Impact of network topology on CDS performance
- Performance under different node densities

**Ex4**: Single parameter optimization
- Sensitivity analysis of α

**Ex5**: Dynamic frame sequence analysis
- CDS member changes over time

**Ex6**: Multi-parameter joint optimization
- Combined effects of multiple parameters

**Ex7**: Scenario-specific optimization
- Energy-constrained or delay-sensitive scenarios

### Supplementary Experiments

**Ex-Alpha-Sweep**: Extended parameter scanning (α∈[0, 1.2], showing out-of-range degradation)

**Ex-Large**: Large-scale network scalability (N up to 300 nodes)

**Ex-CDS-Stability**: CDS stability analysis
- Jaccard similarity (J_t)
- Member change rate (τ_t)

**Ex-Energy-Model**: Comparison of different energy distribution models
- Exponential distribution
- Gamma distribution
- Lognormal distribution
- Weibull distribution

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

### Plot Files (figures/)

| File | Source | Content |
|------|--------|---------|
| figure1.png - figure7.png | Ex1-Ex7 | Main experiment result curves |
| figure3_alpha_scan.png | Ex0-Alpha | Alpha scanning curves |
| supplement/*.png | Supplementary experiments | Supplementary experiment plots |

### Data Files (tables/)

| File | Source | Format |
|------|--------|--------|
| *.csv | Supplementary experiments | CSV format data |
| README.md | Data description | Field explanation document |

### Experiment Results (output/)

| Directory | Content |
|-----------|---------|
| graphs/ | Generated topology graph files |
| ex0_alpha/ | Ex0-Alpha raw output |
| ex1-ex7/ | Main experiment raw output |
| exX/ | Supplementary experiment raw output |

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
# Individual compilation
g++ -o graphgene.exe graphgene.cpp -std=c++17
g++ -o simulations.exe simulations.cpp -std=c++17 -fopenmp
g++ -o decideab.exe decideab.cpp -std=c++17
g++ -o simulations_supplement.exe simulations_supplement.cpp -std=c++17 -fopenmp

# Or use script for automatic compilation
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
