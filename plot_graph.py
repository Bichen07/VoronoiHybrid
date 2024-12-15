import matplotlib.pyplot as plt
import _voronoi_fortune
import _voronoi_halfplane
from scipy.spatial import Voronoi, voronoi_plot_2d
import numpy as np
import os

def generate_points(num_points):
    points = np.random.rand(num_points, 2)  # Random points in 2D space
    return points

def plot_voronoi(points, ax=None):
    """Plot Voronoi diagram using SciPy."""
    vor = Voronoi(points)
    if ax is None:
        fig, ax = plt.subplots(figsize=(10, 8))  # Set larger frame size
    voronoi_plot_2d(vor, ax=ax, show_vertices=False, line_colors='blue', line_width=1, line_alpha=0.6)

    # Plot the input points
    points_array = np.array(points)
    ax.plot(points_array[:, 0], points_array[:, 1], 'o', color='red')

    ax.set_xlim(0, 1)
    ax.set_ylim(0, 1)
    ax.set_aspect('equal', adjustable='box')
    ax.set_title("SciPy Voronoi Diagram", fontsize=14)

def plot_voronoi_halfplane(points, ax=None):
    edges = _voronoi_halfplane.compute_voronoi_halfplane(points)

    # Plot edges
    if ax is None:
        fig, ax = plt.subplots(figsize=(10, 8))  # Set larger frame size
    for (x1, y1), (x2, y2) in edges:
        ax.plot([x1, x2], [y1, y2], color="blue", alpha=0.6, linewidth=1)

    # Plot the input points
    points_array = np.array(points)
    ax.plot(points_array[:, 0], points_array[:, 1], 'o', color='red')

    ax.set_xlim(0, 1)
    ax.set_ylim(0, 1)
    ax.set_aspect('equal', adjustable='box')
    ax.set_title("Voronoi Diagram (Half-Plane Intersection)", fontsize=14)

def plot_voronoi_fortune(points, ax=None):
    """Plot Voronoi diagram using the C++ implementation."""
    edges = _voronoi_fortune.compute_voronoi_fortune(points)
    if ax is None:
        fig, ax = plt.subplots(figsize=(10, 8))  # Set larger frame size
    for (x1, y1), (x2, y2) in edges:
        ax.plot([x1, x2], [y1, y2], color="blue", alpha=0.6, linewidth=1)

    # Plot the input points
    points_array = np.array(points)
    ax.plot(points_array[:, 0], points_array[:, 1], 'o', color='red')

    ax.set_xlim(0, 1)
    ax.set_ylim(0, 1)
    ax.set_aspect('equal', adjustable='box')
    ax.set_title("Voronoi Diagram (fortune)", fontsize=14)

if __name__ == '__main__':
    # Generate random points
    points_list = [10, 20, 40, 80, 160, 300]  # Number of points for the Voronoi diagram
    # points_list = [200]  # Number of points for the Voronoi diagram
    for num_points in points_list:
        points = generate_points(num_points)  # More points for a better diagram

        # Create a single figure with two subplots
        fig, axs = plt.subplots(1, 3, figsize=(18, 6))  # Larger combined frame

        # Plot SciPy Voronoi on the left
        plot_voronoi(points, ax=axs[0])

        # Plot SciPy Voronoi on the middle
        plot_voronoi_fortune(points, ax=axs[1])

        # Plot C++ Voronoi on the right
        plot_voronoi_halfplane(points, ax=axs[2])

        plt.tight_layout() # Adjust subplots to fit into the figure area

        # Save plot
        folder = 'img_result'
        if not os.path.exists(folder):
            os.makedirs(folder)

        filename = f'{folder}/voronoi_plot_num={num_points}.png'
        plt.savefig(filename)
        # # Show the combined plot
        # plt.show()