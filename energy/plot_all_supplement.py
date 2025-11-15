#!/usr/bin/env python3
"""
Unified plotting script for all supplement experiments
Handles: alpha_sweep, large, stability, energy_model
Updated to match actual file formats and export CSV data
"""

import argparse
import sys
import csv
from pathlib import Path
import matplotlib.pyplot as plt
import numpy as np

script_dir = Path(__file__).parent.resolve()
fig_dir = script_dir.parent / "figures" / "supplement"
table_dir = script_dir.parent / "tables"

# Create directories if they don't exist
fig_dir.mkdir(parents=True, exist_ok=True)
table_dir.mkdir(parents=True, exist_ok=True)

# ============================================================================
# Plotting function: Ex-Alpha-Sweep
# ============================================================================
def plot_alpha_sweep():
    """Plot Ex-Alpha-Sweep results"""
    print("\n========== Plotting Ex-Alpha-Sweep ==========")

    output_dir = script_dir / "output" / "ex_alpha_sweep"
    if not output_dir.exists():
        print(f"✗ Results directory not found: {output_dir}")
        return False

    try:
        import os
        result_files = sorted([f for f in os.listdir(output_dir) if f.endswith('.txt')])

        if not result_files:
            print(f"✗ No result files found")
            return False

        N_values = [50, 100, 150]
        r_values = [75, 100, 125]

        # Collect data for CSV export
        csv_data_lifetime = []
        csv_data_cds = []

        # Plot 1: Lifetime vs Alpha for each N
        fig, axes = plt.subplots(1, 3, figsize=(15, 5))
        fig.suptitle('Ex-Alpha-Sweep: Network Lifetime vs Alpha', fontsize=14, fontweight='bold')

        for n_idx, N in enumerate(N_values):
            ax = axes[n_idx]

            for r in r_values:
                result_file = output_dir / f"alpha_sweep_N{N}_r{r}.txt"
                if not result_file.exists():
                    continue

                alpha_vals = []
                lifetime_vals = []
                with open(result_file, 'r') as f:
                    for line in f:
                        line = line.strip()
                        if not line or line.startswith('#'):
                            continue
                        parts = line.split()
                        if len(parts) >= 2:
                            try:
                                alpha_vals.append(float(parts[0]))
                                lifetime_vals.append(float(parts[1]))
                            except ValueError:
                                continue

                if alpha_vals:
                    ax.plot(alpha_vals, lifetime_vals, marker='o', linewidth=2, label=f'r={r}', markersize=6)
                    # Collect CSV data
                    for a, l in zip(alpha_vals, lifetime_vals):
                        csv_data_lifetime.append([N, r, f"{a:.1f}", f"{l:.2f}"])

            ax.set_xlabel('Alpha (α)', fontsize=11)
            ax.set_ylabel('Average Lifetime (frames)', fontsize=11)
            ax.set_title(f'N={N} nodes', fontsize=12, fontweight='bold')
            ax.legend(fontsize=10)
            ax.grid(True, alpha=0.3, linestyle='--')
            ax.set_xlim(-0.05, 1.25)

        plt.tight_layout()
        save_path = fig_dir / "alpha_sweep_lifetime.png"
        save_path.parent.mkdir(parents=True, exist_ok=True)
        plt.savefig(save_path, dpi=150, bbox_inches='tight')
        print(f"✓ Saved: {save_path}")
        plt.close()

        # Plot 2: CDS size vs Alpha
        fig, axes = plt.subplots(1, 3, figsize=(15, 5))
        fig.suptitle('Ex-Alpha-Sweep: CDS Size vs Alpha', fontsize=14, fontweight='bold')

        for n_idx, N in enumerate(N_values):
            ax = axes[n_idx]

            for r in r_values:
                result_file = output_dir / f"alpha_sweep_N{N}_r{r}.txt"
                if not result_file.exists():
                    continue

                alpha_vals = []
                cds_vals = []
                with open(result_file, 'r') as f:
                    for line in f:
                        line = line.strip()
                        if not line or line.startswith('#'):
                            continue
                        parts = line.split()
                        if len(parts) >= 3:
                            try:
                                alpha_vals.append(float(parts[0]))
                                cds_vals.append(float(parts[2]))
                            except ValueError:
                                continue

                if alpha_vals:
                    ax.plot(alpha_vals, cds_vals, marker='s', linewidth=2, label=f'r={r}', markersize=6)
                    # Collect CSV data
                    for a, c in zip(alpha_vals, cds_vals):
                        csv_data_cds.append([N, r, f"{a:.1f}", f"{c:.2f}"])

            ax.set_xlabel('Alpha (α)', fontsize=11)
            ax.set_ylabel('Average CDS Size', fontsize=11)
            ax.set_title(f'N={N} nodes', fontsize=12, fontweight='bold')
            ax.legend(fontsize=10)
            ax.grid(True, alpha=0.3, linestyle='--')
            ax.set_xlim(-0.05, 1.25)

        plt.tight_layout()
        save_path = fig_dir / "alpha_sweep_cds_size.png"
        plt.savefig(save_path, dpi=150, bbox_inches='tight')
        print(f"✓ Saved: {save_path}")
        plt.close()

        # Export CSV
        csv_path = table_dir / "alpha_sweep_lifetime.csv"
        with open(csv_path, 'w', newline='') as f:
            writer = csv.writer(f)
            writer.writerow(['N', 'radius(m)', 'alpha', 'lifetime(frames)'])
            writer.writerows(csv_data_lifetime)
        print(f"✓ Saved: {csv_path}")

        csv_path = table_dir / "alpha_sweep_cds_size.csv"
        with open(csv_path, 'w', newline='') as f:
            writer = csv.writer(f)
            writer.writerow(['N', 'radius(m)', 'alpha', 'cds_size'])
            writer.writerows(csv_data_cds)
        print(f"✓ Saved: {csv_path}")

        print(f"✓ Alpha-Sweep plotting completed")
        return True
    except Exception as e:
        print(f"✗ Plotting error: {e}")
        import traceback
        traceback.print_exc()
        return False

