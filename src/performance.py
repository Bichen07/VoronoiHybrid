import _voronoi_fortune
import _voronoi_halfplane
import timeit
import random
import numpy as np
from scipy.spatial import Voronoi, voronoi_plot_2d
import matplotlib.pyplot as plt

def generate_points(num_points):
    points = np.random.rand(num_points, 2)  # Random points in 2D space
    return points

def benchmark_all():
    num_points_list = [5*4**i for i in range(0, 7)]  # Generate points for 1, 10, 100, 1000, 10000, 100000
    with open('src/performance.txt', 'w') as f:
        for num_points in num_points_list:
            points = generate_points(num_points)

            # Measure time for the _voronoi_fortune C implementation
            time_voronoi_C_fortune = min(timeit.repeat(lambda: _voronoi_fortune.compute_voronoi_fortune(points), number=1, repeat=2))
            if(num_points < 21000):
                # Measure time for the _voronoi_halfplane C implementation
                time_voronoi_C_halfplane = min(timeit.repeat(lambda: _voronoi_halfplane.compute_voronoi_halfplane(points), number=1, repeat=2))

            # Measure time for SciPy's Voronoi
            time_voronoi_scipy = min(timeit.repeat(lambda: Voronoi(points), number=1, repeat=2))

            # Write results to the file
            f.write(f"Number of points: {num_points}\n")
            f.write(f"SciPy method: {time_voronoi_scipy:.6f} seconds\n")
            f.write(f"C Fortune method: {time_voronoi_C_fortune:.6f} seconds\n")
            if(num_points < 21000):
                f.write(f"C halfplane method: {time_voronoi_C_halfplane:.6f} seconds\n")
            f.write(f"==========================\n")
            f.write(f"Speed-up of C Fortune over SciPy: {time_voronoi_scipy / time_voronoi_C_fortune:.2f}x\n")
            if(num_points < 21000):
                f.write(f"Speed-up of C halfplane over SciPy: {time_voronoi_scipy / time_voronoi_C_halfplane:.2f}x\n")
            f.write("\n")

def benchmark_fortune_scipy():
    num_points_list = [5*4**i for i in range(0, 9)]  # Generate points for 1, 10, 100, 1000, 10000, 100000
    with open('src/performance_two.txt', 'w') as f:
        for num_points in num_points_list:
            points = generate_points(num_points)

            # Measure time for the _voronoi_fortune C implementation
            time_voronoi_C_fortune = min(timeit.repeat(lambda: _voronoi_fortune.compute_voronoi_fortune(points), number=1, repeat=2))

            # Measure time for SciPy's Voronoi
            time_voronoi_scipy = min(timeit.repeat(lambda: Voronoi(points), number=1, repeat=2))

            # Write results to the file
            f.write(f"Number of points: {num_points}\n")
            f.write(f"SciPy method: {time_voronoi_scipy:.6f} seconds\n")
            f.write(f"C Fortune method: {time_voronoi_C_fortune:.6f} seconds\n")
            f.write(f"==========================\n")
            f.write(f"Speed-up of C Fortune over SciPy: {time_voronoi_scipy / time_voronoi_C_fortune:.2f}x\n")
            f.write("\n")

if __name__ == '__main__':
    benchmark_all()
    benchmark_fortune_scipy()