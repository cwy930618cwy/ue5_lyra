// 游戏角色cpp
#include "BattleCharacter.h"

// 弹簧臂
#include "GameFramework/SpringArmComponent.h"
// 摄像机
#include "Camera/CameraComponent.h"
// 角色移动组件
#include "GameFramework/CharacterMovementComponent.h"
// 输入映射
#include "InputMappingContext.h"
// 输入动作
#include "InputAction.h"
// 增强输入组件
#include "EnhancedInputComponent.h"
// 增强输入子系统
#include "EnhancedInputSubsystems.h"
// 自定义移动组件
#include "Movement/BattleCharacterMovementComponent.h"

ABattleCharacter::ABattleCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UBattleCharacterMovementComponent>(
        ACharacter::CharacterMovementComponentName))
{
    PrimaryActorTick.bCanEverTick = true;

    // 绑定骨骼网格体（人物模型）
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(
        TEXT("/Game/MyResource/Meshes/SKM_Manny.SKM_Manny"));
    if (MeshAsset.Succeeded())
    {
        GetMesh()->SetSkeletalMesh(MeshAsset.Object); // 设置骨骼网格体
        GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));  // 调整位置，让模型脚踩地面
        GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f)); // 调整旋转，让模型面朝前方
    }

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

    // 不让控制器旋转角色（由移动组件来控制）
    bUseControllerRotationYaw = false;
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;

    // 加载输入映射上下文
    static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC_Finder(
        TEXT("/Game/MyResource/Input/IMC_Default.IMC_Default"));
    if (IMC_Finder.Succeeded())
    {
        DefaultMappingContext = IMC_Finder.Object;
    }

    // 加载移动输入动作
    static ConstructorHelpers::FObjectFinder<UInputAction> MoveAction_Finder(
        TEXT("/Game/MyResource/Input/IA_Move.IA_Move"));
    if (MoveAction_Finder.Succeeded())
    {
        MoveAction = MoveAction_Finder.Object;
    }

    // 加载视角输入动作
    static ConstructorHelpers::FObjectFinder<UInputAction> LookAction_Finder(
        TEXT("/Game/MyResource/Input/IA_Look.IA_Look"));
    if (LookAction_Finder.Succeeded())
    {
        LookAction = LookAction_Finder.Object;
    } 

    // 加载跳跃输入动作
    static ConstructorHelpers::FObjectFinder<UInputAction> JumpAction_Finder(
        TEXT("/Game/MyResource/Input/IA_Jump.IA_Jump"));
    if (JumpAction_Finder.Succeeded())
    {
        JumpAction = JumpAction_Finder.Object;
    }

    // 加载冲刺输入动作
    static ConstructorHelpers::FObjectFinder<UInputAction> SprintAction_Finder(
        TEXT("/Game/MyResource/Input/IA_Sprint.IA_Sprint"));
    if (SprintAction_Finder.Succeeded())
    {
        SprintAction = SprintAction_Finder.Object;
    }

    // 获取自定义移动组件指针
    BattleMovement = Cast<UBattleCharacterMovementComponent>(GetCharacterMovement()); 
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
        EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &ABattleCharacter::StartJump);
        EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &ABattleCharacter::StopJump);
        EnhancedInput->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ABattleCharacter::StartSprint);
        EnhancedInput->BindAction(SprintAction, ETriggerEvent::Completed, this, &ABattleCharacter::StopSprint);
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

// 开始跳跃
void ABattleCharacter::StartJump()
{
    Jump();
}

// 停止跳跃
void ABattleCharacter::StopJump()
{
    StopJumping();
}

// 开始冲刺
void ABattleCharacter::StartSprint()
{
    if (BattleMovement)
    {
        BattleMovement->StartSprint();
    }
}

// 停止冲刺
void ABattleCharacter::StopSprint()
{
    if (BattleMovement)
    {
        BattleMovement->StopSprint();
    }
} 