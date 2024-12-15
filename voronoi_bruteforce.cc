#include <vector>
#include <utility>
#include <algorithm>
#include <limits>
#include <cmath>
#include "voronoi_common.hh"


struct Point {
    double x, y;
};

struct Edge {
    Point start, end;
};
// Check if a point lies on the correct side of a line
bool is_inside(const Point& p, const Point& a, const Point& b) {
    return (b.x - a.x) * (p.y - a.y) - (b.y - a.y) * (p.x - a.x) >= 0;
}

// Check if a curr point lies on the correct side of a line
bool is_inside_polygon(const Point& curr, const Point& pj, const Point& p1, const Point& p2) {
    bool pj_side = ((p2.x - p1.x) * (pj.y - p1.y) - (p2.y - p1.y) * (pj.x - p1.x)) >= 0;
    bool curr_side = ((p2.x - p1.x) * (curr.y - p1.y) - (p2.y - p1.y) * (curr.x - p1.x)) >= 0;
    return !(pj_side == curr_side);
}

// Compute intersection of two lines
Point line_intersection(const Point& a, const Point& b, const Point& c, const Point& d) {
    double A1 = b.y - a.y;
    double B1 = a.x - b.x;
    double C1 = A1 * a.x + B1 * a.y;

    double A2 = d.y - c.y;
    double B2 = c.x - d.x;
    double C2 = A2 * c.x + B2 * c.y;

    double det = A1 * B2 - A2 * B1;
    if (std::fabs(det) < 1e-10) {
        return {std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN()};
    }

    return {
        (B2 * C1 - B1 * C2) / det,
        (A1 * C2 - A2 * C1) / det
    };
}

// Compute Voronoi edges using half-plane intersection
std::vector<std::pair<std::pair<double, double>, std::pair<double, double>>> compute_voronoi_halfplane(const std::vector<std::pair<double, double>>& input_points) {
    std::vector<Edge> edges;

    for (size_t i = 0; i < input_points.size(); ++i) {
        const point& pi = {input_points[i].x, input_points[i].y};
        std::vector<Edge> region_edges;

        // Start with a bounding box 1 x 1 size
        // const double box_size = 1.0;
        std::vector<Point> polygon = {
            {X0, Y0}, {X1, Y0}, {X1, Y1}, {X0, Y1}
        };

        for (size_t j = 0; j < input_points.size(); ++j) {
            if (i == j) continue;
            const Point& pj = {input_points[j].x, input_points[j].y};

            // Compute perpendicular bisector of pi and pj
            Point mid = {(pi.x + pj.x) / 2, (pi.y + pj.y) / 2};
            Point dir = {pj.y - pi.y, pi.x - pj.x}; // Perpendicular direction
            Point p1 = {mid.x - dir.x, mid.y - dir.y};
            Point p2 = {mid.x + dir.x, mid.y + dir.y};

            std::vector<Point> new_polygon; // Store the new polygon after cutting with the half-plane
            for (size_t k = 0; k < polygon.size(); ++k) { 
                const Point& curr = polygon[k];
                const Point& next = polygon[(k + 1) % polygon.size()];

                if (is_inside_polygon(curr, pj, p1, p2)) {  // curr is inside the half-plane if curr are on different side with pj base on line p1-p2
                    new_polygon.push_back(curr);
                }

                if (is_inside(curr, p1, p2) != is_inside(next, p1, p2)) {
                    Point intersect = line_intersection(curr, next, p1, p2);
                    new_polygon.push_back(intersect);
                }
            }
            polygon = std::move(new_polygon);
        }

        // Add edges of the resulting polygon to the Voronoi edges
        for (size_t k = 0; k < polygon.size(); ++k) {
            const Point& start = polygon[k];
            const Point& end = polygon[(k + 1) % polygon.size()];
            edges.push_back({start, end});
        }
    }

    // Convert to output format
    std::vector<std::pair<std::pair<double, double>, std::pair<double, double>>> result;
    for (const auto& edge : edges) {
        result.push_back({{edge.start.x, edge.start.y}, {edge.end.x, edge.end.y}});
    }
    return result;
}

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

PYBIND11_MODULE(_voronoi_halfplane, m) {
    m.doc() = "Voronoi diagram computation using Half-Plane Intersection and Fortune's Algorithm";
    m.def("compute_voronoi_halfplane", &compute_voronoi_halfplane, "Compute Voronoi diagram using half-plane intersection");
}
