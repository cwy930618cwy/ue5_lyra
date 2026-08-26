# 四、输入系统（Input）

**路径**：`Source/LyraGame/Input/`

## 关键类

| 类名 | 职责 |
|------|------|
| `ULyraInputConfig` | 输入配置 DataAsset，定义 NativeInputActions + AbilityInputActions |
| `ULyraInputComponent` | Lyra 输入组件，提供 `BindNativeAction` / `BindAbilityActions` 模板方法 |
| `ULyraInputModifiers` | 输入修饰器（死区、反转、灵敏度曲线等） |
| `ULyraInputUserSettings` | 用户输入设置（键位绑定、灵敏度等） |
| `ULyraPlayerMappableKeyProfile` | 按键映射配置 |
| `ULyraAimSensitivityData` | 瞄准灵敏度数据 |

## 输入绑定流程
```
1. InputConfig 定义 InputAction → GameplayTag 映射
2. LyraInputComponent::BindAbilityActions() 将 InputTag 绑定到 ASC
3. 玩家按下按键 → InputAction 触发 → ASC->TryActivateAbilitiesByTag()
4. 对应 GA 被激活
```

---

> 📌 详细笔记待补充
