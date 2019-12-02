1. 为了让编译时支持simd指令，需要添加```-march=native```

   指定march为native时，gcc会根据本地环境上的cpu类型选择对应的体系架构，而且相应的支持的指令集打开；

   之前使用```-march=skylake-avx512```，会有**instruction invalid**的bug;

   在CodeBlocks里面，Settings -> Compiler -> other Compiler options 添加 ```-march=native```

2. 为了让链接器支持多线程需要在Codeblocks的project -> build options里面添加 ```-pthread```
3. 性能提升方面，单纯用多线程在W = 10000, H = 1000, N = 10 时，线程数为8时，提升5-6倍。