# SPMS

---

## 文件解读

>   要求：在Linux（apollo远程）下用C实现
>
>   ​	核心是**操作系统级的进程协调与调度算法**

### 五个模块

#### **一、项目核心模块与任务**

1.  **输入模块（Part I）**
    
    -   **目标**：支持用户交互输入和批量文件导入预约请求，存储所有请求（有效/无效）。
    -   **任务-得分**：
        
        -   实现命令行交互界面（如`addParking`, `addReservation`等）。
        -   解析输入命令，提取参数（用户、时间、设施需求等）。
        -   读取批量文件（如`importBatch -batch001.dat`），逐行处理请求。
        -   数据存储：设计数据结构（如链表或哈希表）保存所有请求状态（待处理/接受/拒绝）
        
        大概的实现思路：
        
        -   **具体实现**：
        
            ```c
            struct Booking {
                char member[20];          // 用户ID（如member_A）
                char type[15];            // 类型（Parking/Reservation/Event）
                char date[11];            // 日期（YYYY-MM-DD）
                char start_time[6];       // 开始时间（hh:mm）
                float duration;          // 持续时间（小时）
                int slot;                 // 停车位编号（1-10）
                char essentials[][];  // 设施列表（如battery, cable）
                int status;               // 状态（0: Pending, 1: Accepted, -1: Rejected）
            };
            ```
        
            -   **关键代码**：
                -   使用`scanf`或`getline`读取用户输入，按分号`;`分割命令。
                -   解析命令参数（如`addParking -member_A 2025-05-10 09:00 2.0 battery cable;`）。
                -   批量文件处理：逐行读取`.dat`文件，调用相同解析逻辑。
        
        
    
1.  **调度模块（Part II）**
    
    -   **目标**：基于不同调度算法生成时间表，分配资源。
    
    -   **任务**：
        -   实现至少三种调度算法：
            -   **FCFS（先到先得）**：按请求顺序分配资源。
            -   **优先级（PRIO）**：事件 > 预约 > 停车 > 设施（根据收益高低）。
            -   **优化算法（OPTI，Bonus）**：动态调整冲突请求（如重新安排被拒绝的请求）。
        -   资源管理：跟踪10个停车位及6种设施（电池、电缆等，需成对分配）。
        -   冲突处理：高优先级请求可抢占低优先级资源（如事件抢占停车位）。
        
        实现思路（大概）：
        
        -   **FCFS算法**：
        
            -   维护全局请求队列，按提交顺序处理。
            -   检查资源可用性（停车位+设施），若冲突则标记为拒绝。
        
        -   **PRIO算法**：
        
            -   定义优先级：Event > Reservation > Parking > Essentials（根据收益高低排序）。
            -   动态调整队列，高优先级请求可抢占已分配的低优先级请求。
            -   抢占机制：当高优先级请求与已接受的低优先级请求发生时间冲突时，系统会取消低优先级请求并将其状态从"Accepted"改为"Rejected"。
            -   例如：若Event请求与已接受的Parking冲突，则Parking被取消，标记为Rejected；若Reservation请求与已接受的Parking冲突，同样会导致Parking被取消。
            -   这种动态调整确保系统能够最大化收益，优先满足高价值的预约需求。
            -   实现细节：
                -   系统维护一个按优先级排序的请求队列。
                -   处理新请求时，先检查是否与已接受的低优先级请求冲突。
                -   若冲突，则取消低优先级请求，更新其状态为Rejected。
                -   被抢占的请求不会自动重新安排，除非使用OPTI算法。
                -   抢占过程中，系统会释放被抢占请求占用的所有资源（停车位和设施）。
                -   同等优先级的请求之间仍按FCFS原则处理，不会相互抢占。
        
        -   **资源管理**：
        
            -   资源限制：
                -   5个成员：member_A, member_B, member_C, member_D, member_E
                -   6种设施，每种3个：
                    -   3个储物柜（lockers）
                    -   3个雨伞（umbrellas）
                    -   3个电池（batteries）
                    -   3个电缆（cables）
                    -   3个代客泊车（valet parking）
                    -   3个充气服务（inflation services）
            
            -  资源管理：

            使用二维数组记录时间槽占用状态：
             ```c
                int time_slots[7][24]; // 7天，每天24小时  
                int battery_usage[3];  // 3个电池的占用状态（0:空闲，1:占用）
            ```
            ```c    
                // 资源使用状态跟踪
                int locker_usage[3];       // 3个储物柜的占用状态
                int umbrella_usage[3];     // 3个雨伞的占用状态
                int battery_usage[3];      // 3个电池的占用状态
                int cable_usage[3];        // 3个电缆的占用状态
                int valetpark_usage[3];    // 3个代客泊车的占用状态
                int inflation_usage[3];    // 3个充气服务的占用状态
                ```

        
            -   设施依赖规则：
                - addReservation：停车位需要配备[battery]和[cable]
                - addEvent：停车位需要配备[umbrella]和[valetpark]
                - 设施成对使用规则：
                  - battery必须与cable成对使用
                  - locker必须与umbrella成对使用
                  - valetpark可以与inflation service成对使用
         检查输入：
         booking的日期
            We test the booking schedule for a period, from 10 to 16 May, 2025
         booking时间
            booking slots between 08:00 AM – 08:00 PM. One time slot is an hour of time
