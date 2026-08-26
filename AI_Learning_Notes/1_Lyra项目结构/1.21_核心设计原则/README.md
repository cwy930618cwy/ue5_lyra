# 二十一、核心设计原则

## Lyra 六大设计原则

1. **数据驱动**：PawnData、ExperienceDefinition、AbilitySet 都是 DataAsset，策划配表即可改变游戏行为
2. **组件化**：PawnExtensionComponent 协调各组件，功能拆分到独立 Component
3. **Tag 驱动**：GameplayTag 贯穿状态、事件、条件、输入
4. **模块化**：GameFeature 插件按需启用，Experience 动态组合
5. **网络优先**：ASC 放 PlayerState、FFastArraySerializer 增量复制、ReplicationGraph 优化
6. **Fragment 组合**：物品/装备用 Fragment 数组组合功能，避免继承爆炸

---

> 📌 详细笔记待补充
