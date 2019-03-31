#include "fastHook.h"

void format1(){

	void *h=dlopen("libA.dylib", 1);

	void *fn1=dlsym(h, "testA");

	void *h1=dlopen("libA.dylib", 1);

	void *fn2=dlsym(h1, "testA");

	struct mach_header_64 *header=(struct mach_header_64 *)h;
	struct mach_header_64 *header1=_dyld_get_image_header(1); 

	int slide=_dyld_get_image_vmaddr_slide(1);


	printf("libA.dylib is %p\n",h);
	printf("libA.dylib is %p\n",h1);
	dlclose(h);
	dlclose(h1);

	// struct nlist_64 list;
	// nlist("format", &list);

}

void testUpdate(){
	testA();
	RelationTabMeta relationTabMeta;
	getRelationTabMeta(0,&relationTabMeta);

	SymMeta *symMeta;
	symMeta=findSymMeta(0,"testA");

	setLaSymPtr(0, "testA", format1);

	testA();

}

int main(int argc,char **argv){


	testUpdate();

	getchar();

	return 0;
}

