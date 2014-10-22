hook
====

mac OSX and ios hook (mac和ios平台hook)

1. A.c 编译为libA.dylib 动态库 

   clang -g -shared -fPIC A.c -o libA.dylib; testA为测试被hook的方法

2. hook.c 文件编译
 
   clang -g -framework Foundation -L./ -lA hook.c -o hook

3. 运行 ./hook 

hook 的原理就是修改stub的相应的跳转表
关键方法 为getRelationTabMeta  findSymMeta 和 setLaSymPtr ，由此可以抽象出更为复杂的hook api

setLaSymPtr(0, "testA", format1); // hook libA.dylib 的testA 变为当前的format1 函数指针

然后再执行testA 将会跳转到format1 




