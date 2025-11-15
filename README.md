# GMERS: 无线传感网络能源高效连通支配集算法

## 项目简介

本项目实现了 GMERS（贪心最小能量比率选择）算法，用于无线传感网络中的连通支配集(CDS)构建。通过在能源消耗和覆盖率之间找到最优平衡，实现显著延长网络寿命的目标。

### 核心创新
- **Alpha参数权衡机制**：通过α∈[0,1]参数平衡能源效率（能量权重）和覆盖性能（邻域度数）
- **贪心选择策略**：动态计算节点的能源比率，优先选择能源充足的节点
- **连通性保证**：确保CDS构建过程中网络始终保持连通

## 目录结构

```
cecs/
├── energy/                       核心实验代码目录
│   ├── 源代码
│   │   ├── graph.hpp            图数据结构定义
│   │   ├── graphgene.cpp        图生成程序（两种模式）
│   │   ├── simulations.cpp      主实验执行器 (Ex1-Ex7)
│   │   ├── simulations_supplement.cpp  补充实验执行器 (4个)
│   │   └── decideab.cpp         Ex0-Alpha实验 (alpha扫描)
│   │
│   ├── 执行脚本
│   │   ├── run_experiments.sh   一键执行所有实验
│   │   └── run_supplement.sh    执行补充实验
│   │
│   ├── 绘图脚本
│   │   ├── plot.py              主实验绘图
│   │   └── plot_all_supplement.py  补充实验绘图
│   │
│   ├── 数据
│   │   ├── smaller/             原始图数据（小度数）
│   │   ├── equal/               原始图数据（中度数）
│   │   ├── greater/             原始图数据（大度数）
│   │   └── output/              生成结果输出
│   │       ├── graphs/          生成的图文件
│   │       ├── ex0_alpha/       Ex0-Alpha结果
│   │       ├── ex1-ex7/         主实验结果
│   │       └── exX/             补充实验结果
│   │
│   └── 文档
│       └── README.md            详细使用说明
│
├── docs/                        项目文档
├── figures/                     生成的图表
├── tables/                      导出的CSV数据
└── README.md                    本文件
```

## 实验说明

### Ex0-Alpha: Alpha参数扫描实验

**目标**：验证α参数对网络寿命的影响，找到最优的权衡点

**参数范围**：
- α ∈ [0.1, 1.0]，步长 0.1
- 图类型：smaller (度数=N/4), equal (度数=N/2), greater (度数=3N/4)
- 节点数：N=50, 100
- 每配置100个样本图

**输出**：
- 网络寿命(lifetime)随α变化的曲线
- 支配集大小随α变化的趋势
- CSV格式的详细数据

### Ex1-Ex7: 主论文实验

**图生成**：单位圆盘随机几何图 (R=250, N=10-100, 每N值100个样本)

**Ex1**：GMERS与传统方法的性能对比
- 比较寿命、收敛时间、计算开销

**Ex2**：不同初始能量分配方案的影响
- 均匀分配 vs 不均匀分配

**Ex3**：网络拓扑对CDS性能的影响
- 不同节点密度下的表现

**Ex4**：单参数优化
- α的敏感性分析

**Ex5**：动态帧序列分析
- CDS成员的变化过程

**Ex6**：多参数联合优化
- 多个参数的共同影响

**Ex7**：特定场景优化
- 能源受限或延迟敏感等场景

### 补充实验

**Ex-Alpha-Sweep**：扩展的参数扫描 (α∈[0, 1.2], 展示区间外退化现象)

**Ex-Large**：大规模网络可扩展性 (N到300节点)

**Ex-CDS-Stability**：CDS稳定性分析
- Jaccard相似度 (J_t)
- 成员变化率 (τ_t)

**Ex-Energy-Model**：不同能量分布模型对比
- Exponential (指数分布)
- Gamma (伽马分布)
- Lognormal (对数正态分布)
- Weibull (韦布尔分布)

## 快速开始

### 执行全部实验

```bash
cd energy/
bash run_experiments.sh
```