2.  **输出模块（Part III）**
    -   **目标**：按格式输出所有预约结果（接受/拒绝）。
    -   **任务**：
        -   实现`printBookings`命令，支持按算法输出（如`-fcfs`）。
        -   表格化展示：按用户分组，包含时间、类型、设施详情。
        -   生成错误日志：记录无效请求（如设施名称错误）。
        
        实现思路（大概）：
        
        ​	实现`printBookings -fcfs`命令，遍历全局请求队列，按用户分组输出
        
        -   表格化输出示例：
        
            ```
            *** Parking Booking – ACCEPTED / FCFS ***
            Member_A has the following bookings:
            | Date       | Start  | End    | Type       | Device               |
            |------------|--------|--------|------------|----------------------|
            | 2025-05-10 | 09:00  | 11:00  | Parking    | Battery, Cable       |
            ```

3.  **分析模块（Part IV）**
    
    -   **目标**：生成总结报告，对比算法性能。
    -   **任务**：
        -   统计指标：总请求数、接受率、拒绝率、资源利用率。
        -   扩展性：支持未来增加停车位（当前N=3）和设施类型。
        -   输出性能报告：对比不同算法的资源利用效率。
        
        -   定义统计结构体：
        
            ```
            struct Stats {
                int total_requests;
                int accepted;
                int rejected;
                float util;  // 使用率 = 使用时长 / 总可用时长
            };
            ```
        
        -   输出报告示例：
        
            ```
            *** Summary Report ***
            For FCFS:
                Total Requests: 100
                Accepted: 70 (70%)
                Rejected: 30 (30%)
                Utilization: 85%
            ```
    
4.  **优先级检查模块（Part V）**
    -   **目标**：动态检查设施可用性，分配优先级。
    -   **任务**：
        -   设施依赖规则：如预订电池需同时预留电缆。
        -   优先级逻辑：若停车位+设施被同时占用，拒绝低优先级请求。
    

tip额外分：    
    -   **进程协调**：
        -   使用`fork()`创建子进程处理调度和输出模块。
        -   用`pipe()`实现父子进程通信（传递预约数据）。
        -   **资源竞争**：
        -   通过信号量或互斥锁（如`pthread`）管理共享资源（停车位、设施）。
        -   **数据结构**：
        -   设计时间struct（存储每天24小时，7天的占用状态）。
        -   用链表管理动态请求队列

## 动作拆解

-   **分工**：

    1.  **模块1+2（输入+调度）**-LH：

        -   负责命令解析、数据结构设计、FCFS/PRIO算法实现。

    2.  **模块3（输出）-蜘蛛侠**：

        -   实现格式化输出、统计逻辑、性能报告生成。

    2.  模块4(分析报告)-lrq：

        分析报告（对比算法性能）

    4.  **模块5+系统调用（优先级+进程通信）-yxy**：

        -   处理设施依赖、优先级抢占，集成`fork/pipe`。

    5.  **测试+文档**：
    
        ![image-20250318123109624](C:\Users\L\AppData\Roaming\Typora\typora-user-images\image-20250318123109624.png)
    
        -   编写测试用例（如高并发、边界条件），撰写项目报告。







命令格式,[列了一下](./smcmd.md)

