#include "voronoi_common.hh"
#include <iostream>
#include <fstream>
#include <queue>
#include <limits>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>
#include <utility>


using namespace std;

priority_queue<point,  vector<point>,  gt> points; // site events
priority_queue<event*, vector<event*>, gt> events; // circle events



void process_point()
{
   // Get the next point from the queue.
   point p = points.top();
   points.pop();

   // Add a new arc to the parabolic front.
   front_insert(p);
}

void process_event()
{
   // Get the next event from the queue.
   event *e = events.top();
   events.pop();

   if (e->valid) {
      // Start a new edge.
      seg *s = new seg(e->p);

      // Remove the associated arc from the front.
      arc *a = e->a;
      if (a->prev) {
         a->prev->next = a->next;
         a->prev->s1 = s;
      }
      if (a->next) {
         a->next->prev = a->prev;
         a->next->s0 = s;
      }

      // Finish the edges before and after a.
      if (a->s0) a->s0->finish(e->p);
      if (a->s1) a->s1->finish(e->p);

      // Recheck circle events on either side of p:
      if (a->prev) check_circle_event(a->prev, e->x);
      if (a->next) check_circle_event(a->next, e->x);
   }
   delete e;
}

void front_insert(point p)
{
   if (!root) {
      root = new arc(p);
      return;
   }

   // Find the current arc(s) at height p.y (if there are any).
   for (arc *i = root; i; i = i->next) {
      point outputPoint, outputpoint2;
      if (intersect(p,i,&outputPoint)) { // checks if the new site p form arc intersects the current arc 𝑖 
      // Stores the intersection point between the new parabola (from p) and the current arc i in z

         // If there is a next arc (i->next), and the new site does not intersect it, duplicate i->next.
         if (i->next && !intersect(p,i->next, &outputpoint2)) { // arc i <> arc i->next
            i->next->prev = new arc(i->p,i,i->next); // arc i < >> new duplicate arc i <> arc i->next
            i->next = i->next->prev; // arc i <> new duplicate arc i<> arc i->next
         }
         // original arc i, arc i->next. turn into arc i, new arc, new duplicate arc i, arc i->next.

         // no need to add new arc of duplicate arc i, if new site p intersects with i->next
         else i->next = new arc(i->p,i); // arc i <> new duplicate arc i << arc i->next
         i->next->s1 = i->s1;

         // Add p between i and new duplicate arc i.
         i->next->prev = new arc(p,i,i->next); // arc i <>> site p <> new duplicate arc i <> arc i->next
         i->next = i->next->prev; // arc i <> site p <> new duplicate arc i <> arc i->next

         i = i->next; // Now i points to the new arc(point p, site event).

         // Add new half-edges connected to i's endpoints.
         i->prev->s1 = i->s0 = new seg(outputPoint);
         i->next->s0 = i->s1 = new seg(outputPoint);

         // Check for new circle events around the new arc:
         check_circle_event(i, p.x);
         check_circle_event(i->prev, p.x);
         check_circle_event(i->next, p.x);

         return;
      }
   }

   // Special case: If p never intersects an arc, append it to the list.
   arc *i;
   for (i = root; i->next; i=i->next) ; // Find the last node.

   i->next = new arc(p,i);
   // Insert segment between p and i
   point start;
   start.x = X0;
   start.y = (i->next->p.y + i->p.y) / 2;
   i->s1 = i->next->s0 = new seg(start);
}

// Look for a new circle event for arc i, and new site p.x = x0
void check_circle_event(arc *i, double x0)
{
   // Invalidate any old event.
   if (i->e && i->e->x != x0)
      i->e->valid = false;
   i->e = NULL;

   if (!i->prev || !i->next)
      return;

   double x;
   point o;

   if (circle(i->prev->p, i->p, i->next->p, &x,&o) && x > x0) {
      // Create new event.
      i->e = new event(x, o, i);
      events.push(i->e);
   }
}

// Find the rightmost point on the circle through a,b,c.
bool circle(point a, point b, point c, double *x, point *o)
{
   // Check that bc is a "right turn" from ab.
   if ((b.x-a.x)*(c.y-a.y) - (c.x-a.x)*(b.y-a.y) > 0)
      return false;

   // Algorithm from O'Rourke 2ed p. 189.
   double A = b.x - a.x,  B = b.y - a.y,
          C = c.x - a.x,  D = c.y - a.y,
          E = A*(a.x+b.x) + B*(a.y+b.y),
          F = C*(a.x+c.x) + D*(a.y+c.y),
          G = 2*(A*(c.y-b.y) - B*(c.x-b.x));

   if (G == 0) return false;  // Points are co-linear.

   // Point o is the center of the circle.
   o->x = (D*E-B*F)/G;
   o->y = (A*F-C*E)/G;

   // o.x plus radius equals max x coordinate.
   *x = o->x + sqrt( pow(a.x - o->x, 2) + pow(a.y - o->y, 2) );
   return true;
}

