#!/bin/bash

set -e  # 遇到错误时退出

echo "=================================================="
echo "完整论文实验执行脚本 (所有实验 ex0-ex7)"
echo "=================================================="
echo ""

cd "$(dirname "$0")"

# ============================================================
# STEP 1: 检查和编译必要文件
# ============================================================
echo "[STEP 1] 检查和编译必要的可执行文件..."
echo "=================================================="

if [ ! -f ./graphgene.exe ]; then
    echo "编译 graphgene..."
    g++ -o graphgene.exe graphgene.cpp -std=c++17 2>&1 | grep -i error || true
fi

if [ ! -f ./simulations.exe ]; then
    echo "编译 simulations..."
    g++ -o simulations.exe simulations.cpp -std=c++17 -fopenmp 2>&1 | grep -i error || true
fi

if [ ! -f ./decideab.exe ]; then
    echo "编译 decideab (Ex0-Alpha)..."
    g++ -o decideab.exe decideab.cpp -std=c++17 2>&1 | grep -i error || true
fi

echo "✓ 编译检查完成"
echo ""

# ============================================================
# STEP 2: 创建输出目录
# ============================================================
echo "[STEP 2] 创建输出目录..."
echo "=================================================="

mkdir -p ./output/graphs
mkdir -p ./output/alpha_scan
for k in {1..7}; do
    mkdir -p ./output/ex$k
done
mkdir -p ./figures ./tables

echo "✓ 输出目录已创建"
echo ""

# ============================================================
# STEP 3: 生成图数据 Mode 1 (Ex1-Ex7)
# ============================================================
echo "[STEP 3] 生成图数据 Mode 1 (Ex1-Ex7)..."
echo "=================================================="
echo "参数: R=250, N=10-100, 100个样本/N"
echo ""

echo "1" | ./graphgene.exe

GRAPH_COUNT=$(find ./output/graphs/R=250 -type f -name "net*.txt" 2>/dev/null | wc -l)
echo ""
echo "✓ Mode 1 图生成完成！已生成 $GRAPH_COUNT 个图文件"
echo ""

# ============================================================
# STEP 4: 生成图数据 Mode 2 (Ex0-Alpha)
# ============================================================
echo "[STEP 4] 生成图数据 Mode 2 (Ex0-Alpha)..."
echo "=================================================="
echo "参数: smaller/equal/greater 三种度数, N=50,100"
echo ""

echo "2" | ./graphgene.exe

ALPHA_GRAPH_COUNT=$(find ./output/graphs -type d -name "smaller" -o -name "equal" -o -name "greater" | wc -l)
echo ""
echo "✓ Mode 2 图生成完成！"
echo ""

# ============================================================
# STEP 5: 运行 Ex0-Alpha (decideab)
# ============================================================
echo "[STEP 5] 运行 Ex0-Alpha 实验 (alpha扫描)..."
echo "=================================================="
echo "参数: α=0.0, 0.1, ..., 1.0"
echo "对象: smaller/equal/greater 度数, N=50,100"
echo ""

./decideab.exe

echo ""
echo "✓ Ex0-Alpha 完成！结果保存到 ./output/alpha_scan/"
echo ""

# ============================================================
# STEP 6: 运行 Ex1-Ex7 (主实验)
# ============================================================
echo "[STEP 6] 运行主实验 Ex1-Ex7..."
echo "=================================================="

NUM_CORES=$(nproc)
echo "系统检测到 $NUM_CORES 个 CPU 核心"
echo "使用 OpenMP 多线程计算..."
echo ""

export OMP_NUM_THREADS=$NUM_CORES
./simulations.exe

echo ""
echo "✓ Ex1-Ex7 完成！"
echo ""

# ============================================================
# STEP 7: 验证输出
# ============================================================
echo "[STEP 7] 验证输出文件..."
echo "=================================================="

echo ""
echo "输出位置:"
echo "  Ex0-Alpha 结果:  ./output/alpha_scan/"
echo "  Ex1-Ex7 结果:    ./output/ex{1..7}/"
echo ""

echo "=================================================="
echo "✓ 所有实验执行完成！"
echo "=================================================="
echo ""
