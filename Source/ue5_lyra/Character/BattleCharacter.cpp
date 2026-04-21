// 游戏角色cpp
#include "BattleCharacter.h"

// 弹簧臂
#include "GameFramework/SpringArmComponent.h"
// 摄像机
#include "Camera/CameraComponent.h"
// 角色移动组件
#include "GameFramework/CharacterMovementComponent.h"
// 增强输入组件
#include "EnhancedInputComponent.h"
// 增强输入子系统
#include "EnhancedInputSubSystem.h"

ABattleCharacter::ABattleCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // 创建弹簧臂组件
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent); // 将弹簧臂组件附加到根组件
    SpringArm->TargetArmLength = 400.0f; // 设置弹簧臂长度
    SpringArm->bUsePawnControlRotation = true; // 使用角色控制旋转

    // 创建摄像机组件
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName); // 将摄像机组件附加到弹簧臂组件
    FollowCamera->bUsePawnControlRotation = false; // 不使用角色控制旋转

    // 创建角色移动组件
    GetCharacterMovement()->bOrientRotationToMovement = true; // 角色朝移动方向转身
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // 旋转速度
    GetCharacterMovement()->MaxWalkSpeed = 500.0f; // 最大行走速度

    // 不让控制器旋转角色（由移动组件来控制）
    bUseControllerRotationYaw = false;
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;
}

// 角色开始游戏
void ABattleCharacter::BeginPlay()
{
    Super::BeginPlay();

    // 注册输入映射上下文（Enhanced Input）
	if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }
}

// 设置玩家输入组件
void ABattleCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // 绑定输入动作
        EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABattleCharacter::Move);
        EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABattleCharacter::Look);
    }
}

// 处理移动输入
void ABattleCharacter::Move(const FInputActionValue& Value)
{
    // 获取输入的 2D 向量（WASD → X/Y）
	const FVector2D MoveVector = Value.Get<FVector2D>();

    // 根据控制器朝向计算移动方向
    const FRotator Rotation = Controller->GetControlRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0); // 只考虑 Yaw 旋转

    const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X); // 前向
    const FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);  // 右向

    // 计算移动方向
    AddMovementInput(Forward, MoveVector.Y);  // 前后移动
    AddMovementInput(Right, MoveVector.X);  // 左右移动
}

// 处理视角输入
void ABattleCharacter::Look(const FInputActionValue& Value)
{
    // 获取输入的 2D 向量（MouseX/MouseY → X/Y）
    const FVector2D LookVector = Value.Get<FVector2D>();

    // 设置视角旋转
    AddControllerYawInput(LookVector.X);
    AddControllerPitchInput(LookVector.Y);
}