# 长期记忆

## 用户偏好
- 遇到问题时，经验记录到项目根目录的 `AI_Learning_Notes/` 文件夹中，按主题分类存储，不混在一个文件里
- 分类目录：01_环境搭建、02_C++基础框架、03_GAS技能系统、04_战斗系统、05_AI与场景、06_工具链与技巧、07_踩坑记录
- 文件名格式：`YYYYMMDD_问题简述.md`

## 项目切换（2026-08-05）
- 用户决定在 `e:\ue5\ue5_lyra` 项目开发，不再使用 `ue5_battle`
- `ue5_lyra` 是进阶版，已实现 GAS 系统（ASC + HealthSet + TakeDamage/Heal + GE流程）
- `ue5_battle` 是基础版，GAS 未实现
- AI_Learning_Notes 已整体迁移到 ue5_lyra 目录下

## AgentFlow MCP
- 地址：http://127.0.0.1:9080/mcp
- 版本：v2.0.0
- workspace 未关联，API 返回空数据

## 会话惯例（2026-08-05）
- 每次对话开始时自动连接 AgentFlow MCP（健康检查确认连通）
- 连接后直接进入当前学习阶段的任务推进（当前在 P2 攻击与伤害）

## AI 教学注意事项（2026-08-05）
- **每次教用户写代码前，必须先读一遍 MEMORY.md 和当天日志**，回顾之前的经验和踩过的坑
- 教用户写代码时，涉及类名/函数名必须对照真实代码确认，不能凭印象编造
- 错误案例：前向声明写成 `ULyraHealthSet`，实际项目里叫 `UHealthSet`（Lyra 示例项目才有 Lyra 前缀）
- 用户偏好：每次只讲一个点，拆细了教，不要一次给大段代码
- **严禁直接修改用户代码**，只教不改，让用户自己动手
- **教新代码时必须提前列出所有可能踩的坑**：访问权限（public/protected/private）、include 是否齐全、前向声明是否够用，不能等编译报错再补救
- 错误案例（2026-08-18）：教用户加 HealthBarWidget 指针到 HealthComponent 时，没提醒要放 public 也没提醒 .cpp 要 include Widget 头文件，导致编译报 C2248（protected 访问）+ C2027（未定义类型）两个错误

## ⚠️ P0：UE5.5 API 变更强制规范（2026-08-20）
- **项目使用 UE 5.5，所有教学代码必须用 5.5+ 新 API，禁止使用旧版写法**
- 每次教 GAS 相关代码前，先查 `C:\Program Files\Epic Games\UE_5.5\Engine\Plugins\Runtime\GameplayAbilities\Source\GameplayAbilities\Public\` 或 Lyra 源码确认 API 名称
- **已知的 UE5.5 API 变更清单**：
  | 旧写法（5.4-） | 新写法（5.5+） |
  |---|---|
  | `EGameplayAbilityInstancingPolicy::PerExecution` | `EGameplayAbilityInstancingPolicy::InstancedPerExecution`（注意：Lyra 实际用 `InstancedPerActor`） |
  | `FGameplayAbilityTriggerData(EGameplayAbilityTriggerSource::X)` 构造函数 | `FAbilityTriggerData TriggerData; TriggerData.TriggerSource = ...; AbilityTriggers.Add(TriggerData);` |
- 错误案例（2026-08-20）：教 GA_BattleAbility 时用旧版 `PerExecution` 枚举名 + `FGameplayAbilityTriggerData` 构造函数，导致 C2039/C2065/C3861 三次编译报错，用户明确要求"更新教程用 5.5+ 新写法"

## 调试打印规范（2026-08-18）
- **项目中所有运行时调试打印统一使用 `UDebugHelper::DebugLog`**（位于 `Components/DebugHelper/`）
- 不要用 `UE_LOG` 做运行时调试打印（仅启动时的严重错误保留 UE_LOG）
- `UDebugHelper::DebugLog(Message, Duration, Color)` 同时输出到屏幕 + 日志
- 颜色约定：受伤/扣血=Red，治疗/加血=Green，状态切换/信息=Yellow
- 该方法基于 `UBlueprintFunctionLibrary`，蓝图也能调用
