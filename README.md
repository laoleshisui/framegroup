# FrameGroup
状态/帧同步引擎SDK，常用于游戏场景，但不局限于游戏场景.

参考音视频实时流封装与传输的主要流程，设计实现了包括帧的封装、收发逻辑、数据采集与回调等主要流程。可以支持任意业务场景的数据。

因为采用 C++，具备优秀的可移植性，移动端、pc端均支持。

## Features
1. 基于 ACore 封装的底层能力，设计高性能数据处理业务逻辑
2. 基于 AProto（基于protobuf）实现数据的封装。
3. 基于 swig 实现多语言的移植，如C#、python等

## 依赖于 FrameGroup 的开源项目
1. 《90坦克TTT》Unity 项目
   - 跨平台：Android、iOS、Windows、Mac等均有实现