## 0. Common Settings (Reproduce Experiment Baseline)

* Topology: **100 random seeds** per configuration; nodes uniformly independently distributed. Fixed set of `seeds.txt` shared by all algorithms.
* Two density scenarios:
  * **Same-domain increasing density**: 500×500 m², N∈{50,100,150,200,250,300}.
  * **Equal-density expanding domain**: Fixed expected degree (proportionally expand domain size based on r).
* Communication radius: r∈{75,100,125} m (default 100).
* α scanning: α∈{0,0.1,…,1.2} (default 0.7, retain out-of-range points to show degradation).
* Energy consumption model:
  * Fixed: Mod-F;
  * Stochastic: **Exp** (baseline), **Gamma(k∈{0.5,1,2,5})**, **Lognormal(σ∈{0.25,0.5,1.0})**, **Weibull(k∈{0.8,1,2})**, **Truncated uniform**; all do **E[c]=1 alignment**, use variance variation to verify robustness.
* Unified metrics:
  * **Network Lifetime** (frame count until first node runs out of energy);
  * **CDS Size**: |C_t| and mean±95% CI;
  * **Stability**: J_t=|C_t∩C_{t-1}|/|C_t∪C_{t-1}|, τ_t=|C_t△C_{t-1}|/|C_{t-1}|, update frequency and node entry/exit count CDF;
  * **Latency Proxy**: average hops to sink; VB path vs shortest path **stretch**;
  * **Energy Balance**: Var/Std/CV, Gini, P90–P10;
  * **Control Overhead**: control packet count/bytes per frame and proportion of energy consumption;
  * **Runtime**: network building time per frame (ms).
* Statistics and presentation: mean±95% CI; key comparisons annotated with U-test p-value.

---

## Ex-Large (⭐⭐⭐⭐⭐, 5–8h)

**Objective**: Respond to R4's scalability and dense network performance.
**Factors/Levels**: N∈{50,100,150,200,250,300} × r∈{75,100,125}; run both density scenarios.
**Output**:

* Fig-L1: Network lifetime vs N (two scenarios side by side).
* Fig-L2: Network building runtime vs N (algorithm scalability).
* Fig-L3: Control overhead proportion vs N (box plots).
* Fig-L4: CDS size and stability (Jaccard mean) vs N dual-axis plot.
  **Success Criterion**: At N≥200, method lifetime and stability not inferior to baseline, runtime growth sub-linear/linear acceptable; control overhead <~5–10%.

---

## Ex-CDS-Stability (⭐⭐⭐⭐⭐, 3–5h)

**Objective**: Directly and quantitatively provide "rotation stability" (R2).
**Computation**: Export C_t frame by frame, compute J_t, τ_t, and overall stability S=1/(T-1)∑_{t>1}J_t.
**Output**:

* Fig-S1: J_t and τ_t over time (mean±CI).
* Fig-S2: S vs N and α heatmap.
* Fig-S3: Node entry/exit count CDF.
  **Success Criterion**: At medium-high density or large-scale, method's S significantly higher than baseline, τ significantly lower.

---

## Ex-Alpha-Sweep (⭐⭐⭐⭐, 10–15h)

**Objective**: Systematically characterize α's multi-objective trade-off on "lifetime–CDS–stability–latency–balance" and show out-of-range degradation.
**Factors/Levels**: α∈{0,0.1,…,1.2}, N∈{100,200}, Mod-F and Mod-R(Exp).
**Output**:

* Fig-A1: Five-panel plot (lifetime, CDS, stability, latency, balance) vs α.
* Fig-A2: CDS size vs latency scatter + linear regression (show trade-off).
  **Success Criterion**: Provide "recommended α" interval (e.g., 0.4–0.9) and evidence of **out-of-range degradation** (α<0.1: CDS too large; α>1: coverage insufficient/performance drop).

---

## Ex-Energy-Model (⭐⭐⭐, 2–3h)

**Objective**: Verify the reasonableness and robustness of stochastic energy consumption model selection (R3).
**Factors/Levels**: Distribution∈{Exp, Gamma, Lognormal, Weibull, Uniform-trunc}, variance from low to high (mean fixed at 1).
**Output**:

* Fig-E1: Box plots of lifetime and stability across distributions×variances.
* Fig-E2: Energy balance metrics (CV/Gini) comparison across distributions as radar plot or table.
  **Success Criterion**: Conclusions remain consistent across distribution families or differences are explainable (e.g., heavier tails → larger lifetime variance but stable ordering).

---

## Low-Cost Gains (Can be incorporated into above experiments)

* **Control Overhead Measurement**: Provide curves of "control energy/total energy ratio" and "control packet count/bytes".
* **Runtime/Complexity**: Curve of network building time per frame vs N, with complexity discussion.

---

## Code Changes (Corresponding to Section 6)

**C++**

* `collect_cds_members(t)`: Output C_t (node ID list) frame by frame.
* `log_control_packet(bytes,type,t)`: Accumulate control packet statistics.
* `measure_build_time(t)`: Network building latency.
* Energy cost model interface: `sample_energy_cost(model, params, rng)`, unified E[c]=1.

**Python**

* `analyze_stability.py`: Read C_t sequence → (J_t, τ_t, S) and CDF.
* `analyze_latency.py`: Sample source-sink pairs, compute VB path hops and stretch.
* `analyze_energy_balance.py`: CV/Gini/P90–P10.
* `plot_suite.py`: Generate full Fig-L/S/A/E suite; `--seeds seeds.txt`.

**Directory/Artifacts**

```
results/
  N{}/r{}/alpha{}/model{}/seed{}/
    cds_t.csv   # CDS members per frame
    ctrl.csv    # Control packet count/bytes
    life.json   # Lifetime/network building latency summary
    latency.csv # Hops/stretch
figs/{L,S,A,E}/*.pdf
tables/*.csv
```

---

## Writing Anchors (Can reference when replying to reviews)

* **R4 (Scalability)**: Fig-L1/L2/L3/L4 (N up to 300; two density scenarios).
* **R2 (CDS Change Metrics)**: Fig-S1/S2/S3 + formula definitions.
* **R1 (Latency/Energy Variance)**: Fig-A2 (CDS-latency trade-off), energy balance metrics.
* **R3 (Model Reasonableness)**: Fig-E1/E2 (distribution and variance sensitivity).
