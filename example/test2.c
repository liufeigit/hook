#include "../fastHook/fastHook.h"
#include <math.h>

int fn1(int num){
	
	return num+1;
}

int main(int argc,char **argv){

	int num1=10,num2=0;

	num2=abs(num1);
	int (*fn2)(int )=abs;

	printf("orign abs is %d\n",num2);
	fastHook("abs",fn1);
	num2=abs(num1);
	printf("hook abs is %d\n",num2);

	int num3=fn2(num1);
	printf("orign abs is %d\n",num3);


	return 0;
}

