#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <getopt.h>
#include "vector.h"
#include "color.h"
#include "image.h"

#define MIN(a,b)(a<b) ? a : b;

typedef struct Sphere Sphere;
struct Sphere{
  Vec3d *center;
  float radius;
  float reflection;
  Color *color;
  Sphere* next_sphere;
};

typedef struct{
    Vec3d origin, destination;
}Ray;

typedef struct Light Light;
struct Light {
  Vec3d *pos;
  Color *intensity;
  Light* next_light;
};

int intersect(Sphere *sphere, Ray *ray, float *f) {
  //Ax^2 + Bx + C = 0
  float b, c;
  Vec3d distance_origin_center = sub_vectors(&ray->origin, sphere->center);
  b = 2 * dot_product(&distance_origin_center, &ray->destination);
  c = dot_product(&distance_origin_center, &distance_origin_center) - sphere->radius * sphere->radius;

  float disc = b*b-4*c;
  if (disc > 0) {
    disc = sqrtf(disc);
    float minimum = MIN(((-b + disc) / 2), ((-b - disc) / 2));
    if (minimum > 0.001f && minimum < *f) {
      *f = minimum;
      return 1;
    }
  }
  return 0;
}

Sphere *sphere_head = NULL;
void add_sphere(Sphere *new_sphere) {
  Sphere *current = sphere_head;
  if (sphere_head == NULL) {
      sphere_head = new_sphere;
      return;
  }

  while (current->next_sphere != NULL) {
    current = current->next_sphere;
  }
  current->next_sphere = new_sphere;
  current->next_sphere->next_sphere = NULL;
}

Light *light_head = NULL;
void add_light(Light *new_light) {
  Light *current = light_head;
  if (light_head== NULL) {
      light_head = new_light;
      return;
  }

  while (current->next_light != NULL) {
    current = current->next_light;
  }
  current->next_light = new_light;
  current->next_light->next_light = NULL;
}

void free_spheres() {
  while(sphere_head) {
    Sphere *tmp = sphere_head;
    sphere_head = sphere_head->next_sphere;
    free(tmp);
  }
}

void free_lights() {
  while(light_head) {
    Light *tmp = light_head;
    light_head = light_head->next_light;
    free(tmp);
  }
}

void cleanup(Image *image) {
  free_spheres();
  free_lights();
  destroy_pixels(image);
}

void setup_random_world(Image *image, int amount_of_spheres_and_lights) {
  time_t t;
  srand((unsigned) time(&t));
  for (int i = 0; i < amount_of_spheres_and_lights; i++) {
    ///Sphere
    Vec3d *sphere_center = malloc(sizeof(Vec3d));
    sphere_center->x = (float)(rand() % image->width);
    sphere_center->y = (float)(rand() % image->height);
    sphere_center->z = (float)((rand() % 3000) - 1000);

    Color *color = malloc(sizeof(Color));
    color->red = (float)(rand() % 256); //color between 0 and 255.
    color->green = (float)(rand() % 256);
    color->blue = (float)(rand() % 256);

    Sphere *sphere_one = malloc(sizeof(Sphere));
    sphere_one->color = color;
    sphere_one->center = sphere_center;
    sphere_one->reflection = (float)(60 + rand() % 41) / 100;
    sphere_one->radius = (float)(50+rand() % (2*(image->width + image->height) / (int)sqrtf(image->width + image->height)));
    sphere_one->next_sphere = NULL;
    add_sphere(sphere_one);

    //Light
    Vec3d *light_position = malloc(sizeof(Vec3d));
    light_position->x = (float)(rand() % image->width);
    light_position->y = (float)(rand() % image->height);
    light_position->z = (float)((rand() % 3000) - 1000);

    Color *intensity = malloc(sizeof(Color));
    intensity->red = (float)(50 + rand() % 51) / 100;
    intensity->green = (float)(50 + rand() % 51) / 100;
    intensity->blue = (float)(50 + rand() % 51) / 100;

    Light *light = malloc(sizeof(Light));
    light->pos = light_position;
    light->intensity = intensity;
    add_light(light);
  }
}

