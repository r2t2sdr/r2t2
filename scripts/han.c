#include <math.h>
#include <stdio.h>
int fftSize=1024;

int main() {
printf ("memory_initialization_radix=16;\n");
printf ("memory_initialization_vector=");
	for (int i=0;i<fftSize;i++) {
		printf ("%08x",(int)((0.5  - 0.5  * cos (2 * M_PI / fftSize * i))*((1<<23)-1))) ;
		if (i<fftSize-1)
			printf (",\n");
		else
			printf (";\n");
	}
}

