# 🛠️ VSCode + UE5 C++ 工作流经验

> 本项目用户的 C++ IDE 是 **VSCode**（不是 Visual Studio，也不是 Rider）。
> 本文档沉淀 VSCode 环境下教学 UE5 C++ 时必须遵循的术语、操作映射、踩坑点。

---

## 🔴 P0 核心原则

- 所有 IDE 操作指令必须按 **VSCode** 工作流给
- 严禁出现 `Rider` / `Visual Studio` / `VS 里 F5` / `解决方案资源管理器` / `Ctrl+Shift+B (VS)` 等非 VSCode 术语
- 用户已反复强调过这条，每次违反都会被斥责

---

## 🧭 UE5 C++ 改代码后的标准工作流（VSCode）

### 方案 A：Live Coding 热重载（最常用，推荐）
1. **VSCode** 里改 `.h` / `.cpp`
2. **VSCode** 里 `Ctrl + S` 保存
3. 切到 **UE 编辑器**（保持开着，不要关）
4. 点击 UE 编辑器**右下角**或**工具栏**的【**编译**】按钮（图标是一个小扳手/锤子，或按快捷键 `Ctrl + Alt + F11`）
5. 等待底部状态栏提示"**Live Coding 编译完成**" / "**Compile Complete**"
6. 直接运行 / 继续测试，**不需要重启编辑器**

> ⚠️ 仅改【函数体】用 Live Coding 没问题；如果改了【类成员变量 / UPROPERTY / UCLASS 结构】，有时需要**关掉编辑器后重新编译**。

### 方案 B：完全重编（改了头文件结构 / Live Coding 失败时）
1. 关闭 UE 编辑器
2. **VSCode** 里打开终端（`` Ctrl + ` ``）
3. 运行 UBT 构建命令（或直接用 VSCode 左侧【运行和调试】面板选 `Launch UE5Editor (Development)`）
4. 编译通过后自动拉起编辑器

---

## 📋 VSCode ↔ 其他 IDE 术语映射

| 操作 | VSCode | Visual Studio | Rider |
|---|---|---|---|
| 保存 | `Ctrl+S` | `Ctrl+S` | `Ctrl+S` |
| 跳转到定义 | `F12` | `F12` | `Ctrl+B` |
| 查找所有引用 | `Shift+F12` | `Shift+F12` | `Alt+F7` |
| 全局搜索 | `Ctrl+Shift+F` | `Ctrl+Shift+F` | `Ctrl+Shift+F` |
| 命令面板 | `Ctrl+Shift+P` | ❌ | `Ctrl+Shift+A` |
| 重命名符号 | `F2` | `Ctrl+R, R` | `Shift+F6` |
| 打开终端 | `` Ctrl+` `` | — | `Alt+F12` |
| 编译（项目） | 终端跑 `Build.bat` / 任务 | `Ctrl+Shift+B` | `Ctrl+F9` |
| 运行调试 | `F5`（需配 launch.json） | `F5` | `Shift+F9` |

> 教学时只需用 **VSCode** 那列；其他列仅供我自己参考，不要输出给用户。

---

## ⚠️ 常见踩坑

### 坑 1：Live Coding 提示成功但行为没变
- 原因：改了【UPROPERTY 新增/删除】、【UCLASS 继承关系】等结构性变化，Live Coding 处理不了
- 解法：关编辑器 → VSCode 终端跑完整 Build → 重新启动编辑器

### 坑 2：VSCode 的 IntelliSense 一直报红 / 找不到 UE API
- 原因：`compileCommands_*.json` 没生成或过时
- 解法：右键 `.uproject` → Generate Visual Studio project files（是的，文件名这样，但 VSCode 也用它生成 `compile_commands.json`）→ VSCode 里 `Ctrl+Shift+P` → `C/C++: Edit Configurations (JSON)` 指向生成的文件

### 坑 3：教学时误说"Rider 里点编译"
- 用户会立刻斥责
- 正确表述：**"VSCode 里保存 → UE 编辑器点【编译】按钮"**

---

<!-- 新经验追加到这里 -->

## [2026-04-26] P0 铁律 5：用户 IDE 是 VSCode，禁说 Rider/VS

**场景**：教学 UE5 GAS 时讲到"改完 cpp 怎么编译"
**坑点**：AI 多次在回答里写 `Rider 里点编译` / `VS 里 Ctrl+Shift+B`
**根本原因**：默认习惯用了主流商业 IDE 的术语，忽视了用户实际环境
**正确做法**：
- 只说 **VSCode** 的快捷键和工作流
- 编译一律表述为：**"VSCode 里 Ctrl+S 保存 → UE 编辑器顶部点【编译】按钮触发 Live Coding 热重载"**
- 不关心用户底层是点按钮还是用 F11，核心指令是"保存 + 去 UE 编辑器点编译"
**关联文件**：全局教学行为
**相关记忆**：memory:7y7n7glj（P0 五铁律）