// Will a new parabola at new site point p intersect with arc i? intersection point stores in res
bool intersect(point p, arc *i, point *res)
{
   if (i->p.x == p.x) return false; // sepcial case: intersection in middle of tow point, and create a horizontal line

   double a,b;
   if (i->prev) // Get the intersection of: i->prev, arc i, new site p
      a = intersection(i->prev->p, i->p, p.x).y;
   if (i->next) // Get the intersection of: arc i, next arc, new site p
      b = intersection(i->p, i->next->p, p.x).y;

   if ((!i->prev || a <= p.y) && (!i->next || p.y <= b)) {
      res->y = p.y;

      // Plug it back into the parabola equation.
      res->x = (i->p.x*i->p.x + (i->p.y-res->y)*(i->p.y-res->y) - p.x*p.x)
                / (2*i->p.x - 2*p.x);

      return true;
   }
   return false;
}

// Where do two parabolas intersect? focus of p0, p1 and new point p.x = l
point intersection(point p0, point p1, double l)
{
   point res, p = p0;
   // same x coordinate, set y to (y1+y0)/2
   if (p0.x == p1.x)
      res.y = (p0.y + p1.y) / 2;
   else if (p1.x == l) // if p1.x == l(p.x, new point x coordinate), set res.y to p1.y
      res.y = p1.y; // why? don't it res.y = (p1.y + p.y) / 2?
   else if (p0.x == l) {
      res.y = p0.y;
      p = p1;
   } else {
      // Use the quadratic formula.
      double z0 = 2*(p0.x - l);
      double z1 = 2*(p1.x - l);

      double a = 1/z0 - 1/z1;
      double b = -2*(p0.y/z0 - p1.y/z1);
      double c = (p0.y*p0.y + p0.x*p0.x - l*l)/z0
               - (p1.y*p1.y + p1.x*p1.x - l*l)/z1;

      res.y = ( -b - sqrt(b*b - 4*a*c) ) / (2*a);
   }
   // Plug back into one of the parabola equations.
   res.x = (p.x*p.x + (p.y-res.y)*(p.y-res.y) - l*l)/(2*p.x-2*l);
   return res;
}

void finish_edges()
{
   // Advance the sweep line so no parabolas can cross the bounding box.
   double l = X1 + (X1-X0) + (Y1-Y0);

   // Extend each remaining segment to the new parabola intersections.
   for (arc *i = root; i->next; i = i->next)
      if (i->s1)
         i->s1->finish(intersection(i->p, i->next->p, l*2));
}

void reset_globals() {
    while (!events.empty()) events.pop();  // Clear event queue
    output.clear();                       // Clear output edges
    root = nullptr;                       // Reset beachline root
}


std::vector<std::pair<std::pair<double, double>, std::pair<double, double>>> compute_voronoi_fortune(const std::vector<std::pair<double, double>>& input_points) {
   // Clear global variables before processing
   points = priority_queue<point, vector<point>, gt>();
   events = priority_queue<event*, vector<event*>, gt>();
   output.clear();
   reset_globals(); // Reset global state before each computation

   // Push input points into the priority queue
   for (const auto& p : input_points) {
      points.push(point{p.x, p.y});
   }

   // Process the points and events
   while (!points.empty())
      if (!events.empty() && events.top()->x <= points.top().x)
            process_event();
      else
            process_point();

   while (!events.empty())
      process_event();

   finish_edges();

    // Convert output to Python-friendly format
   std::vector<std::pair<std::pair<double, double>, std::pair<double, double>>> edges;
   for (auto seg : output) {
      if (seg->done) {
         edges.emplace_back(
            std::make_pair(seg->start.x, seg->start.y),
            std::make_pair(seg->end.x, seg->end.y)
         );
      }
   }

   return edges;
}

namespace py = pybind11;

PYBIND11_MODULE(_voronoi_fortune, m) {
    m.doc() = "Voronoi diagram computation using Fortune's algorithm (C++)";

    m.def("compute_voronoi_fortune", &compute_voronoi_fortune, "Compute Voronoi edges",
          py::arg("input_points"));
}
