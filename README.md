## 停车计费系统流程图

```mermaid
flowchart TD
    classDef startEnd fill:#90EE90,stroke:#333,stroke-width:2px,color:#000
    classDef decision fill:#FFFACD,stroke:#333,stroke-width:2px,color:#000
    classDef process fill:#ADD8E6,stroke:#333,stroke-width:2px,color:#000
    classDef penalty fill:#FFA07A,stroke:#333,stroke-width:2px,color:#000
    classDef free fill:#98FB98,stroke:#333,stroke-width:2px,color:#000
    classDef note fill:#F0F0F0,stroke:#666,stroke-width:1px,stroke-dasharray:5 5,color:#333

    Start([开始]) --> Input[输入停车记录]
    Input --> SpotType{车位类型}
    
    SpotType -->|普通车位| NormalTime{停车时长大于30分钟}
    NormalTime -->|否| Free1[免费]
    NormalTime -->|是| CarType1{车型}
    
    CarType1 -->|小车| NormalSmall[按小时计费<br/>5元每小时]
    CarType1 -->|大车| NormalLarge[按小时计费<br/>10元每小时]
    
    NormalSmall --> Cap1{大于24小时}
    NormalLarge --> Cap2{大于24小时}
    
    Cap1 -->|是| Fee1[封顶50元]
    Cap1 -->|否| FeeNormal1[费用计算<br/>小时数乘5]
    Cap2 -->|是| Fee2[封顶100元]
    Cap2 -->|否| FeeNormal2[费用计算<br/>小时数乘10]
    
    SpotType -->|充电车位| Energy{是否新能源车}
    
    Energy -->|否| OccupyTime1{时长大于30分钟}
    OccupyTime1 -->|否| OccupyShort[惩罚性计费半价<br/>小车15元每小时]
    OccupyTime1 -->|是| OccupyLong[惩罚性计费全价<br/>小车30元每小时]
    
    Energy -->|是| Charging{是否正在充电}
    
    Charging -->|否| OccupyTime2{时长大于30分钟}
    OccupyTime2 -->|否| OccupyShort2[惩罚性计费半价]
    OccupyTime2 -->|是| OccupyLong2[惩罚性计费全价]
    
    Charging -->|是| ChargeStatus{充电状态}
    
    ChargeStatus -->|充电中| Free2[免费]
    ChargeStatus -->|已充满| Overdue[计算占位费<br/>1元每分钟]
    Overdue --> Cap3[封顶100元]
    
    OccupyShort --> Cap4{占用封顶检查}
    OccupyLong --> Cap4
    OccupyShort2 --> Cap5{占用封顶检查}
    OccupyLong2 --> Cap5
    
    Cap4 -->|小车| CapSmall[封顶150元]
    Cap4 -->|大车| CapLarge[封顶300元]
    Cap5 -->|小车| CapSmall
    Cap5 -->|大车| CapLarge
    
    Free1 --> Output[输出结果]
    Fee1 --> Output
    FeeNormal1 --> Output
    Fee2 --> Output
    FeeNormal2 --> Output
    Free2 --> Output
    Cap3 --> Output
    CapSmall --> Output
    CapLarge --> Output
    
    Output --> End([结束])
    
    class Start,End startEnd
    class SpotType,NormalTime,CarType1,Cap1,Cap2,Energy,OccupyTime1,Charging,OccupyTime2,ChargeStatus,Cap4,Cap5 decision
    class Input,NormalSmall,NormalLarge,Fee1,FeeNormal1,Fee2,FeeNormal2,OccupyShort,OccupyLong,OccupyShort2,OccupyLong2,Overdue,Cap3,CapSmall,CapLarge,Output process
    class Free1,Free2 free
    class OccupyShort,OccupyLong,OccupyShort2,OccupyLong2,CapSmall,CapLarge penalty
