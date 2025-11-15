# Energy Optimization Experiments - Complete Guide

## 📋 目录结构

```
energy/
├── README.md                          # 本文档
│
├── 源代码文件
│   ├── graphgene.cpp                  # 图生成程序
│   ├── simulations.cpp                # 主论文实验 (ex1-ex7)
│   ├── simulations_supplement.cpp     # 补充实验 (4个)
│   ├── decideab.cpp                   # Ex0-Alpha 实验 (alpha扫描)
│   └── graph.hpp                      # 图数据结构定义
│
├── 执行脚本
│   ├── run_experiments.sh             # ★ 主脚本：执行所有实验 (ex0-ex7)
│   ├── run_supplement.sh              # 补充实验专用脚本
│   └── plot*.py                       # 绘图脚本
│
└── 输出目录 (执行脚本后生成)
    ├── output/
    │   ├── graphs/                    # 生成的图数据
    │   ├── ex0_alpha/                 # Ex0-Alpha 结果 (alpha扫描)
    │   ├── alpha_scan/                # Ex0-Alpha 结果 (原目录名)
    │   ├── ex1/ ~ ex7/                # 主实验结果
    │   └── exX/ (补充)                # 补充实验结果
    ├── figures/                       # 生成的图表
    ├── supplement/                    # 补充实验图表
    └── tables/                        # CSV数据文件
```

---

## 🔧 源代码说明

### 1. **graphgene.cpp** - 图生成程序
**功能**: 生成单位圆盘随机几何图和特定度数分布的图

**编译**:
```bash
g++ -o graphgene.exe graphgene.cpp -std=c++17
```

**执行**:
```bash
./graphgene.exe
# 选择模式:
#   1: 单位圆盘随机图 (R=250, N=10-100, 100个样本/N)
#      输出: ./output/graphs/R=250/N=*/net{0..99}.txt
#   2: 特定度数图 (smaller/equal/greater)
#      输出: ./output/graphs/{smaller|equal|greater}/N={50,100}/net{0..99}.txt
```

---

### 2. **simulations.cpp** - 主论文实验 (Ex1-Ex7)
**功能**: 实现7个主要实验，测试GMERS算法的性能

**编译**:
```bash
g++ -o simulations.exe simulations.cpp -std=c++17 -fopenmp
```

**执行**:
```bash
export OMP_NUM_THREADS=<num>  # 推荐使用所有CPU核心
./simulations.exe
# 输出: ./output/ex{1..7}/
```

**线程数参数**:
- 推荐: `export OMP_NUM_THREADS=$(nproc)` (使用所有核心)
- 默认: 系统自动分配
- 最小: 1 (单线程)
- 最大: CPU核心数

---

### 3. **simulations_supplement.cpp** - 补充实验 (4个)
**功能**: 实现4个补充实验

**编译**:
```bash
g++ -o simulations_supplement.exe simulations_supplement.cpp -std=c++17 -fopenmp
```

**执行**:
```bash
export OMP_NUM_THREADS=16
./simulations_supplement.exe [experiment_name]

# 可选参数:
#   alpha_sweep       Ex-Alpha-Sweep (α参数扫描)
#   large            Ex-Large (大规模网络)
#   stability        Ex-CDS-Stability (稳定性)
#   energy_model     Ex-Energy-Model (能量模型)
#   all              执行所有补充实验
```

**输出**:
- `./output/ex_alpha_sweep/`
- `./output/ex_large/`
- `./output/ex_cds_stability/`
- `./output/ex_energy_model/`

---

### 4. **decideab.cpp** - Ex0-Alpha (Alpha扫描)
**功能**: Alpha参数扫描实验，测试α=0.0-1.0对网络寿命的影响

**编译**:
```bash
g++ -o decideab.exe decideab.cpp -std=c++17
```

**执行**:
```bash
./decideab.exe
# 输入数据: ./output/graphs/{smaller|equal|greater}/N={50,100}/net*.txt
# 输出数据: ./output/alpha_scan/{smaller|equal|greater}/N=*.txt
```

**参数**:
- **α值**: 0.0, 0.1, 0.2, ..., 1.0 (11个点)
- **度数类型**: smaller, equal, greater
- **节点数**: N=50, 100
- **样本数**: 100 (每配置)

---

### 5. **graph.hpp** - 图数据结构
**功能**: 定义 `Graph` 类，用于图的存储和操作

