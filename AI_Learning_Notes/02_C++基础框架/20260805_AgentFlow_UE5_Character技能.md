# AgentFlow ue5_character 技能 DNA

> 来源：AgentFlow MCP `get_skill("ue5_character")` | 版本 v8 | 最后进化 2026-04-21

## 1. Enhanced Input 攻击蒙太奇完整链路

UE5 Enhanced Input + AnimMontage 攻击的完整链路，**缺少任何一环都会导致攻击无反应**：

1. 在编辑器创建 `IA_Attack` 资产（Value Type = Digital）
2. 在 IMC 里绑定按键（如鼠标左键）
3. C++ 里 `BindAction` 绑定 `ETriggerEvent::Started`
4. `Attack()` 函数里调用 `Montage_Play`
5. 动画蓝图 AnimGraph 里**必须有 DefaultSlot 节点**连在状态机和 Output Pose 之间

调试顺序：先加 `UE_LOG` 确认 `Attack()` 是否被调用 → 再检查蒙太奇和 Slot 节点。

## 2. ObjectInitializer 替换默认组件的标准写法

UE5 中替换 `ACharacter` 默认的 `UCharacterMovementComponent`，**必须使用带 `FObjectInitializer` 参数的构造函数**：

```cpp
ABattleCharacter(const FObjectInitializer& ObjectInitializer) 
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UBattleCharacterMovementComponent>(
        ACharacter::CharacterMovementComponentName))
{
    BattleMovement = Cast<UBattleCharacterMovementComponent>(GetCharacterMovement());
}
```

原因：父类在构造函数执行前就已经创建了默认组件，只有通过 ObjectInitializer 才能在创建前拦截并替换。**对标 Lyra 的 `ALyraCharacter` 写法**。

## 3. Constructor 里修改共享 DataAsset 属性会污染资产

⚠️ **UDataAsset 是只读的共享资产**，在 ACharacter 的 Constructor 里直接修改 DataAsset 的属性（如 `InputConfig->InputActions = NewObject<...>()`）会：

- 污染原始资产，导致所有角色实例共享被修改的数据
- 热重载时出问题
- 引入变量遮蔽、空指针崩溃等新 bug

✅ **正确做法**：DataAsset 只存配置引用，不在 Constructor 里动态修改它的属性；或者把需要动态赋值的字段直接放在 Character 自身的成员变量上。

## 4. 热重载时 FObjectFinder 找不到 DataAsset

在 Constructor 里用 `FObjectFinder` 加载 UDataAsset（如 `DA_BattleInputActions`）时，热重载（Live Coding）会偶发 `'CDO Constructor: Failed to find'` 警告。

- **原因**：热重载时资产加载顺序不稳定
- **处理**：这只是热重载的警告，不影响正常运行，**直接忽略即可**
- ⚠️ **不要因为这个警告去改架构**（如改用 NewObject），改了反而会引入 DataAsset 污染问题

## 5. 蒙太奇播放无效果

在 UE5 中播放 `AnimMontage` 时，如果动画蓝图 AnimGraph 里没有连接 **DefaultSlot 节点**，蒙太奇会在后台播放但画面上完全看不到任何变化。

**正确连接方式**：状态机 → Slot `'DefaultSlot'` (Group `'DefaultGroup'`) → Output Pose

这是蒙太奇能否显示的根本前提，C++ 代码本身没有问题时首先检查这里。

## 6. P1-合并：热重载 FObjectFinder 警告 + DataAsset 污染陷阱

综合上述 3、4 条的经验总结：

| 现象 | 根因 | 正确处理 |
|------|------|----------|
| 热重载时 `Failed to find` 警告 | 资产加载顺序不稳定 | **忽略即可** |
| 想用 NewObject 替代 FObjectFinder | 误解了警告含义 | **保持 FObjectFinder** |
| Constructor 里修改 DataAsset 属性 | 以为可以动态赋值 | **DataAsset 只读，不改属性** |
