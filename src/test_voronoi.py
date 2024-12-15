import numpy as np
from scipy.spatial import Voronoi
import _voronoi_fortune
import _voronoi_halfplane


def generate_test_points(num_points):
    return np.random.rand(num_points, 2)

def normalize_vertices(vertices, tolerance=1e-3):
    """Merge vertices that are within a certain tolerance of each other."""
    unique_vertices = []
    for v in vertices:
        if not any(np.allclose(v, uv, atol=tolerance) for uv in unique_vertices):
            unique_vertices.append(v)
    return unique_vertices

def extract_edges_from_scipy(voronoi):
    """Extract edges as pairs of points from a SciPy Voronoi diagram."""
    edges = []
    for ridge in voronoi.ridge_vertices:
        if -1 not in ridge:  # Ignore ridges extending to infinity
            edge = tuple(sorted((tuple(voronoi.vertices[ridge[0]]), tuple(voronoi.vertices[ridge[1]]))))
            edges.append(edge)
    return edges


def compare_edges_with_vertices(scipy_edges, other_edges, tolerance=1e-2):
    """
    Compare edges from two Voronoi diagrams by focusing on vertices.
    Parameters:
        scipy_edges: List of edges from SciPy Voronoi.
        other_edges: List of edges from another implementation.
        tolerance: Maximum allowed deviation for floating-point comparisons.
    Returns:
        bool: True if the diagrams match within tolerance, False otherwise.
    """
    # Flatten edges into a set of unique vertices
    scipy_vertices = set(tuple(np.round(v, decimals=3)) for edge in scipy_edges for v in edge)
    other_vertices = set(tuple(np.round(v, decimals=3)) for edge in other_edges for v in edge)

    # Check if all vertices in SciPy exist in the other implementation
    missing_vertices = [v for v in scipy_vertices if not any(np.allclose(v, ov, atol=tolerance) for ov in other_vertices)]
    
    if missing_vertices:
        print(f"Missing vertices in other implementation: {missing_vertices}")
        return False
    
    print("Vertices match within tolerance.")
    return True

def test_10points_fortune_edges_with_scipy():
    """Test SciPy Voronoi against Fortune's algorithm with vertex-based comparison."""
    points = generate_test_points(10)
    scipy_voronoi = Voronoi(points)
    scipy_edges = extract_edges_from_scipy(scipy_voronoi)
    fortune_edges = _voronoi_fortune.compute_voronoi_fortune(points)

    assert compare_edges_with_vertices(scipy_edges, fortune_edges), "Mismatch in Fortune's algorithm!"
    print("fortune implementations agree!")

# def test_halfplane_edges_with_scipy():
#     """Test SciPy Voronoi against Half-Plane Intersection with vertex-based comparison."""
#     points = generate_test_points(10)
#     scipy_voronoi = Voronoi(points)
#     scipy_edges = extract_edges_from_scipy(scipy_voronoi)
#     halfplane_edges = _voronoi_halfplane.compute_voronoi_halfplane(points)

#     assert compare_edges_with_vertices(scipy_edges, halfplane_edges), "Mismatch in Half-Plane Intersection!"
#     print("halfplane implementations agree!")

def test_50points_fortune_edges_with_scipy():
    """Test SciPy Voronoi against Fortune's algorithm with vertex-based comparison."""
    points = generate_test_points(50)
    scipy_voronoi = Voronoi(points)
    scipy_edges = extract_edges_from_scipy(scipy_voronoi)
    fortune_edges = _voronoi_fortune.compute_voronoi_fortune(points)

    assert compare_edges_with_vertices(scipy_edges, fortune_edges), "Mismatch in Fortune's algorithm!"
    print("fortune implementations agree!")