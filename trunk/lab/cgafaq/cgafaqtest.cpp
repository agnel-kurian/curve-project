#include "cgafaq.h"
#include <cstdio>
#include <cmath>

#include "gtest/gtest.h"

using namespace cgafaq;

TEST(CgaFaqTest, Intersection) {

  P2<double> a(0.0,0.0), b(100.0,100.0), c(0.0,100.0), d(100.0,0.0);
  double r, s;
  intersect(&r, &s, a, b, c, d);
  
  P2<double> ixn(a.x + (b.x - a.x) * r, a.y + (b.y - a.y) * r);

  double ea, eb, ec;
  line_equation(a, b, &ea, &eb, &ec);
  
  EXPECT_NEAR(point_on_line(ixn, ea, eb, ec), 0.0, 0.001);
  
  line_equation(c, d, &ea, &eb, &ec);
  EXPECT_NEAR(fabs(point_on_line(ixn, ea, eb, ec)), 0.0, 0.001);

}

TEST(CgaFaqTest, Projection){
  P2<double> a(0.0, 0.0), b(100.0, 0.0), c(50.0, 10.0);
  
  double r, s;
  distance_from_line(&r, &s, a, b, c);
  double dx = b.x - a.x;
  double dy = b.y - a.y;

  P2<double> p(a.x + r * dx, a.y + r * dy);
  
  double ea, eb, ec;
  line_equation(a, b, &ea, &eb, &ec);
  
  EXPECT_NEAR(fabs(point_on_line(p, ea, eb, ec)), 0.0, 0.001);
  
  double lab = sqrt(dx * dx + dy * dy);
  double l1 = fabs(lab * s);
  
  double dx1 = (p.x - c.x), dy1 = (p.y - c.y);
  double l2 = sqrt(dx1 * dx1 + dy1 * dy1);
  EXPECT_NEAR(l1, l2, 0.001);
}
