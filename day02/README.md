添加一个方法
____
  为了方便编码以及代码的可读性，可以封装一个错误处理函数：<br>
```c++
void errif(bool condition, const char *errmsg){
    if(condition){
         perror(errmsg);
         exit(EXIT_FAILURE);
   }
}
```

    第一个参数是是否发生错误，如果为真，则表示有错误发生，会调用<stdio.h>头文件中的perror，
    这个函数会打印出errno的实际意义，还会打印出我们传入的字符串，也就是第函数第二个参数，
    让我们很方便定位到程序出现错误的地方。然后使用<stdlib.h>中的exit函数让程序退出并返回一个预定义常量EXIT_FAILURE。
    例如，
    ```c++
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket create error");
    ```
    可以方便的查看出错的位置
