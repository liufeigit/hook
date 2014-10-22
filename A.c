#include "A.h"


static void _testA(){
	printf("_testA......\n");
}

void testA(){
	_testA();
	printf("testA......\n");
}

void callBack(int num){
	printf("callBack is %d\n",num );
}

A *initA(){

	int size;
	size=sizeof(A);
	A *a1=(A *)malloc(size*1);
	a1->name="asdf";
	a1->num=12;
	a1->callBack=callBack;
	a1->id=(void *)testA; // 测试使用

	return a1;
}

A ATable={130,"table",callBack,testA};
