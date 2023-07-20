#include <SDL.h>
#include <vector>
#include "../headers/point.h"
#include "../headers/macros.h"
#include "../headers/rasterization.h"

point ViewportToCanvas(float x, float y) {
  point ret;
  ret.x = (int)(x * WIDTH / VIEWPORT_SIZE);
  ret.y = (int)(y * HEIGHT / VIEWPORT_SIZE);
  ret.h = 1.0f;
  return ret;
}

point Project(vertex v) {
  return ViewportToCanvas(v.x * PROJECTION_PLANE_Z / v.z, v.y * PROJECTION_PLANE_Z / v.z);
}

projectedMesh ProjectMesh(mesh m) {
  projectedMesh ret;
  ret.vertices = m.vertices; // This is probably fine right??? I suck at cpp lol
  for (int i = 0; i < m.vertices.size(); i++) {
    ret.points.push_back(Project(m.vertices[i]));
  }
  // HELP! I am noob, am I assigning copy or reference here, I want copy. 
  ret.triangles = m.triangles;
  return ret;
}

// TODO: weird things happen when this is given negative values
mesh Translate(mesh m, int x, int y, int z) {
  for (int i = 0; i < m.vertices.size(); i++) {
    m.vertices[i].x += x;
    m.vertices[i].y += y;
    m.vertices[i].z += z;
  }
  return m;
}

//mesh Transform() {} // TODO: Implement (I view this one as less important for now)

void DebugDrawMesh(SDL_Renderer* renderer, projectedMesh m, int color[]) {
  for (int i = 0; i < m.triangles.size(); i++) {
    DrawWireframeTriangle(renderer, 
                          m.points[m.triangles[i].points[0]], 
                          m.points[m.triangles[i].points[1]], 
                          m.points[m.triangles[i].points[2]], 
                          color);
  }
}
