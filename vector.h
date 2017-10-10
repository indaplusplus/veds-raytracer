#ifndef VECTOR_H_
#define VECTOR_H_

/*
 * Varius functions for handling vectors (since libc doesn't include any).
 */
typedef struct{
    float x,y,z;
}Vec3d;

float dot_product(Vec3d *v1, Vec3d *v2);
Vec3d mul_vector(Vec3d *v, float c);
Vec3d div_vector(Vec3d *v, float c);
Vec3d add_vectors(Vec3d *v1, Vec3d *v2);
Vec3d sub_vectors(Vec3d *v1, Vec3d *v2);
float vector_to_float(Vec3d *v);
Vec3d get_normal(Vec3d *v1, Vec3d *v2, float c);
Vec3d normalize_vector(Vec3d *v);

#endif
