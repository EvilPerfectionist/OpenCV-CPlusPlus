/* sprintf example */
#include <stdio.h>

int main()
{
	char buffer[50];
	int n, a = 5, b = 3;
	n = sprintf_s(buffer, "%02d plus %02d is %02d", a, b, a + b);
	printf("[%s] is a string %d chars long\n", buffer, n);
	return 0;
}