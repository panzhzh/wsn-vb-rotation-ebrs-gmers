#!/bin/bash

# Supplement Experiments Execution Script
# Runs all 4 supplementary experiments: alpha_sweep, large, stability, energy_model

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Color definitions
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo ""
echo "========================================================================"
echo " Supplement Experiments - $(date '+%Y-%m-%d %H:%M:%S')"
echo "========================================================================"
echo ""

# Check simulations_supplement.exe
if [ ! -f "./simulations_supplement.exe" ]; then
    echo -e "${BLUE}[INFO]${NC} Compiling simulations_supplement.exe..."
    g++ -o simulations_supplement.exe simulations_supplement.cpp -std=c++17 -fopenmp
    if [ $? -ne 0 ]; then
        echo -e "${RED}[ERROR]${NC} Compilation failed"
        exit 1
    fi
fi

# Get command parameter
COMMAND="${1:-help}"

case "$COMMAND" in
    alpha_sweep)
        echo -e "${BLUE}[INFO]${NC} Running Ex-Alpha-Sweep experiment..."
        mkdir -p ./output/ex_alpha_sweep
        ./simulations_supplement.exe alpha_sweep 2>&1 | tee ./output/ex_alpha_sweep/run.log
        RET=${PIPESTATUS[0]}
        if [ $RET -eq 0 ]; then
            echo -e "${GREEN}[SUCCESS]${NC} Ex-Alpha-Sweep completed"
            echo ""
            echo -e "${BLUE}[INFO]${NC} Generating plots..."
            python3 plot_all_supplement.py --experiment alpha_sweep 2>&1 | tee -a ./output/ex_alpha_sweep/run.log || true
            echo -e "${GREEN}[SUCCESS]${NC} Results saved to output/ex_alpha_sweep/"
        else
            echo -e "${RED}[ERROR]${NC} Ex-Alpha-Sweep failed (exit code: $RET)"
            exit $RET
        fi
        ;;

    large)
        echo -e "${BLUE}[INFO]${NC} Running Ex-Large experiment..."
        mkdir -p ./output/ex_large
        ./simulations_supplement.exe large 2>&1 | tee ./output/ex_large/run.log
        RET=${PIPESTATUS[0]}
        if [ $RET -eq 0 ]; then
            echo -e "${GREEN}[SUCCESS]${NC} Ex-Large completed"
            echo ""
            echo -e "${BLUE}[INFO]${NC} Generating plots..."
            python3 plot_all_supplement.py --experiment large 2>&1 | tee -a ./output/ex_large/run.log || true
            echo -e "${GREEN}[SUCCESS]${NC} Results saved to output/ex_large/"
        else
            echo -e "${RED}[ERROR]${NC} Ex-Large failed (exit code: $RET)"
            exit $RET
        fi
        ;;

    stability)
        echo -e "${BLUE}[INFO]${NC} Running Ex-CDS-Stability experiment..."
        mkdir -p ./output/ex_cds_stability
        ./simulations_supplement.exe stability 2>&1 | tee ./output/ex_cds_stability/run.log
        RET=${PIPESTATUS[0]}
        if [ $RET -eq 0 ]; then
            echo -e "${GREEN}[SUCCESS]${NC} Ex-CDS-Stability completed"
            echo ""
            echo -e "${BLUE}[INFO]${NC} Generating plots..."
            python3 plot_all_supplement.py --experiment stability 2>&1 | tee -a ./output/ex_cds_stability/run.log || true
            echo -e "${GREEN}[SUCCESS]${NC} Results saved to output/ex_cds_stability/"
        else
            echo -e "${RED}[ERROR]${NC} Ex-CDS-Stability failed (exit code: $RET)"
            exit $RET
        fi
        ;;

    energy_model)
        echo -e "${BLUE}[INFO]${NC} Running Ex-Energy-Model experiment..."
        mkdir -p ./output/ex_energy_model
        ./simulations_supplement.exe energy_model 2>&1 | tee ./output/ex_energy_model/run.log
        RET=${PIPESTATUS[0]}
        if [ $RET -eq 0 ]; then
            echo -e "${GREEN}[SUCCESS]${NC} Ex-Energy-Model completed"
            echo ""
            echo -e "${BLUE}[INFO]${NC} Generating plots..."
            python3 plot_all_supplement.py --experiment energy_model 2>&1 | tee -a ./output/ex_energy_model/run.log || true
            echo -e "${GREEN}[SUCCESS]${NC} Results saved to output/ex_energy_model/"
        else
            echo -e "${RED}[ERROR]${NC} Ex-Energy-Model failed (exit code: $RET)"
            exit $RET
        fi
        ;;

    all)
        echo -e "${BLUE}[INFO]${NC} Running all supplement experiments sequentially..."
        echo ""

        SCRIPT_PATH="$SCRIPT_DIR/run_supplement.sh"
        for exp in alpha_sweep large stability energy_model; do
            echo "========================================================================"
            echo " Starting: $exp"
            echo "========================================================================"
            bash "$SCRIPT_PATH" "$exp" || echo -e "${YELLOW}[WARNING]${NC} $exp encountered issues, continuing..."
            echo ""
        done

        echo "========================================================================"
        echo -e "${GREEN}[SUCCESS]${NC} All experiments completed"
        echo "========================================================================"
        ;;

    check)
        echo -e "${BLUE}[INFO]${NC} Checking environment..."
        echo ""

        if [ -f "./simulations_supplement.exe" ]; then
            echo -e "${GREEN}✓${NC} simulations_supplement.exe exists"
        else
            echo -e "${RED}✗${NC} simulations_supplement.exe not found"
        fi

        if python3 -c "import matplotlib" 2>/dev/null; then
            echo -e "${GREEN}✓${NC} Python matplotlib available"
        else
            echo -e "${RED}✗${NC} Python matplotlib not available (may affect plotting)"
        fi

        for exp in ex_alpha_sweep ex_large ex_cds_stability ex_energy_model; do
            if [ -d "./output/$exp" ]; then
                count=$(ls ./output/$exp/*.txt 2>/dev/null | wc -l)
                echo -e "${GREEN}✓${NC} output/$exp (contains $count result files)"
            else
                echo -e "${YELLOW}○${NC} output/$exp (not yet generated)"
            fi
        done
        ;;

    help|--help|-h)
        cat << EOF

Supplement Experiments Execution Script

Usage:
    bash run_supplement.sh [command]

Commands:
    alpha_sweep      Run Ex-Alpha-Sweep experiment
    large            Run Ex-Large experiment
    stability        Run Ex-CDS-Stability experiment
    energy_model     Run Ex-Energy-Model experiment
    all              Run all supplement experiments sequentially
    check            Check environment and data status
    help             Show this help message

Examples:
    bash run_supplement.sh alpha_sweep
    bash run_supplement.sh all
    bash run_supplement.sh check

Output Locations:
    - Ex-Alpha-Sweep:     output/ex_alpha_sweep/
    - Ex-Large:           output/ex_large/
    - Ex-CDS-Stability:   output/ex_cds_stability/
    - Ex-Energy-Model:    output/ex_energy_model/

Notes:
    1. All output files go to project root output/ directory
    2. Plotting requires matplotlib and numpy
    3. Each experiment generates logs to output/ex_*/run.log

Compile Command:
    g++ -o simulations_supplement.exe simulations_supplement.cpp -std=c++17 -fopenmp

EOF
        ;;

    *)
        echo -e "${RED}[ERROR]${NC} Unknown command: $COMMAND"
        echo "Use 'bash run_supplement.sh help' for usage"
        exit 1
        ;;
esac

echo ""
echo "========================================================================"
echo " Completed at $(date '+%Y-%m-%d %H:%M:%S')"
echo "========================================================================"
echo ""
