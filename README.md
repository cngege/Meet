<div align=center>
  <img src="https://capsule-render.vercel.app/api?type=Waving&color=timeGradient&height=200&animation=fadeIn&section=header&text=山水有“相逢”&fontSize=60" />
</div>

---

### ⚓这是什么?⚓ ###

- 🥗这是一个用C++编写的TCP网络框架(UDP部分正在开发中),以方便进行网络数据交换,通信。
- 🥟框架以注册回调的方式,获取远程网络消息,断开消息，错误消息

### 🌭我要怎么才能在自己的项目里面使用这个库?🌭 ###

- 🍵很简单, 先将这个项目克隆到你的项目所在的计算机上
  - 如： `git clone https://github.com/cngege/Meet.git`
- ☕将`include`文件夹中的`Meet.hpp`复制到你的项目中
- 🍺然后引用 `#include "Meet.hpp"`文件即可
- 🍻之后就是使用API 来调用这个库就好了

### 🥽API🥽 ###

- 公共API **本项目所有代码均在命名空间 "meet" 下**


```cpp
// meet::IP
IP addr;
addr.toString()
//该函数可以将IP类型的地址信息转为 字符串类型的IP地址
addr = "127.0.0.1";
//字符串直接转化为IP类

// 其他IP相关的信息 等待后续补充

// meet::Error
Error 有多种错误类型,与一直表示没有错误的类型
auto err = Error::noError;

// 可以用 meet::getString(err);
// 将错误类型转为字符串 -> "noError"

```

- TCPClient API

```cpp
// 建立客户端类，进行所有的客户端操作
TCPClient c;

// 注册事件回调,在创建类后，建立连接之前进行
// 比如 注册 被动断开连接时的回调
c.onDisConnect([]() {
  std::cout << "服务端断开了连接" << std::endl;
});

// 其他注册回调请看示例文件 test.cpp

// 回调函数参数可去 类的定义中查看
// 在建立连接之前除了 要注册回调,还有设置阻塞/非阻塞模式也要在连接之前完成

// 关闭阻塞模式 (默认阻塞模式为 true)
c.setBlockingMode(false);

// 设置回调与设置阻塞模式完成之后,便是建立连接
c.connectV4("127.0.0.1", 3000)
//c.connectV6("::1", 3000)
//c.connect()
// 返回公共类型 meet::Error, 判断返回的是否是 Error::noError 以判断是否连接成功

// 发送消息
//c.sendText(std::string)
//c.sendData(const char* data)
// 同样判断返回值是否是 Error::noError 来判断是否发送成功

```

- TCPServer Api

```cpp
// 服务端的API和客户端的API很相近
// 首先初始化服务端类
TcpServer s;

//分别设置输入参数 IP,最大连接数量
s.setListenAddress("::"); // 设置监听地址,默认0.0.0.0
s.setMaxConnectCount(30);


//然后是设置阻塞和非阻塞模式
//设置回调
s.onClientDisConnect([](meet::TCPServer::MeetClient meetClient) {
   printf("\n[%s -:- %d][连接] 断开连接\n", meetClient.addr.toString().c_str(), meetClient.port);
});

// 然后就是开启监听
s.Listen(1234);  // 传参设置监听端口
// 同样判断返回值是否是 Error::noError
// 来判断是否监听成功

//获取所有连接的客户端
std::vector<MeetClient>& GetALLClient()
// 使用引用传递,防止在修改时客户端断开连接,值未同步变更
auto& all = s.GetALLClient();
// 若不使用引用传递,请获取所有客户端后在耗时操作前处理完毕
auto all = s.GetALLClient();
// 或者在客户端连接、断开连接时重新获取

// 断开客户端的连接
Error disClientConnect(IP addr,ushort port)

```