# ============================================================================
# Plotting function: Ex-Large
# ============================================================================
def plot_large():
    """Plot Ex-Large results"""
    print("\n========== Plotting Ex-Large ==========")

    output_dir = script_dir / "output" / "ex_large"
    if not output_dir.exists():
        print(f"✗ Results directory not found: {output_dir}")
        return False

    try:
        import os
        result_files = sorted([f for f in os.listdir(output_dir) if f.endswith('.txt')])

        if not result_files:
            print(f"✗ No result files found")
            return False

        N_values = [50, 100, 150, 200, 250, 300]
        r_values = [75, 100, 125]

        csv_data = []

        fig, axes = plt.subplots(1, 3, figsize=(15, 5))
        fig.suptitle('Ex-Large: Scalability Analysis', fontsize=14, fontweight='bold')

        for r_idx, r in enumerate(r_values):
            ax = axes[r_idx]
            lifetime_data = []
            cds_data = []
            valid_N = []

            for N in N_values:
                result_file = output_dir / f"large_N{N}_r{r}.txt"
                if not result_file.exists():
                    continue

                avg_lifetime = 0.0
                avg_cds = 0.0
                avg_build_time = 0.0
                count = 0

                with open(result_file, 'r') as f:
                    for line in f:
                        line = line.strip()
                        if not line or line.startswith('#'):
                            continue
                        parts = line.split()
                        if len(parts) >= 2:
                            try:
                                avg_lifetime += float(parts[0])
                                avg_cds += float(parts[1])
                                if len(parts) >= 3:
                                    avg_build_time += float(parts[2])
                                count += 1
                            except ValueError:
                                continue

                if count > 0:
                    lifetime_data.append(avg_lifetime / count)
                    cds_data.append(avg_cds / count)
                    valid_N.append(N)
                    csv_data.append([N, r, f"{avg_lifetime/count:.2f}", f"{avg_cds/count:.2f}", f"{avg_build_time/count:.2f}"])

            if valid_N:
                ax2 = ax.twinx()
                line1 = ax.plot(valid_N, lifetime_data, marker='o', linewidth=2.5, label='Lifetime', color='#1f77b4', markersize=8)
                line2 = ax2.plot(valid_N, cds_data, marker='s', linewidth=2.5, label='CDS Size', color='#ff7f0e', markersize=8)

                ax.set_xlabel('Network Size (N nodes)', fontsize=11)
                ax.set_ylabel('Average Lifetime (frames)', fontsize=11, color='#1f77b4')
                ax2.set_ylabel('Average CDS Size', fontsize=11, color='#ff7f0e')
                ax.tick_params(axis='y', labelcolor='#1f77b4')
                ax2.tick_params(axis='y', labelcolor='#ff7f0e')
                ax.set_title(f'r={r}m', fontsize=12, fontweight='bold')
                ax.grid(True, alpha=0.3, linestyle='--')
                ax.set_xticks(valid_N)

                lines = line1 + line2
                labels = [l.get_label() for l in lines]
                ax.legend(lines, labels, loc='upper left', fontsize=10)

        plt.tight_layout()
        save_path = fig_dir / "large_scalability.png"
        plt.savefig(save_path, dpi=150, bbox_inches='tight')
        print(f"✓ Saved: {save_path}")
        plt.close()

        # Export CSV
        csv_path = table_dir / "large_scalability.csv"
        with open(csv_path, 'w', newline='') as f:
            writer = csv.writer(f)
            writer.writerow(['N', 'radius(m)', 'lifetime(frames)', 'cds_size', 'build_time(ms)'])
            writer.writerows(csv_data)
        print(f"✓ Saved: {csv_path}")

        print(f"✓ Large plotting completed")
        return True
    except Exception as e:
        print(f"✗ Plotting error: {e}")
        import traceback
        traceback.print_exc()
        return False