void create_image(Image *image) {
  Ray ray;
  for(int x = 0; x < image->width; x++) {
    for(int y = 0; y < image->height; y++) {
      float red = 0;
      float green = 0;
      float blue = 0;
      float coef = 1.0;
      int iteration = 0;

      Vec3d ray_origin = {x, y, -2000};
      ray.origin = ray_origin;
      Vec3d ray_destination = {0,0,1};
      ray.destination = ray_destination;

      do {
        float f = 20000.0f;
        //find the closest intersection;
        Sphere *sphere = sphere_head;
        Sphere *current_sphere = sphere_head;

        while(current_sphere != NULL) {
          if(intersect(current_sphere, &ray, &f)) {
            sphere = current_sphere;
          }
          current_sphere = current_sphere->next_sphere;
        }

        Vec3d arg1 = mul_vector(&ray.destination, f);
        //point of intersect and the normal for the intersect and the sphere.
        Vec3d pi = add_vectors(&ray.origin, &arg1);
        Vec3d n = get_normal(sphere->center, &pi, sphere->radius);

        //loop through all light sources.
        Light *current_light = light_head;
        while(current_light != NULL) {
          Vec3d distance = sub_vectors(&pi, current_light->pos);
          if(dot_product(&n, &distance) <= 0.0f) {
            current_light = current_light->next_light;
            continue;
          }
          distance = normalize_vector(&distance);
          Ray light_ray;
          light_ray.origin = pi;
          light_ray.destination = distance;

          Sphere *some = sphere_head;
          int has_hit_sphere = 0;
          while(some != NULL) {
            if (!intersect(some, &light_ray, &f)) {
              //lambert dot product
              float lambert = dot_product(&light_ray.destination, &n) * coef;
              red += lambert * current_light->intensity->red * sphere->color->red;
              green += lambert * current_light->intensity->green * sphere->color->green;
              blue += lambert * current_light->intensity->blue * sphere->color->blue;
              some = some->next_sphere;
              break;
            }
          }
          current_light = current_light->next_light;
        }
        coef *= sphere->reflection;
        //follow the reflection ray
        float reflection = 2.0f * dot_product(&ray.destination, &n);
        Vec3d tmp = mul_vector(&n,reflection);
        ray.origin = pi;
        ray.destination = sub_vectors(&ray.destination, &tmp);
        iteration++;
      }while((coef > 0.0f) && (iteration < 3)); //three is the magic number.
      image->pixels[(x + y*image->width)*3 + 0] = BOUND(red);
      image->pixels[(x + y*image->width)*3 + 1] = BOUND(green);
      image->pixels[(x + y*image->width)*3 + 2] = BOUND(blue);
    }
  }
}

void usage() {
  printf("raytrace -o output.png -w WIDTH -h HEIGHT -n (number of objects and lights)\n");
}

int main(int argc, char *argv[]) {
  int opt;
  int width = -1;
  int height = -1;
  int number = -1;
  char *output_file_name = NULL;

  while ((opt = getopt (argc, argv, "o:w:n:h:")) != -1) {
    switch (opt) {
      case 'o':
        output_file_name = optarg;
      break;
      case 'w':
        //this also accepts negative numbers... :/
        width = atoi(optarg);
        break;
      case 'h':
        height = atoi(optarg);
        break;
      case 'n':
        number = atoi(optarg);
        break;
      default:
        usage();
        break;
    }
  }

  //there's probably a far better way of checking this.
  if (width == -1 || height == -1 ||
    number == -1 || output_file_name == NULL) {
      usage();
      return 0;
  }

  Image image = {width, height};
  alloc_pixels(&image);
  setup_random_world(&image, number);
  create_image(&image);

  if (!save_as_png(&image, output_file_name)) {
    printf("Failed so save image.\n");
  }
  
  cleanup(&image);
  return 0;
}
