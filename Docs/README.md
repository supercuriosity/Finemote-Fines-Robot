# 文件结构


```
│ CMakeLists.txt		整个项目的CMAKE配置，在此文件中处理公共部分的文件，并通过子模块的方式引入BSP包中提供的构建目标
├─Algorithms			算法/工具类
├─BSP					板级支持包，负责屏蔽开发板/芯片之间的差异。
│  ├─MC_Board				每个目录代表一个BSP包，目录名即为BSP包名
│  │  │ CMakeLists.txt			子目录的CMAKE配置，处理BSP包对于的文件与配置，并添加构建目标
│  │  │ MC_Board.cpp
│  │  │ MC_Board.h				与BSP包同名的头/源文件，负责提供上层结构调用BSP包的接口
│  │  │ MC_Board.ioc			CUBE项目文件
│  │  ├─Core					CUBE生成的外设配置文件
│  │  ├─Drivers					标准库和HAL库文件
│  │  └─MDK-ARM					MDK项目文件夹，和keil相关的配置等存储在这里
│  └─Robomaster_C			结构同上
├─Components			存储多个设备的集合体：组件类。组件一般在物理结构上具有一定的整体性
├─Devices				广义上的设备类
│  │ DeviceBase.cpp			
│  │ DeviceBase.h			设备基类，定义了定时执行的行为
│  ├─Controllers
│  ├─Motors
│  └─Sensors
├─Interface
│  │ InstanceManager.cpp	负责实例管理的文件，项目使用的全部对象将在此进行实例化，复杂的组件对象将使用构建器进行构建
│  │ InstanceManager.h
│  │ Interface.cpp			提供用户逻辑编写的接口，提供状态机，初始化，循环，定时中断几种调用方式
│  │ Interface.h
│  │ ProjectConfig.h		管理组件-设备-外设依赖链，并且可以手动关闭和启用组件
├─OpenOCD				OpenOCD烧录的配置文件
├─Services				服务类，主要涉及通信总线类
└─Tests					测试类
```

# 项目组织逻辑

项目采取分层逻辑，从芯片外设层次逐步封装，最终提供给用户易于调用的应用层接口。
要使用FineMote框架，首先要了解的是Interface文件夹中的文件
### Interface文件夹
其中InstanceManager管理着项目使用的所有实例化对象：存储一些对象构建时需要使用的参数，进行简单对象的直接构建，复杂对象的构建器调用构建。

- 对象的实例化要求声明与定义的分离，即在InstanceManager头文件中使用extern 关键字声明对象名，在InstanceManager源文件中进行具体的构建。由于Interface文件会包含InstanceManager的头文件，所以所有构建的对象将能在Interface文件中直接使用。
- 源文件中的对象构建顺序应该遵循：先部分后整体，依赖对象先于被依赖对象的原则

Interface提供了面向用户的调用逻辑其中Setup，Loop为用户提供了类python的使用方式，对于高级用户，也可以直接使用提供的定时器中断回调函数，其调用周期为1ms

ProjectConfig管理着项目的外设-设备-组件依赖关系，具体可以查看相关文档


# 启动流程

本框架提供基于MDK-ARM和CMake两种构建方式，两种方式的差异只存在于选择BSP包时的操作上。

对于MDK-ARM，首先选择要使用的BSP包，通过包中的keil项目文件启动整个项目。
CMake是通过子模块的方式进行管理的，并且加载了所有的可用BSP包中的构建目标，只需选择对应的构建目标即可。

程序都从名称以startup开头的汇编文件启动，汇编文件内一般调用函数SystemInit后调用__main函数，__main函数由标准库提供，内部会执行程序执行前的准备工作，也包括各个全局变量的构造函数也会在这里被执行。


> 由于本框架统一将初始化行为的内容放于构造函数之中，而部分设备需要使用HAL库的功能，所以需要在所有其他构造函数执行前，执行HAL库的初始化。具体实现方法是在BSP包中于包名同名的头文件中定义HALInit类，将HAL库初始化的内容复制到HALInit类的构造函数中，同时使用单例模式保证这个类不会被重复构造。在所有对HAL库有依赖的类中获取HALInit类的对象，即可保证HAL库被及时初始化。

综上，先执行HALInit的构造函数，再执行全局变量的构造函数，在HALInit的构造函数中将使能时间中断和其他中断。然后执行main函数中的Setup，死循环执行Loop函数。

在定时中断时，将根据所有设备类注册的顺序，反向执行设备的Handle函数。