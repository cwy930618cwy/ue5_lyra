// 游戏角色
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BattleCharacter.generated.h"

// 机械臂
class USpringArmComponent;
// 摄像机
class UCameraComponent;
// 输入映射
class UInputMappingContext;
// 输入动作
class UInputAction;

// 输入动作值
struct FInputActionValue;

UCLASS()
class UE5_LYRA_API ABattleCharacter : public ACharacter
{
    GENERATED_BODY()
    
public:
    ABattleCharacter();
protected:
    virtual void BeginPlay() override;
    // 设置玩家输入组件
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // 机械臂
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<USpringArmComponent> SpringArm;

    // 摄像机
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UCameraComponent> FollowCamera;

    // 输入映射
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputMappingContext> DefaultMappingContext;

    // 输入动作
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> LookAction;

    // 输入回调
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
};
