#include <stdio.h>
#include <iostream>
#include <vector>

// Data Structures
struct vector3d {
  float x, y, z;
};

struct triangle {
  vector3d point[3];
};

struct mesh {
  std::vector<triangle> tris; 
};

// Matrices and Projection
void MatrixMultiply();

void Project();

void Rotate();

void Scale();

void Translate();

int main(int argc, char* argv[]) {
  printf("Testing matrices and projection\n");
}
