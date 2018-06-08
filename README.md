# TCP-socket_Exp2
# 计算机网络实验二  by何明栩
>实验环境：Ubuntu 16.04 LTS GCC编译器（Linux）、Microsoft Visual Studio 2015（Windows）、使用C语言进行 socket编程。本次实验共有四个文件夹:<br>
>>文件夹`tcp_socket`中实现的是Linux内server端与client端传输字符串，运行时只需要分别输入./server和./client即可。<br><br>
>>文件夹`file_transfer`中实现的是Linux内server端与client端传输文件，运行时需要注意的是，server端运行时需要指定端口号与需要保存的文件，此文件可以是存在的文件也可以是不存在的文件./file_server <端口号> <需要保存的文件名>、client端：运行时需要指定ip地址、端口号与需要上传的文件./file_client <ip地址> <端口号> <需要上传的文件名>。里面的file1.txt和file2.txt为需要上传的测试文件；<br><br>
>>文件夹`linux_server`中实现的是Linux作为server端实现传输字符串和文件，其中linux_server.c是用来实现传输字符串的，运行时输入./linux_server即可；linux_server_file.c是用来实现传输文件的，运行时输入./linux_server_file即可。其中的test.txt是需要上传的测试文件；<br><br>
>>文件夹`windows_client`实现的是Windows作为client端实现Windows实现传输字符串和文件，其中Windows_client.cpp是对应用来实现传输字符串的，Windowsclient_file.cpp是对应用来实现传输文件的，均可在VS2015下运行，运行时可按“e”退出。<br><br>
>Linux与Windows之间进行TCP通信时，传输字符串实现了client端输入字符串，server端接收数据；传输文件实现了server端上传文件，client端接收文件。
<br>
## 一些小小的总结
>看了诸多参考资料，结合这些资料总结一下C socket编程的通用点：<br>
>>Server端：用socket()创建套接字->用bind()绑定套接字->用listen()将套接字设为监听状态准备接收客户请求->用accept()等待并接收客户请求并返回一个对应此连接的套接字->用返回的套接字与客户进行通信send()/recv()->最后用close()关闭连接，等待另一客户请求。在这里的accept()默认使用的是阻塞方式。
>>Client端：用socket()创建套接字->用connect()向服务器发起连接请求->和服务器进行通信send()/recv()->用close()关闭套接字。本实验实现了在Linux下开两个终端进行通信以及Linux与Windows间的通信，由于以上通用点，这些代码有很多共通的地方。