# ============================================================================
# Plotting function: Ex-CDS-Stability
# ============================================================================
def plot_cds_stability():
    """Plot Ex-CDS-Stability results"""
    print("\n========== Plotting Ex-CDS-Stability ==========")

    output_dir = script_dir / "output" / "ex_cds_stability"
    if not output_dir.exists():
        print(f"✗ Results directory not found: {output_dir}")
        return False

    try:
        import os
        result_files = sorted([f for f in os.listdir(output_dir) if f.endswith('.txt')])

        if not result_files:
            print(f"✗ No result files found")
            return False

        N_values = [50, 100, 150]
        r_values = [100, 125]
        alpha_values = [0.3, 0.5, 0.7]

        csv_data = []

        fig, axes = plt.subplots(1, 2, figsize=(14, 5))
        fig.suptitle('Ex-CDS-Stability: Stability Metrics', fontsize=14, fontweight='bold')

        for alpha in alpha_values:
            jaccard_data = []
            change_rate_data = []
            valid_N = []

            for N in N_values:
                # Try r=100 first
                result_file = output_dir / f"stability_N{N}_r100_alpha{int(alpha*10)}.txt"
                if not result_file.exists():
                    # Try alternate file naming
                    result_file = output_dir / f"stability_N{N}_r125_alpha{int(alpha*10)}.txt"

                if not result_file.exists():
                    continue

                with open(result_file, 'r') as f:
                    for line in f:
                        line = line.strip()
                        if not line or line.startswith('#'):
                            continue
                        parts = line.split()
                        if len(parts) >= 2:
                            try:
                                J = float(parts[0])
                                Tau = float(parts[1])
                                jaccard_data.append(J)
                                change_rate_data.append(Tau)
                                if N not in valid_N:
                                    valid_N.append(N)
                                csv_data.append([N, alpha, f"{J:.4f}", f"{Tau:.4f}"])
                            except ValueError:
                                continue

            if valid_N and jaccard_data:
                axes[0].plot(valid_N, jaccard_data[:len(valid_N)], marker='o', linewidth=2.5, label=f'α={alpha}', markersize=8)
                axes[1].plot(valid_N, change_rate_data[:len(valid_N)], marker='s', linewidth=2.5, label=f'α={alpha}', markersize=8)

        axes[0].set_xlabel('Network Size (N nodes)', fontsize=11)
        axes[0].set_ylabel('Jaccard Similarity (J_t)', fontsize=11)
        axes[0].set_title('CDS Jaccard Similarity', fontsize=12, fontweight='bold')
        axes[0].legend(fontsize=10)
        axes[0].grid(True, alpha=0.3, linestyle='--')
        axes[0].set_ylim([0, 1.05])

        axes[1].set_xlabel('Network Size (N nodes)', fontsize=11)
        axes[1].set_ylabel('Member Change Rate (τ_t)', fontsize=11)
        axes[1].set_title('CDS Member Change Rate', fontsize=12, fontweight='bold')
        axes[1].legend(fontsize=10)
        axes[1].grid(True, alpha=0.3, linestyle='--')
        axes[1].set_ylim([0, 1.05])

        plt.tight_layout()
        save_path = fig_dir / "stability_metrics.png"
        plt.savefig(save_path, dpi=150, bbox_inches='tight')
        print(f"✓ Saved: {save_path}")
        plt.close()

        # Export CSV
        csv_path = table_dir / "stability_metrics.csv"
        with open(csv_path, 'w', newline='') as f:
            writer = csv.writer(f)
            writer.writerow(['N', 'alpha', 'jaccard_similarity', 'member_change_rate'])
            writer.writerows(csv_data)
        print(f"✓ Saved: {csv_path}")

        print(f"✓ Stability plotting completed")
        return True
    except Exception as e:
        print(f"✗ Plotting error: {e}")
        import traceback
        traceback.print_exc()
        return False

