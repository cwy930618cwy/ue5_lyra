# 九、装备系统（Equipment）

**路径**：`Source/LyraGame/Equipment/`

## 关键类

| 类名 | 职责 |
|------|------|
| `ULyraEquipmentDefinition` | 装备定义 DataAsset，配置 AbilitySets + ActorsToSpawn |
| `ULyraEquipmentInstance` | 装备实例 |
| `ULyraEquipmentManagerComponent` | 装备管理组件，处理装备/卸下逻辑 |
| `ULyraGameplayAbility_FromEquipment` | 装备赋予的技能基类 |
| `ULyraPickupDefinition` | 拾取物定义 |
| `ULyraQuickBarComponent` | 快捷栏组件 |

## 装备流程
```
1. 玩家拾取 EquipmentDefinition
2. EquipmentManagerComponent 创建 EquipmentInstance
3. 授予 Definition 中配置的 AbilitySets
4. 生成 ActorsToSpawn（武器 Mesh、特效等）
5. 玩家获得新能力
```

---

> 📌 详细笔记待补充
