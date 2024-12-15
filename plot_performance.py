import matplotlib.pyplot as plt
import os

def read_performance_file(filename):
    """Reads the performance data from the given file and returns parsed data."""
    num_points = []
    scipy_times = []
    c_fortune_times = []
    c_halfplane_times = []
    
    with open(filename, 'r') as f:
        lines = f.readlines()
        i = 0
        while i < len(lines):
            try:
                num_points.append(int(lines[i].split(":")[1].strip()))
                scipy_times.append(float(lines[i + 1].split(":")[1].strip().split()[0]))
                c_fortune_times.append(float(lines[i + 2].split(":")[1].strip().split()[0]))
                
                # Check if "C halfplane method" exists; if not, append None
                if "C halfplane method" in lines[i + 3]:
                    c_halfplane_times.append(float(lines[i + 3].split(":")[1].strip().split()[0]))
                    i += 6  # Skip 6 lines for the next block
                else:
                    c_halfplane_times.append(None)  # No halfplane method for this block
                    i += 5  # Skip 5 lines for the next block
            except (ValueError, IndexError):
                print(f"Skipping malformed block starting at line {i}")
                i += 1  # Move to the next line in case of error

    return num_points, scipy_times, c_fortune_times, c_halfplane_times


def plot_performance(x, y_scipy, y_fortune, y_halfplane, log_scale=False):
    """Plots the performance data, with an optional log scale."""
    plt.figure(figsize=(10, 6))
    
    if log_scale:
        plt.xscale('log')
        plt.yscale('log')
        plt.title('Performance Comparison (Log Scale)')
    else:
        plt.title('Performance Comparison (Original Scale)')
    
    plt.plot(x, y_scipy, label='SciPy', marker='o', linestyle='-')
    plt.plot(x, y_fortune, label='C Fortune', marker='s', linestyle='--')
    plt.plot(x, y_halfplane, label='C Halfplane', marker='^', linestyle=':')
    
    plt.xlabel('Number of Points')
    plt.ylabel('Time (seconds)')
    plt.legend()
    plt.grid(which="both" if log_scale else "major", linestyle='--', linewidth=0.5)
    plt.tight_layout()

    # Save plot
    folder = 'img_result'
    if not os.path.exists(folder):
        os.makedirs(folder)

    filename = f'{folder}/performance_plot_{"log" if log_scale else "original"}.png'
    plt.savefig(filename)
    plt.show()

if __name__ == '__main__':
    # Read data from the performance file
    filename = 'performance.txt'
    num_points, scipy_times, c_fortune_times, c_halfplane_times = read_performance_file(filename)
    
    # Plot the data (original scale and log scale)
    plot_performance(num_points, scipy_times, c_fortune_times, c_halfplane_times, log_scale=False)
    plot_performance(num_points, scipy_times, c_fortune_times, c_halfplane_times, log_scale=True)
