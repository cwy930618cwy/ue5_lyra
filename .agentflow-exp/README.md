# 📚 项目经验库（Local Experience Base）

> 本目录用于沉淀本项目教学/开发过程中遇到的**坑点、陷阱、最佳实践、踩坑修复方案**。
> 由 AI 助手（af）根据用户授权，在教学过程中自动维护。

---

## 🔄 工作协议（AI 读写规则）

### 每次回答前的上下文检索顺序
1. **优先**：调用 `agentflow` MCP 工具（`search_context` / `query_fix_experience`）检索全局/跨项目经验
2. **其次**：读取本目录下的项目本地经验（`.agentflow-exp/*.md`）
3. **最后**：结合当前上下文给出回答

### 经验写入时机
遇到以下情况，AI 应主动把经验**追加**到对应主题的 `.md` 文件：
- 用户踩了一个非常具体的坑（路径错、API 用错、宏漏了等）
- 用户反复在同一类问题上卡住
- 讲解了一个容易混淆的概念（如 `Triggered` vs `Started`、`&Class::Func` vs `this->Func`）
- 修复了一个编译/运行时报错，且这个坑有通用性
- 用户明确说"记一下"、"以后注意"、"下次别再犯"

### 文件命名规则
- `ue5-<主题>.md` — UE5 某个子系统的教学经验（如 `ue5-gas-teaching.md`、`ue5-enhanced-input.md`）
- `cpp-<主题>.md` — C++ 语法/用法相关
- `workflow-<主题>.md` — 协作流程相关

---

## 📂 当前经验文件清单

| 文件 | 主题 | 说明 |
|---|---|---|
| [ue5-learning-roadmap.md](./ue5-learning-roadmap.md) | **UE5 学习总路线图（SSOT）** | 六阶段总览 + 当前位置 + 下一步计划，AI 每次回答前必读 |
| [ue5-gas-teaching.md](./ue5-gas-teaching.md) | UE5 GAS + 战斗系统教学 | ASC / AttributeSet / Enhanced Input / 动画蒙太奇 相关坑点与最佳实践 |
| [workflow-ide-vscode.md](./workflow-ide-vscode.md) | VSCode + UE5 C++ 工作流 | 用户 IDE 是 VSCode（非 VS / Rider），编译/热重载/跳转/调试等术语与操作映射 |

---

## 🧭 教学推进 SOP（AI 自治流程）

1. 用户说"继续" / "下一步" / "按教程来" → AI **不问路径**，直接：
   - 读 `ue5-learning-roadmap.md` 的 `CURRENT_POSITION` 锚点
   - 推进到该位置的下一个未完成小步
2. 每完成一小步，AI 更新路线图的状态标记（🔄 → ✅）
3. 每完成一大节，AI 把本节核心经验沉淀到对应技术文档（如 `ue5-gas-teaching.md`）

---

## 🔴 教学五铁律（P0 最高优先级，任何情况不得违反）

### 铁律 1：不代写代码
- 严禁 `edit_file` / `replace_in_file` / `multi_replace` 修改 `.h` / `.cpp` 的【实质代码】
- 所有实质代码必须由用户亲手敲，我只做 review
- 例外：用户明确说"帮我写/你改/你来/你动手"时才能动手

### 铁律 2：完整代码写进回答里
- 讲解必须直接给出【可抄的完整代码块】放在聊天回答里
- 严禁说"参考 XXX 那行推导" / "自己替换 3 处" / "结构和上面一样" 等偷懒教法
- 标准流程：**原理/API 讲解 → 完整代码块 → 自检清单**

### 铁律 3：只有注释可以动手
- 用户懒得处理注释，允许 AI 单独修改 `//` 和 `/* */` 注释
- 改注释时不得连带改任何代码逻辑

### 铁律 4：UE 编辑器中文版，所有 UI 术语用中文
- 用户的 UE5 编辑器语言是**中文**
- 菜单路径、按钮、面板字段名必须用中文（可括号附英文辅助）
- ✅ 对：`编辑 → 项目设置 → GameplayTags → 添加新游戏性标签`
- ❌ 错：`Edit → Project Settings → Add New Gameplay Tag`
- 代码里的字符串 / API 名保持英文原样，**只有 UE 编辑器 UI 操作步骤**用中文

### 铁律 5：C++ IDE 是 VSCode（不是 VS，不是 Rider）
- 用户的 C++ 编辑器是 **VSCode**，所有 IDE 相关操作必须按 VSCode 讲解
- ❌ 禁止说：`Rider 里点编译` / `VS 里 F5` / `解决方案资源管理器`
- ✅ 推荐表述：`VSCode 里 Ctrl+S 保存` → `UE 编辑器工具栏点【编译】按钮（Live Coding 热重载）`
- **UE5 C++ 改完后最便捷的工作流**：保留 UE 编辑器开着 → VSCode 里保存 `.h/.cpp` → 切到 UE 编辑器顶部点【编译】按钮（或按 `Ctrl+Alt+F11` 触发 Live Coding）→ 等待底部提示"编译完成"
- 详见 [workflow-ide-vscode.md](./workflow-ide-vscode.md)

### 📖 中英文术语映射速查表（UE 中文版）

| 英文 | 中文 |
|---|---|
| Content Browser | 内容浏览器 |
| Blueprint Class | 蓝图类 |
| Details (Panel) | 细节面板 |
| Outliner | 大纲 |
| Project Settings | 项目设置 |
| Edit | 编辑 |
| Modifier / Modifiers | 修饰符 |
| Duration Policy | 持续时间策略 |
| Instant / Duration / Infinite | 瞬时 / 持续 / 无限 |
| Attribute | 属性 |
| Modifier Op | 修饰符操作 |
| Magnitude Calculation Type | 数值计算类型 |
| Set By Caller | 由调用者设置 |
| Data Tag | 数据标签 |
| Scalable Float | 可缩放浮点数 |
| Attribute Based | 基于属性 |
| Custom Calculation Class | 自定义计算类 |
| Override | 覆盖 |
| Add / Multiply / Divide | 加 / 乘 / 除 |
| Gameplay Tag List | 游戏性标签列表 |
| Add New Gameplay Tag | 添加新游戏性标签 |
| Save | 保存 |
| Compile | 编译 |
| Play (PIE) | 运行（在编辑器中运行） |
| Viewport | 视口 |
| Output Log | 输出日志 |
| World Settings | 世界设置 |
| Character Movement | 角色移动 |
| Skeletal Mesh | 骨骼网格体 |
| Static Mesh | 静态网格体 |
| Animation Blueprint | 动画蓝图 |
| Level / Map | 关卡 / 地图 |

---

## ✍️ 追加格式模板

每条经验建议按以下结构追加，方便将来检索：

```markdown
## [日期] 主题 / 坑点标题

**场景**：（什么情况下遇到的，如"绑定 IA 到角色 C++ 函数时"）
**坑点/错误**：（具体现象，如"按键无反应，Succeeded() 一直是 false"）
**根本原因**：（为什么会这样）
**正确做法**：（代码或步骤）
**关联文件**：（如 `Source/ue5_lyra/Character/BattleCharacter.cpp`）
**相关 API**：（如 `BindAction` / `ETriggerEvent::Started`）
```