**包含**:
- `number_of_nodes`: 节点数
- `outneighbors()`: 获取邻居列表
- `read()`: 从文件读取图
- `write()`: 将图写入文件

---

## 🚀 执行脚本说明

### ★ 主脚本: `run_experiments.sh`

**功能**: 一键执行完整的 ex0-ex7 实验流程

**使用方式**:
```bash
cd energy/
bash run_experiments.sh
```

**执行流程**:

| STEP | 操作 | 命令 | 输出位置 |
|------|------|------|--------|
| 1 | 检查/编译 | 自动编译 graphgene, simulations, decideab | - |
| 2 | 创建目录 | mkdir output/graphs 等 | - |
| 3 | 生成图 Mode 1 | `echo 1 \| graphgene.exe` | ./output/graphs/R=250/ |
| 4 | 生成图 Mode 2 | `echo 2 \| graphgene.exe` | ./output/graphs/{smaller\|equal\|greater}/ |
| 5 | Ex0-Alpha | `./decideab.exe` | ./output/alpha_scan/ |
| 6 | Ex1-Ex7 | `./simulations.exe` | ./output/ex{1..7}/ |
| 7 | 验证 | 统计输出文件 | - |

**预期运行时间**:
- STEP 1-2: < 1 分钟
- STEP 3: 10-15 分钟 (图生成)
- STEP 4: 5-10 分钟 (图生成)
- STEP 5: 10-15 分钟 (alpha扫描)
- STEP 6: 30分钟-2小时 (主实验，取决于CPU)

**总耗时**: 约 2-3 小时（使用多核CPU）

---

### 补充脚本: `run_supplement.sh`

**功能**: 执行4个补充实验

**使用方式**:
```bash
cd energy/
bash run_supplement.sh
```

**或单独运行某个补充实验**:
```bash
export OMP_NUM_THREADS=16
./simulations_supplement.exe alpha_sweep
./simulations_supplement.exe large
./simulations_supplement.exe stability
./simulations_supplement.exe energy_model
```

---

## 📊 参数详解

### OpenMP 线程数设置

**主实验推荐**:
```bash
export OMP_NUM_THREADS=$(nproc)  # 使用所有CPU核心
./simulations.exe
```

**补充实验推荐**:
```bash
export OMP_NUM_THREADS=16
./simulations_supplement.exe all
```

**Ex0-Alpha**:
```bash
./decideab.exe  # 单线程 (原代码不支持并行)
```

**查看CPU核心数**:
```bash
nproc                    # 查看物理核心数
lscpu | grep "CPU(s)"    # 详细信息
```

### 实验参数

#### Ex0-Alpha (Alpha扫描)
- **α范围**: 0.0 → 1.0 (11个点，步长0.1)
- **度数类型**: 3种 (smaller/equal/greater)
- **节点数**: 2种 (N=50, 100)
- **样本数**: 100 (每配置)
- **总处理**: 6×11 = 66组实验

#### Ex1-Ex7 (主实验)
- **拓扑**: 单位圆盘随机图 (R=250)
- **节点数**: N=10, 20, 30, ..., 100 (10种)
- **样本数**: 100 (每N值)
- **总处理**: 1000个图

#### 补充实验
- **Alpha Sweep**: α=0.1-0.9 (9个点)
- **Large**: N=100-1000 (可扩展性测试)
- **Stability**: CDS成员变化分析
- **Energy Model**: 4种能量分布 (Exp, Gamma, Lognormal, Weibull)

---

## 📈 输出数据格式

### 实验结果 (.txt)

**Ex0-Alpha 结果格式**:
```
# Alpha 扫描结果 (N=50, smaller 度数)
# alpha_value  average_lifetime
0.0 XXXXX
0.1 XXXXX
...
1.0 XXXXX
```

**主实验结果格式**:
```
N=10
...
<metric1> <metric2> <metric3> ...
```

### CSV 数据 (tables/)

**补充实验导出的CSV**:
- `alpha_sweep_lifetime.csv` - Alpha扫描网络寿命
- `alpha_sweep_cds_size.csv` - Alpha扫描CDS大小
- `large_scalability.csv` - 大规模可扩展性
- `stability_metrics.csv` - 稳定性指标
- `energy_model_comparison.csv` - 能量模型对比
- `README.md` - CSV字段说明

---

## 🎯 完整使用示例

