## 停车计费系统流程图

```mermaid
flowchart TD
    %% 定义样式类
    classDef startEnd fill:#90EE90,stroke:#333,stroke-width:2px,color:#000
    classDef decision fill:#FFFACD,stroke:#333,stroke-width:2px,color:#000
    classDef process fill:#ADD8E6,stroke:#333,stroke-width:2px,color:#000
    classDef penalty fill:#FFA07A,stroke:#333,stroke-width:2px,color:#000
    classDef free fill:#98FB98,stroke:#333,stroke-width:2px,color:#000
    classDef note fill:#F0F0F0,stroke:#666,stroke-width:1px,stroke-dasharray: 5 5,color:#333

    %% 流程开始
    Start([开始]) --> Input[输入停车记录<br/>车型/车位/车辆类型/充电状态/时长]
    class Start startEnd
    class Input process

    %% 第一层判断：车位类型
    Input --> SpotType{车位类型?}
    class SpotType decision
    
    %% ========== 普通车位分支 ==========
    SpotType -->|普通车位| NormalTime{停车时长<br/>>30分钟?}
    class NormalTime decision
    
    NormalTime -->|否| Free1[免费<br/>0元]
    class Free1 free
    
    NormalTime -->|是| CarType1{车型?}
    class CarType1 decision
    
    CarType1 -->|小车| NormalSmall[按小时计费<br/>5元/小时]
    CarType1 -->|大车| NormalLarge[按小时计费<br/>10元/小时]
    class NormalSmall,NormalLarge process
    
    NormalSmall --> Cap1{>24小时?}
    NormalLarge --> Cap2{>24小时?}
    class Cap1,Cap2 decision
    
    Cap1 -->|是| Fee1[封顶50元]
    Cap1 -->|否| FeeNormal1[费用=min(小时数×5, 50)]
    Cap2 -->|是| Fee2[封顶100元]
    Cap2 -->|否| FeeNormal2[费用=min(小时数×10, 100)]
    class Fee1,Fee2,FeeNormal1,FeeNormal2 process
    
    %% ========== 充电车位分支 ==========
    SpotType -->|充电车位| Energy{是否新能源车?}
    class Energy decision
    
    %% 非新能源车占用充电位
    Energy -->|否| OccupyTime1{占用时长<br/>>30分钟?}
    class OccupyTime1 decision
    OccupyTime1 -->|否| OccupyShort[惩罚性计费<br/>半价:小车15元/时,大车30元/时]
    OccupyTime1 -->|是| OccupyLong[惩罚性计费<br/>全价:小车30元/时,大车60元/时]
    class OccupyShort,OccupyLong penalty
    
    %% 新能源车继续判断
    Energy -->|是| Charging{是否正在充电?}
    class Charging decision
    
    %% 新能源车未充电（占用不充）
    Charging -->|否| OccupyTime2{占用时长<br/>>30分钟?}
    class OccupyTime2 decision
    OccupyTime2 -->|否| OccupyShort2[惩罚性计费<br/>半价:小车15元/时,大车30元/时]
    OccupyTime2 -->|是| OccupyLong2[惩罚性计费<br/>全价:小车30元/时,大车60元/时]
    class OccupyShort2,OccupyLong2 penalty
    
    %% 新能源车充电中或已充满
    Charging -->|是| ChargeStatus{充电状态?}
    class ChargeStatus decision
    
    ChargeStatus -->|充电中| Free2[免费<br/>0元]
    class Free2 free
    
    ChargeStatus -->|已充满| Overdue[计算占位费<br/>1元/分钟]
    class Overdue process
    Overdue --> Cap3[封顶100元]
    class Cap3 process
    
    %% 占用计费封顶（非新能源或未充电）
    OccupyShort --> Cap4{占用封顶检查}
    OccupyLong --> Cap4
    OccupyShort2 --> Cap5{占用封顶检查}
    OccupyLong2 --> Cap5
    Cap4 -->|小车| CapSmall[封顶150元]
    Cap4 -->|大车| CapLarge[封顶300元]
    Cap5 -->|小车| CapSmall
    Cap5 -->|大车| CapLarge
    class Cap4,Cap5 decision
    class CapSmall,CapLarge penalty
    
    %% 汇总输出
    Free1 --> Output
    Fee1 --> Output
    FeeNormal1 --> Output
    Fee2 --> Output
    FeeNormal2 --> Output
    Free2 --> Output
    Cap3 --> Output
    CapSmall --> Output
    CapLarge --> Output
    
    Output[输出结果<br/>停车费 + 占位费] --> End([结束])
    class Output process
    class End startEnd
    
    %% 添加注释说明
    Note1[注：所有计费采用进一法<br/>不足1小时按1小时计] -.-> Input
    Note2[注：普通车位与占用计费<br/>分别独立封顶] -.-> Output
    class Note1,Note2 note
```
