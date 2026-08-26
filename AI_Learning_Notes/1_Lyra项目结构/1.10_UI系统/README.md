# 十、UI 系统（UI）

**路径**：`Source/LyraGame/UI/`

## 子目录结构
```
UI/
├── Basic/              ← 基础 UI 元素
├── Common/             ← 通用 UI 组件（血条、准星、提示等）
├── Foundation/         ← UI 框架基础（Widget 基类、布局）
├── Frontend/           ← 主菜单/大厅 UI
├── IndicatorSystem/    ← 指示器系统（队友标记、目标指示）
├── PerformanceStats/   ← 性能统计 UI
├── Subsystem/          ← UI 子系统
├── Weapons/            ← 武器相关 UI（准星、弹药）
├── LyraHUD.cpp         ← HUD 主类
├── LyraHUDLayout.cpp   ← HUD 布局
├── LyraActivatableWidget.cpp  ← 可激活 Widget 基类
├── LyraJoystickWidget.cpp     ← 摇杆 Widget
├── LyraSettingScreen.cpp      ← 设置界面
├── LyraSimulatedInputWidget.cpp ← 模拟输入 Widget（触屏）
├── LyraTouchRegion.cpp        ← 触屏区域
└── LyraTaggedWidget.cpp       ← 带 Tag 的 Widget
```

## 关键类

| 类名 | 职责 |
|------|------|
| `ULyraHUD` | HUD 主类，管理所有 HUD Widget |
| `ULyraHUDLayout` | HUD 布局容器 |
| `ULyraActivatableWidget` | 可激活/停用 Widget 基类（CommonUI 集成） |
| `ULyraTaggedWidget` | 带 GameplayTag 的 Widget，支持条件显示 |
| `ULyraJoystickWidget` | 虚拟摇杆（触屏设备） |
| `ULyraSettingScreen` | 设置界面 |
| `ULyraGameViewportClient` | Viewport 客户端，处理 UI 输入 |

---

> 📌 详细笔记待补充