该脚本自动执行：
1. **编译检查**：确保所有可执行文件已编译
2. **目录创建**：建立输出目录结构
3. **图生成**：Mode 1 (主实验用) 和 Mode 2 (Ex0-Alpha用)
4. **Ex0-Alpha执行**：α参数扫描实验
5. **主实验执行**：Ex1-Ex7全部实验
6. **结果验证**：检查输出文件完整性

预计耗时：**2-3小时**（使用多核CPU）

### 执行补充实验

```bash
cd energy/
bash run_supplement.sh
```

单独执行某个补充实验：
```bash
export OMP_NUM_THREADS=16
./simulations_supplement.exe alpha_sweep    # 或 large, stability, energy_model
```

## 输出说明

### 图表文件 (figures/)

| 文件 | 来源 | 内容 |
|------|------|------|
| figure1.png - figure7.png | Ex1-Ex7 | 主实验结果曲线 |
| figure3_alpha_scan.png | Ex0-Alpha | Alpha扫描曲线 |
| supplement/*.png | 补充实验 | 补充实验图表 |

### 数据文件 (tables/)

| 文件 | 来源 | 格式 |
|------|------|------|
| *.csv | 补充实验 | CSV格式数据 |
| README.md | 数据说明 | 字段解释文档 |

### 实验结果 (output/)

| 目录 | 内容 |
|------|------|
| graphs/ | 生成的拓扑图文件 |
| ex0_alpha/ | Ex0-Alpha原始输出 |
| ex1-ex7/ | 主实验原始输出 |
| exX/ | 补充实验原始输出 |

## 参数配置

### OpenMP线程数设置

```bash
# 主实验：使用所有CPU核心
export OMP_NUM_THREADS=$(nproc)
./simulations.exe

# Ex0-Alpha：推荐16线程
export OMP_NUM_THREADS=16
./decideab.exe

# 补充实验：推荐16线程
export OMP_NUM_THREADS=16
./simulations_supplement.exe all
```

### 实验参数修改

在源代码中修改：

**graphgene.cpp**：
- `degree = N/4, N/2, N*3/4` 修改度数比例
- `r = 250` 修改通信半径

**decideab.cpp**：
- `for(double a = 0.1; a <= 1.0; a += 0.1)` 修改α范围和步长
- `N=50, 100` 修改测试节点数

**simulations_supplement.cpp**：
- 修改各补充实验的参数范围

## 系统要求

### 编译环境
- **C++标准**：C++17 或更高
- **编译器**：g++ 7.0+ 或 clang++ 5.0+
- **OpenMP**：支持并行计算

### 系统环境
- **内存**：至少 2GB
- **硬盘**：至少 10GB（用于输出结果）
- **CPU**：多核处理器（推荐4核以上）

### 依赖库
- 标准C++库（自带）
- OpenMP（大多数编译器自带）

## 编译命令

```bash
# 单独编译
g++ -o graphgene.exe graphgene.cpp -std=c++17
g++ -o simulations.exe simulations.cpp -std=c++17 -fopenmp
g++ -o decideab.exe decideab.cpp -std=c++17
g++ -o simulations_supplement.exe simulations_supplement.cpp -std=c++17 -fopenmp

# 或使用脚本自动编译
bash run_experiments.sh
```

## 常见问题

**Q: 为什么Ex0-Alpha的结果和论文图表不完全一致？**
A: 这通常是由于随机数生成器或图拓扑的细微差异导致。确保使用原始的图数据文件。

**Q: 程序运行很慢，如何加速？**
A: 增加 OMP_NUM_THREADS 值（不超过CPU核心数）或减少样本数量。

**Q: 如何只运行某个特定的实验？**
A: 直接运行对应的可执行文件，例如 `./decideab.exe` 或 `./simulations_supplement.exe alpha_sweep`

## 详细文档

- 参数说明：见 `energy/README.md`
- 算法伪代码：见 `docs/` 目录
- 数据格式：见 `tables/README.md`

## 论文引用

如果使用本项目，请引用对应的学术论文。详见项目文档。

## 许可证

研究用途使用
