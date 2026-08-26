# Experience 系统总览

## 一句话概括

> **Experience 系统是 Lyra 的核心架构——用数据驱动替代继承，让同一套 C++ 代码通过不同配置组合出多种游戏模式。**

---

## 你会学到什么

本教程按以下顺序逐步深入，建议按编号阅读：

| 序号 | 主题 | 核心问题 |
|------|------|----------|
| 01 | [为什么需要 Experience 系统](./1.1.1.1_为什么需要Experience系统/) | 传统做法有什么问题？Lyra 怎么解决的？ |
| 02 | [核心文件清单](./1.1.1.2_核心文件清单/) | 十几个文件各司什么职责？谁加载谁执行？ |
| 03 | [完整工作流程时间线](./1.1.1.3_完整工作流程时间线/) | 从游戏启动到玩家拥有角色，经历了哪些步骤？ |
| 04 | [加载状态机](./1.1.1.4_加载状态机/) | Experience 有哪些加载状态？Chaos Testing 是什么？ |
| 05 | [依赖关系图](./1.1.1.5_依赖关系图/) | 类与类之间怎么依赖？数据怎么流转？ |

---

## 学习路线图

```
01 为什么需要？          ← 理解动机（传统 vs Lyra）
    │
    ▼
02 有哪些文件？          ← 建立地图（10 个类按职责分类）
    │
    ▼
03 怎么跑起来的？        ← 理解流程（8 步时间线）
    │
    ▼
04 有哪些状态？          ← 理解细节（7 个状态 + 异常处理）
    │
    ▼
05 谁依赖谁？            ← 形成全局观（类依赖 + 数据流）
```

---

## 涉及的核心类速查

| 类名 | 类型 | 一句话职责 |
|------|------|-----------|
| `ULyraExperienceDefinition` | 数据资产 | 配置表：插件列表 + Pawn 数据 + Action 列表 |
| `ULyraExperienceManagerComponent` | ActorComponent | 加载器：挂在 GameState 上，异步加载 Experience |
| `ULyraExperienceManager` | UObject | 执行器：被 Component 委托，实际执行加载 |
| `ULyraExperienceActionSet` | 数据资产 | 可复用的 Action 组合包 |
| `ALyraGameState` | GameState | 承载 ExperienceManagerComponent |
| `ALyraGameMode` | GameMode | 读取 Experience 决定 Pawn 类型 |
| `ALyraWorldSettings` | WorldSettings | 关卡里配置的默认 Experience |
| `ULyraUserFacingExperienceDefinition` | 数据资产 | 主菜单显示的玩法列表 |
| `ULyraBotCreationComponent` | ActorComponent | 根据 Experience 动态创建 AI Bot |
| `UAsyncAction_ExperienceReady` | 蓝图节点 | 蓝图中等待 Experience 加载完成 |
