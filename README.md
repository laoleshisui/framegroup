# FrameGroup
**目标 / Aim：完整开源的网络游戏全栈式解决方案**

状态/帧同步引擎SDK，常用于游戏场景，但不局限于游戏场景.

参考音视频实时流封装与传输的主要流程，设计实现了包括帧的封装、收发逻辑、数据采集与回调等主要流程。可以支持任意业务场景的数据。

因为采用 C++，具备优秀的可移植性，移动端、pc端均支持。项目处于快速迭代开发期。

## Features / 功能
所有依赖库都将开源，整理中。

1. 基于 [<img src="https://simpleicons.org/icons/github.svg" width="12" alt="GitHub"> ACore](https://github.com/laoleshisui/acore) 封装的底层能力，设计高性能数据流分发处理功能
   - 跨平台：Android、iOS、Windows、Mac等均有实现
2. 设计统一通用的数据交互接口 [<img src="https://simpleicons.org/icons/github.svg" width="12" alt="GitHub"> AProto](https://github.com/laoleshisui/aproto)（基于protobuf）
3. 基于 swig 实现多语言的移植，如 C#、 python 等

## 依赖于 FrameGroup 的开源项目
1. [<img src="https://simpleicons.org/icons/github.svg" width="18" alt="GitHub">《90坦克TTT》Unity 项目](https://github.com/laoleshisui/90tankTTT-unity)
   - 跨平台：Android、iOS、Windows、Mac等均有实现


## Author / 作者
[<img src="https://simpleicons.org/icons/bilibili.svg" width="18" alt="Bilibili">BiliBili慕一仙](https://space.bilibili.com/258982523)


[<img src="https://pub.idqqimg.com/wpa/images/group.png" width="16" alt="QQ"> 加入QQ群153964874](https://qm.qq.com/q/K4KR8CCWsy)