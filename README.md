# multiThreadPratice

题目见statement.pdf



#### 机器CPU信息

```
Architecture:        x86_64
CPU op-mode(s):      32-bit, 64-bit
Byte Order:          Little Endian
CPU(s):              6
On-line CPU(s) list: 0-5
Thread(s) per core:  1
Core(s) per socket:  6
Socket(s):           1
... 
L1d cache:           32K
L1i cache:           32K
L2 cache:            256K
L3 cache:            9216K

```



#### 注意事项

1. 为了让编译时支持simd指令，需要添加```-march=native```

   指定march为native时，gcc会根据本地环境上的cpu类型选择对应的体系架构，而且相应的支持的指令集打开；

   之前使用```-march=skylake-avx512```，会有**instruction invalid**的bug;

   在CodeBlocks里面，Settings -> Compiler -> other Compiler options 添加 ```-march=native```

2. 为了让链接器支持多线程需要在Codeblocks的project -> build options里面添加 ```-pthread```

3. 性能提升方面，单纯用多线程在W = 10000, H = 1000, N = 10 时，线程数为8时，提升5-6倍。

4. 关于线程数量，应该使用 ```#include <sys/sysinfo.h>```中的 ```get_nprocs()```函数来得到。