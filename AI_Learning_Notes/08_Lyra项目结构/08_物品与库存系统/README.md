# 八、物品与库存系统（Inventory）

**路径**：`Source/LyraGame/Inventory/`

## 核心概念
采用 **Fragment 组合模式**：物品定义（Definition）→ 物品实例（Instance）→ Fragment 数组组合功能。

## 关键类

| 类名 | 职责 |
|------|------|
| `ULyraInventoryItemDefinition` | 物品定义 DataAsset，包含 Fragments 数组 |
| `ULyraInventoryItemInstance` | 物品实例，运行时数据 |
| `ULyraInventoryManagerComponent` | 库存管理组件，增删改查物品 |
| `IPickupable` | 可拾取接口 |
| `UInventoryFragment_EquippableItem` | 装备 Fragment |
| `UInventoryFragment_PickupIcon` | 拾取图标 Fragment |
| `UInventoryFragment_QuickBarIcon` | 快捷栏图标 Fragment |
| `UInventoryFragment_SetStats` | 属性修改 Fragment |

## Fragment 组合示例
```
InventoryItemDefinition (DataAsset)
├── Fragment: EquippableItem    → 可装备到武器槽
├── Fragment: PickupIcon        → 拾取时显示的图标
├── Fragment: QuickBarIcon      → 快捷栏显示
└── Fragment: SetStats          → 附加属性加成
```

---

> 📌 详细笔记待补充
