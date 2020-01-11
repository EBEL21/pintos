#include<stdio.h>
#include<syscall.h>
#include<string.h>
int convert(char *str, int* flag);
int main(int argc, char** argv) {
	
	if(argc != 5) {
		printf("usage: sum A B C D\n");
		return EXIT_FAILURE;
	}
	int a=0,b=0,c=0,d=0;
	int flag = 0;
	int fib=0, sum=0;
	a = convert(argv[1], &flag);
	if(!flag) {return EXIT_FAILURE;}
	b = convert(argv[2], &flag);
	if(!flag) {return EXIT_FAILURE;}
	c = convert(argv[3], &flag);
	if(!flag) {return EXIT_FAILURE;}
	d = convert(argv[4], &flag);
	if(!flag) {return EXIT_FAILURE;}
	ASSERT(a>=0);
	fib = fibonacci(a);
	sum = sum_of_four_int(a,b,c,d);
	printf("%d %d\n",fib,sum);
	return EXIT_SUCCESS;
}

int convert(char *str, int* flag) {
	
	int sum = 0;
	int i = 0;
	for(i = 0; i < strlen(str); i++) {
		if(!(str[i] >= '0' && str[i] <='9')) {
			*flag = 0;
			return 0;
		}
		sum = sum*10 + (str[i]-'0');
	}
	*flag = 1;
	return sum;
}
