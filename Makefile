raytrace:
	$(CC) raytrace.c vector.c color.c image.c -o raytrace -lm -lpng -std=c99
