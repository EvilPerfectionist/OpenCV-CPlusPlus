#include <iostream>
int main() {

	float n[5] = { 20.4 }; /* declaring n as an array of 5 floats */
	float *p; 		/* p as a pointer to float */
	int i;
	p = n; 		/* p now points to array n */
				/* printing the values of elements of array */
	for (i = 0; i < 5; i++)
	{
		std::cout << "*(p + " << i << ") = " << *(p + i) << std::endl;/* *(p+i) means value at (p+0),(p+1)...*/
	}

	return 0;
}