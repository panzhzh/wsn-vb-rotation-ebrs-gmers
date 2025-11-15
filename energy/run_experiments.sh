#!/bin/bash

set -e  # Exit on error

echo "=================================================="
echo "Complete Paper Experiment Execution Script (All Experiments ex0-ex7)"
echo "=================================================="
echo ""

cd "$(dirname "$0")"

# ============================================================
# STEP 1: Check and compile necessary files
# ============================================================
echo "[STEP 1] Checking and compiling necessary executable files..."
echo "=================================================="

if [ ! -f ./graphgene.exe ]; then
    echo "Compiling graphgene..."
    g++ -o graphgene.exe graphgene.cpp -std=c++17 2>&1 | grep -i error || true
fi

if [ ! -f ./simulations.exe ]; then
    echo "Compiling simulations..."
    g++ -o simulations.exe simulations.cpp -std=c++17 -fopenmp 2>&1 | grep -i error || true
fi

if [ ! -f ./decideab.exe ]; then
    echo "Compiling decideab (Ex0-Alpha)..."
    g++ -o decideab.exe decideab.cpp -std=c++17 2>&1 | grep -i error || true
fi

echo "✓ Compilation check completed"
echo ""

# ============================================================
# STEP 2: Create output directories
# ============================================================
echo "[STEP 2] Creating output directories..."
echo "=================================================="

mkdir -p ./output/graphs
mkdir -p ./output/alpha_scan
for k in {1..7}; do
    mkdir -p ./output/ex$k
done
mkdir -p ./figures ./tables

echo "✓ Output directories created"
echo ""

# ============================================================
# STEP 3: Generate graph data Mode 1 (Ex1-Ex7)
# ============================================================
echo "[STEP 3] Generating graph data Mode 1 (Ex1-Ex7)..."
echo "=================================================="
echo "Parameters: R=250, N=10-100, 100 samples/N"
echo ""

echo "1" | ./graphgene.exe

GRAPH_COUNT=$(find ./output/graphs/R=250 -type f -name "net*.txt" 2>/dev/null | wc -l)
echo ""
echo "✓ Mode 1 graph generation completed! Generated $GRAPH_COUNT graph files"
echo ""

# ============================================================
# STEP 4: Generate graph data Mode 2 (Ex0-Alpha)
# ============================================================
echo "[STEP 4] Generating graph data Mode 2 (Ex0-Alpha)..."
echo "=================================================="
echo "Parameters: Three degree types (smaller/equal/greater), N=50,100"
echo ""

echo "2" | ./graphgene.exe

ALPHA_GRAPH_COUNT=$(find ./output/graphs -type d -name "smaller" -o -name "equal" -o -name "greater" | wc -l)
echo ""
echo "✓ Mode 2 graph generation completed!"
echo ""

# ============================================================
# STEP 5: Run Ex0-Alpha (decideab)
# ============================================================
echo "[STEP 5] Running Ex0-Alpha experiment (alpha scanning)..."
echo "=================================================="
echo "Parameters: α=0.0, 0.1, ..., 1.0"
echo "Targets: Three degree types (smaller/equal/greater), N=50,100"
echo ""

./decideab.exe

echo ""
echo "✓ Ex0-Alpha completed! Results saved to ./output/alpha_scan/"
echo ""

# ============================================================
# STEP 6: Run Ex1-Ex7 (main experiments)
# ============================================================
echo "[STEP 6] Running main experiments Ex1-Ex7..."
echo "=================================================="

NUM_CORES=$(nproc)
echo "System detected $NUM_CORES CPU cores"
echo "Using OpenMP multi-threading for computation..."
echo ""

export OMP_NUM_THREADS=$NUM_CORES
./simulations.exe

echo ""
echo "✓ Ex1-Ex7 completed!"
echo ""

# ============================================================
# STEP 7: Verify output
# ============================================================
echo "[STEP 7] Verifying output files..."
echo "=================================================="

echo ""
echo "Output locations:"
echo "  Ex0-Alpha results:  ./output/alpha_scan/"
echo "  Ex1-Ex7 results:    ./output/ex{1..7}/"
echo ""

echo "=================================================="
echo "✓ All experiments completed!"
echo "=================================================="
echo ""
