#pragma once

struct point {
  int x;
  int y;
  double h;
};

struct vertex {
  float x, y, z;
};

// It's also very confusing that triangle has an array of ints named 'points' but
// there is also a struct named point
struct triangle {
  int points[3];
};

struct mesh {
  std::vector<triangle> triangles;
  std::vector<vertex> vertices;
};

// My terminology is really dumb
// is it confusing that we have points and vertices? (I think it is)
struct projectedMesh {
  std::vector<triangle> triangles;
  std::vector<point> points;
};