# ============================================================================
# Plotting function: Ex-Energy-Model
# ============================================================================
def plot_energy_model():
    """Plot Ex-Energy-Model results"""
    print("\n========== Plotting Ex-Energy-Model ==========")

    output_dir = script_dir / "output" / "ex_energy_model"
    if not output_dir.exists():
        print(f"✗ Results directory not found: {output_dir}")
        return False

    try:
        import os
        result_files = sorted([f for f in os.listdir(output_dir) if f.endswith('.txt')])

        if not result_files:
            print(f"✗ No result files found")
            return False

        models = ["Exp", "Gamma", "Lognormal", "Weibull"]

        fig, axes = plt.subplots(1, 2, figsize=(14, 5))
        fig.suptitle('Ex-Energy-Model: Energy Distribution Comparison (E[c]=1)', fontsize=14, fontweight='bold')

        lifetime_data = []
        cds_data = []
        valid_models = []

        csv_data = []

        for model in models:
            result_file = output_dir / f"energy_model_{model}.txt"
            if not result_file.exists():
                print(f"  Warning: Missing {result_file.name}")
                continue

            lifetimes = []
            cds_sizes = []
            variances = []

            with open(result_file, 'r') as f:
                for line in f:
                    line = line.strip()
                    if not line or line.startswith('#'):
                        continue
                    parts = line.split()
                    if len(parts) >= 3:
                        try:
                            lifetimes.append(float(parts[0]))
                            cds_sizes.append(float(parts[1]))
                            variances.append(float(parts[2]))
                        except ValueError:
                            continue

            if lifetimes:
                avg_lifetime = np.mean(lifetimes)
                avg_cds = np.mean(cds_sizes)
                avg_var = np.mean(variances)
                lifetime_data.append(avg_lifetime)
                cds_data.append(avg_cds)
                valid_models.append(model)
                csv_data.append([model, f"{avg_lifetime:.2f}", f"{avg_cds:.2f}", f"{avg_var:.4f}"])

        if valid_models:
            x_pos = np.arange(len(valid_models))
            width = 0.35

            bars1 = axes[0].bar(x_pos - width/2, lifetime_data, width, label='Lifetime', alpha=0.8, color='#1f77b4')
            bars2 = axes[1].bar(x_pos - width/2, cds_data, width, label='CDS Size', alpha=0.8, color='#ff7f0e')

            axes[0].set_ylabel('Average Lifetime (frames)', fontsize=11)
            axes[0].set_title('Network Lifetime by Energy Model', fontsize=12, fontweight='bold')
            axes[0].set_xticks(x_pos)
            axes[0].set_xticklabels(valid_models)
            axes[0].grid(True, alpha=0.3, axis='y', linestyle='--')

            axes[1].set_ylabel('Average CDS Size', fontsize=11)
            axes[1].set_title('CDS Size by Energy Model', fontsize=12, fontweight='bold')
            axes[1].set_xticks(x_pos)
            axes[1].set_xticklabels(valid_models)
            axes[1].grid(True, alpha=0.3, axis='y', linestyle='--')

        plt.tight_layout()
        save_path = fig_dir / "energy_model_comparison.png"
        plt.savefig(save_path, dpi=150, bbox_inches='tight')
        print(f"✓ Saved: {save_path}")
        plt.close()

        # Export CSV
        csv_path = table_dir / "energy_model_comparison.csv"
        with open(csv_path, 'w', newline='') as f:
            writer = csv.writer(f)
            writer.writerow(['model', 'avg_lifetime(frames)', 'avg_cds_size', 'avg_variance'])
            writer.writerows(csv_data)
        print(f"✓ Saved: {csv_path}")

        print(f"✓ Energy-Model plotting completed")
        return True
    except Exception as e:
        print(f"✗ Plotting error: {e}")
        import traceback
        traceback.print_exc()
        return False

# ============================================================================
# Main function
# ============================================================================
def main():
    parser = argparse.ArgumentParser(
        description='Plot supplement experiment results',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python3 plot_all_supplement.py --experiment alpha_sweep
  python3 plot_all_supplement.py --experiment all
        """)
    parser.add_argument('--experiment',
                       choices=['alpha_sweep', 'large', 'stability', 'energy_model', 'all'],
                       default='all',
                       help='Experiment to plot (default: all)')

    args = parser.parse_args()

    print("\n" + "="*70)
    print(f" Plotting supplement experiment results")
    print(f" Figures: {fig_dir}")
    print(f" Tables:  {table_dir}")
    print("="*70)

    success = True

    if args.experiment in ['alpha_sweep', 'all']:
        if not plot_alpha_sweep():
            success = False

    if args.experiment in ['large', 'all']:
        if not plot_large():
            success = False

    if args.experiment in ['stability', 'all']:
        if not plot_cds_stability():
            success = False

    if args.experiment in ['energy_model', 'all']:
        if not plot_energy_model():
            success = False

    print("\n" + "="*70)
    if success:
        print(f" ✓ Plotting completed successfully")
    else:
        print(f" ✗ Some plotting operations failed - check output above")
    print("="*70 + "\n")

    return 0 if success else 1

if __name__ == "__main__":
    sys.exit(main())
