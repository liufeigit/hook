# hook
====

macOS and iOS hook (mac和ios平台hook)

hook的原理就是基于`Mach_O`文件格式修改stub的相应的跳转表

## api原型

> `int fastHook(char *name,void *fn);`
> 参数说明 `name`:要hook函数名称字符串;`fn`:新的替换方法

## example

> hook `math`库中的abs方法
> 编译运行 `clang -g fastHook/fastHook.c example/test2.c test2`









