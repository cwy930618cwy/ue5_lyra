# C++ 驱动 AnimInstance 的模式

## 设计思路

C++ 侧每帧计算状态变量 -> 暴露给蓝图 -> 蓝图动画状态机消费这些变量。既保持 C++ 的逻辑清晰性，又利用蓝图动画状态机的可视化编辑能力。

## 暴露的状态变量

```cpp
UPROPERTY(BlueprintReadOnly, Category = "Animation State")
float Speed;                  // 角色速度大小

UPROPERTY(BlueprintReadOnly, Category = "Animation State")
bool bIsInAir;                // 是否在空中

UPROPERTY(BlueprintReadOnly, Category = "Animation State")
bool bIsMoving;               // 是否在移动（带死区）

UPROPERTY(BlueprintReadOnly, Category = "Animation State")
bool bIsAttacking;            // 是否正在播放攻击蒙太奇

UPROPERTY(BlueprintReadOnly, Category = "Animation State")
bool bIsSprintingAnim;        // 是否正在冲刺
```

## NativeUpdateAnimation 更新逻辑

```cpp
void UBattleAnimInstance::NativeUpdateAnimation(float DeltaSeconds) {
    Super::NativeUpdateAnimation(DeltaSeconds);
    
    APawn* PawnOwner = TryGetPawnOwner();
    if (!PawnOwner) return;
    
    ACharacter* Character = Cast<ACharacter>(PawnOwner);
    if (!Character) return;
    
    UCharacterMovementComponent* Movement = Character->GetCharacterMovement();
    
    Speed = Character->GetVelocity().Size();
    bIsInAir = Movement->IsFalling();
    bIsMoving = Speed > 3.0f;  // 3 单位死区，避免静止抖动
    
    ABattleCharacter* BattleChar = Cast<ABattleCharacter>(Character);
    if (BattleChar) {
        UAnimMontage* AttackMontage = BattleChar->GetAttackMontage();
        bIsAttacking = AttackMontage && 
            GetSkeletalMeshOwner()->GetAnimInstance()->Montage_IsPlaying(AttackMontage);
        
        UBattleCharacterMovementComponent* BattleMovement = 
            Cast<UBattleCharacterMovementComponent>(Movement);
        bIsSprintingAnim = BattleMovement && BattleMovement->IsSprinting();
    }
}
```

## 经验总结

1. **死区阈值 3.0f**：速度小于 3 视为静止，避免角色站立时因浮点误差导致动画抖动
2. **TryGetPawnOwner() 比 GetOwningActor() 更安全**：专门用于 AnimInstance 获取拥有者
3. **Montage_IsPlaying 检查防重复**：攻击输入时先检查蒙太奇是否已在播放
4. **C++ 计算 + 蓝图消费的分工**：C++ 负责数值计算和状态判断，蓝图负责状态机和混合空间
