```mermaid
flowchart TD
    classDef startEnd fill:#FFCDD2,stroke:#333,stroke-width:2px,color:#000
    classDef decision fill:#C8E6C9,stroke:#333,stroke-width:2px,color:#000
    classDef process fill:#FFE0B2,stroke:#333,stroke-width:2px,color:#000
    classDef io fill:#BBDEFB,stroke:#333,stroke-width:2px,color:#000

    Start([开始]) --> Input[输入A0至A3]
    Input --> Stage1[阶段1：去零靠拢<br/>非零数依次存入T<br/>剩余位置补0]
    Stage1 --> Init[i = 0]
    Init --> Cond1{i 小于 3?}
    
    Cond1 -->|否| Stage3[阶段3：再次去零靠拢<br/>非零数依次存入R<br/>剩余位置补0]
    Cond1 -->|是| Cond2{相邻两数相等<br/>且不等于0?}
    
    Cond2 -->|否| Next[i = i + 1]
    Cond2 -->|是| Merge[左数翻倍<br/>右数置0<br/>i = i + 2]
    
    Next --> Cond1
    Merge --> Cond1
    
    Stage3 --> Output[输出R0至R3]
    Output --> End([结束])
    
    class Start,End startEnd
    class Input,Output io
    class Stage1,Stage3,Init,Next,Merge process
    class Cond1,Cond2 decision
