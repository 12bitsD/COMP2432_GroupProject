# COMP2432_GroupProject

## 项目概述
智能停车管理系统(SPMS)用C语言开发资源调度系统，支持停车位和充电设施的预约管理。系统提供FCFS和优先级调度算法，支持实时请求处理和性能分析报告生成。

## 核心功能
-  多类型请求处理（停车/充电设施/活动预约）
-  支持FCFS/PRIO两种调度算法
-  资源使用率统计与对比报告
-  交互式命令行界面
-  批量文件处理支持

## 代码结构(2025.3.23)
```
sourceCode/
├── main.c          # 程序入口 & 流程控制
├── command_handler.c  # 命令解析与执行
├── scheduling.c    # 调度算法实现(FCFS/PRIO)
└── booking.h       # 数据结构定义(请求/资源状态)
```

## 编译运行
```bash
gcc -o spms main.c command_handler.c scheduling.c
./spms
```

## 命令
详见[命令文档](smcmd.md)，支持以下操作：
- `addParking`   添加停车请求(完成)
- `printStats`   查看统计报告（todo）
- `compareAlgo`  算法性能对比(todo)
- `importBatch`    批量导入请求文件(todo)
