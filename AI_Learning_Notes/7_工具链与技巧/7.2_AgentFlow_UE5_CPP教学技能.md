# AgentFlow ue5_cpp_teaching 技能 DNA

> 来源：AgentFlow MCP `get_skill("ue5_cpp_teaching")` | 版本 v5 | 最后进化 2026-04-25

## 1. 编译方式教学

学生不知道怎么编译代码、Compile 按钮在哪里。UE5 编辑器中文版的编译按钮位置不直观。

**必须在"写完代码"步骤之后，明确教三种编译方式**：
1. 快捷键 **Ctrl+Alt+F11**（最推荐）
2. 顶部菜单 **工具→编译**
3. 关闭编辑器重开（最稳妥）

编译成功 → 底部显示绿色提示；编译失败 → 底部显示红色错误信息。

## 2. 变量名一致性

学生容易在 .h 中声明变量名为 `CameraBoom/FollowCamera`，但在 .cpp 中写成 `SpringArm/Camera`，导致编译报错 `error C2065: 未声明的标识符`。

**教学时必须强调**：你在 .h 里声明的变量叫什么名字，.cpp 里就必须用一模一样的名字。

## 3. 子文件夹组织代码的前置步骤

当教学生使用子文件夹组织代码（如 `BattleCharacter/BattleCharacter.h`）时，**必须在教创建文件之前，先教学生在 Build.cs 中添加**：

```cpp
PublicIncludePaths.AddRange(new string[] { ModuleDirectory });
```

否则编译会报 `fatal error C1083: No such file or directory`。

**正确教学流程**：
1. 改 Build.cs 加 include 路径
2. 创建子文件夹和文件
3. 写代码
4. 编译

## 4. 全程 C++ 教学的资源绑定

当学生已经明确要求"全程 C++ 教学"后，不要再给编辑器操作方案。

**正确做法**：一开始就教用 `ConstructorHelpers::FObjectFinder<USkeletalMesh>` 和 `ConstructorHelpers::FClassFinder<UAnimInstance>` 在构造函数中加载资源。

## 5. UE5 资产迁移的精准筛选原则（最小依赖 SOP）

从 Lyra 等大型项目迁移单个动画到自己项目时的标准流程：

### Step 1：分析依赖
右键资产 → "参考查看器"，展开依赖树辨识必需/可选。

### Step 2：只勾选 5 类核心
- Skeleton（SK_Mannequin_Skeleton）
- Skeletal Mesh（SKM_Manny）
- PhysicsAsset
- 目标 Animation Sequence 本身
- 相关 Material（可选）

### Step 3：明确取消勾选的类别
- ABP_* 动画蓝图
- AnimBP_Mannequin_*
- CR_Mannequin_Procedural（控制绑定）
- BP_* 角色蓝图
- RTPC_* 音效蓝图
- AbilitySystem 相关（GA_/GE_/IA_*）
- Niagara 粒子
- 输入映射 IMC_*

### Step 4：覆盖策略选"不覆盖"或"No to All"

### Step 5：迁移完成后立刻刷新验证

这样迁移的文件通常控制在 **10 个以内**，不会污染项目。

## 6. 学生烦躁时的精简三步法

当学生连续出现 `"!!!!"`、`"为什么老是"`、`"重新教我"` 等烦躁信号时：

**禁止再给长篇操作清单，立即切换到精简三步法**：
1. 一句话诊断问题根因（如"你连线反了"）
2. 给一个最小可验证操作（如"把 Run 连 True Pose、Walk 连 False Pose"）
3. 提供一个 30 秒内能执行完的自检步骤（如"按 Shift 看 Log 里 bIsSprinting 是 0 还是 1"）

不要再展开背景知识或列出 5 个可能原因。

## 7. 教学中区分动画资产类型

| 资产类型 | 图标 | 用途 | 示例 |
|---------|------|------|------|
| Animation Sequence | 蓝色胶片条 | 实际动画数据，用于状态机/Blendspace | MM_Walk_Fwd、MM_Pistol_Melee |
| Animation Montage | 粉色 | 一次性动作（攻击/受击/技能），通过 Montage_Play 触发 | AM_Melee_Attack |
| Pose Asset | 深蓝 | 单帧静态姿态/混合姿势，用于面部表情 | SplashPose_15 |
| Animation Blueprint | 绿色立方 | 驱动逻辑，包含 AnimGraph/EventGraph | BP_BattleAnimInstance |

⚠️ **切勿让学生用 Pose Asset 当攻击动画**。
