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
// TODO: rename points to vertexIndex or something like that.
struct triangle {
  int points[3];
  bool operator== (triangle t1) {
    return (points[0] == t1.points[0]) && (points[1] == t1.points[1]) && (points[2] == t1.points[2]);
  }
};

struct mesh {
  std::vector<triangle> triangles;
  std::vector<vertex> vertices;
};

// My terminology is really dumb
// is it confusing that we have points and vertices? (I think it is)
// TODO: we could probably use this instead of mesh all of the time to keep things simple
struct projectedMesh {
  std::vector<triangle> triangles;
  std::vector<vertex> vertices;
  std::vector<point> points;
};

// For clipping, which is not currently implemented, will get back to it later
struct plane {
  vertex normal;
  int distance;
  plane(vertex n, int d) {
    normal = n;
    distance = d;
  }
};