### 场景1: 快速测试 (完整流程)
```bash
cd energy/
bash run_experiments.sh
```
执行时间: ~2-3小时

### 场景2: 只运行主实验
```bash
cd energy/

# 1. 生成主实验所需的图 (Mode 1)
echo "1" | ./graphgene.exe

# 2. 运行主实验
export OMP_NUM_THREADS=$(nproc)
./simulations.exe

# 3. 绘图
python3 plot.py
```
执行时间: ~1小时

### 场景3: 只运行 Ex0-Alpha
```bash
cd energy/

# 1. 生成 Ex0-Alpha 所需的图 (Mode 2)
echo "2" | ./graphgene.exe

# 2. 运行 alpha 扫描
./decideab.exe

# 3. 查看结果
cat ./output/alpha_scan/smaller/N=50_result.txt
```
执行时间: ~20分钟

### 场景4: 只运行补充实验
```bash
cd energy/

# 1. 编译
g++ -o simulations_supplement.exe simulations_supplement.cpp -std=c++17 -fopenmp

# 2. 运行所有补充实验
export OMP_NUM_THREADS=16
./simulations_supplement.exe all

# 3. 生成图表和CSV
python3 plot_all_supplement.py
```
执行时间: ~30分钟

---

## ⚙️ 高级用法

### 修改参数

#### 修改 Ex0-Alpha 的 α 值范围
编辑 `decideab.cpp` 第 189-191 行:
```cpp
for(double a = 0.1; a <= 1.0; a += 0.1)  // 修改步长
```

#### 修改 Ex1-Ex7 的网络规模
编辑 `simulations.cpp` 中的参数声明

#### 修改补充实验的线程数
```bash
export OMP_NUM_THREADS=32  # 改为32线程
./simulations_supplement.exe all
```

### 并行运行多个实验
```bash
# 运行 graphgene 和 simulations 并行
echo "1" | ./graphgene.exe &
GRAPH_PID=$!
wait $GRAPH_PID

export OMP_NUM_THREADS=$(nproc)
./simulations.exe &
SIM_PID=$!

# 同时运行补充实验
export OMP_NUM_THREADS=8
./simulations_supplement.exe all

wait $SIM_PID
```

---

## 🔍 故障排除

### 问题1: 编译错误
**症状**: `error: ... -fopenmp: command not found`

**解决**:
```bash
# 安装 OpenMP
sudo apt install libomp-dev  # Ubuntu/Debian
brew install libomp          # macOS
```

### 问题2: 内存不足
**症状**: `std::bad_alloc` 或进程被杀死

**解决**:
- 减少样本数 (修改源代码)
- 减少线程数: `export OMP_NUM_THREADS=4`
- 使用更小的 N 值进行测试

### 问题3: 图文件不存在
**症状**: "文件不存在" 错误

**解决**:
```bash
# 确保先运行图生成
echo "1" | ./graphgene.exe
echo "2" | ./graphgene.exe

# 检查生成的文件
ls output/graphs/R=250/N=10/
ls output/graphs/smaller/N=50/
```

### 问题4: 脚本权限不足
**症状**: `Permission denied: ./run_experiments.sh`

**解决**:
```bash
chmod +x run_experiments.sh
chmod +x run_supplement.sh
bash run_experiments.sh  # 用 bash 执行
```

---

## 📝 快速参考

| 任务 | 命令 | 执行时间 |
|------|------|--------|
| 执行所有实验 (ex0-ex7) | `bash run_experiments.sh` | ~2-3小时 |
| 仅生成图 Mode 1 | `echo 1 \| ./graphgene.exe` | ~10分钟 |
| 仅生成图 Mode 2 | `echo 2 \| ./graphgene.exe` | ~5分钟 |
| 仅运行 Ex0-Alpha | `./decideab.exe` | ~10分钟 |
| 仅运行主实验 | `export OMP_NUM_THREADS=$(nproc); ./simulations.exe` | ~1小时 |
| 仅运行补充实验 | `export OMP_NUM_THREADS=16; ./simulations_supplement.exe all` | ~30分钟 |
| 编译所有 | `g++ -o *.exe *.cpp -std=c++17 -fopenmp` | ~30秒 |

---

## 📞 联系和反馈

如有问题或建议，请查看:
- 各脚本文件的注释
- 源代码中的详细文档
- tables/README.md (CSV数据说明)

---

**最后更新**: 2025-10-26
**版本**: 2.0
**状态**: ✅ 完成
