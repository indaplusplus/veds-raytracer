#include "vector.h"
#include "math.h"

float dot_product(Vec3d *v1, Vec3d *v2) {
  return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
}

Vec3d mul_vector(Vec3d *v, float c) {
  Vec3d result = {v->x * c, v->y * c, v->z * c};
  return result;
}

Vec3d div_vector(Vec3d *v, float c) {
  Vec3d result = {v->x / c, v->y / c, v->z / c};
  return result;
}

Vec3d add_vectors(Vec3d *v1, Vec3d *v2) {
  Vec3d result = {v1->x + v2->x, v1->y + v2->y, v1->z + v2->z};
  return result;
}

Vec3d sub_vectors(Vec3d *v1, Vec3d *v2) {
  Vec3d result = {v1->x - v2->x, v1->y - v2->y, v1->z - v2->z};
  return result;
}

float vector_to_float(Vec3d *v) {
  return v->x + v->y + v->z;
}

Vec3d get_normal(Vec3d *v1, Vec3d *v2, float c) {
  Vec3d result = sub_vectors(v1, v2);
  return div_vector(&result, c);
}

Vec3d normalize_vector(Vec3d *v) {
  float a = sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
  Vec3d result = div_vector(v, a);
  return result;
}
