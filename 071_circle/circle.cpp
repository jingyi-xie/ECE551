#include "circle.h"

#include <cmath>

void Circle::move(double dx, double dy) {
  center.move(dx, dy);
}

double Circle::intersectionArea(const Circle & otherCircle) {
  double r = radius;
  double R = otherCircle.radius;
  double d = center.distanceFrom(otherCircle.center);
  if (R < r) {
    // swap
    r = otherCircle.radius;
    R = radius;
  }
  if (d == 0 || d + r <= R) {
    return M_PI * r * r;
  }
  if (d >= r + R || r == 0) {
    return 0;
  }
  double part1 = r * r * std::acos((d * d + r * r - R * R) / (2 * d * r));
  double part2 = R * R * std::acos((d * d + R * R - r * r) / (2 * d * R));
  double part3 = 0.5 * std::sqrt((-d + r + R) * (d + r - R) * (d - r + R) * (d + r + R));
  return part1 + part2 - part3;
}
