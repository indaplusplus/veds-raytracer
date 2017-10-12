FLAGS = -O3 -Wall -std=c99
DEPS = -lm -lpng

raytrace:
	$(CC) raytrace.c vector.c color.c image.c -o raytrace $(FLAGS) $(DEPS)
