# Enhanced Input 双层 DataAsset 架构

## 设计理念

采用双层 DataAsset 结构管理输入系统，便于在编辑器中统一管理所有输入相关资产：

```
UBattleInputConfig (顶层入口)
    ├── UInputMappingContext*     → IMC_Default（按键→动作映射）
    └── UBattleInputActions*      → DA_BattleInputActions（动作引用集合）
        ├── MoveAction            → IA_Move
        ├── LookAction            → IA_Look
        ├── JumpAction            → IA_Jump
        ├── AttackAction          → IA_Attack
        ├── SprintAction          → IA_Sprint
```

## 核心代码模式

### 1. InputActions DataAsset

```cpp
UCLASS()
class UBattleInputActions : public UDataAsset {
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Actions")
    UInputAction* MoveAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Actions")
    UInputAction* LookAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Actions")
    UInputAction* JumpAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Actions")
    UInputAction* AttackAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Actions")
    TObjectPtr<UInputAction> SprintAction;
};
```

### 2. InputConfig DataAsset

```cpp
UCLASS()
class UBattleInputConfig : public UDataAsset {
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Config")
    UInputMappingContext* InputMappingContext;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Config")
    UBattleInputActions* InputActions;
};
```

### 3. BeginPlay 注册 MappingContext

```cpp
void ABattleCharacter::BeginPlay() {
    Super::BeginPlay();
    APlayerController* PC = Cast<APlayerController>(Controller);
    if (PC) {
        UEnhancedInputLocalPlayerSubsystem* Subsystem = 
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
                PC->GetLocalPlayer());
        if (Subsystem && InputConfig->InputMappingContext) {
            Subsystem->AddMappingContext(InputConfig->InputMappingContext, 0);
        }
    }
}
```

### 4. SetupPlayerInputComponent 绑定动作

```cpp
EnhancedInputComponent->BindAction(
    InputConfig->InputActions->MoveAction, 
    ETriggerEvent::Triggered, this, &ABattleCharacter::Move);
// Triggered: 持续触发（适合移动、视角）
// Started: 按下瞬间（适合跳跃、攻击）
// Completed: 释放瞬间（适合停止跳跃）
```

## ETriggerEvent 三种类型对比

| 类型 | 触发时机 | 适用场景 |
|------|---------|---------|
| Triggered | 按下+按住期间每帧 | 移动、视角控制 |
| Started | 按下瞬间 | 跳跃、攻击、冲刺开始 |
| Completed | 释放瞬间 | 停止跳跃、停止冲刺 |

## 经验总结

1. **不要在编辑器面板直接拖拽资产到角色蓝图的属性上**——C++ 构造函数中硬编码加载更可靠
2. **完整的错误日志链**：从 IMC 加载到输入子系统获取，每一步都有 UE_LOG 输出
3. **SprintAction 用 TObjectPtr** 而非裸指针，UE5 推荐的智能指针方式
4. **移动方向计算**：基于控制器 Yaw 而非角色自身旋转，WASD 始终相对于摄像机方向
5. **角色旋转三轴全部设 false**：bUseControllerRotationPitch/Yaw/Roll = false，朝向由移动方向决定
