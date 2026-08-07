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
// 动画蒙太奇
#include "Animation/AnimMontage.h"
// ASC 完整定义
#include "AbilitySystemComponent.h"
// HealthSet 完整定义 
#include "Attributes/HealthSet/HealthSet.h"
// UGameplayEffect 基类定义
#include "GameplayEffect.h"
// FClassFinder / FObjectFinder 工具
#include "UObject/ConstructorHelpers.h"
// GAS 蓝图函数库（AssignTagSetByCallerMagnitude 在这里）
#include "AbilitySystemBlueprintLibrary.h" 

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

    // 加载攻击输入动作
    static ConstructorHelpers::FObjectFinder<UInputAction> AttackAction_Finder(
        TEXT("/Game/MyResource/Input/IA_Attack.IA_Attack"));
    if (AttackAction_Finder.Succeeded())
    {
        AttackAction = AttackAction_Finder.Object;
    }

    // 加载攻击动画蒙太奇
    static ConstructorHelpers::FObjectFinder<UAnimMontage> AttackMontage_Finder(
        TEXT("/Game/MyResource/Animations/Manny/AM_Melee_Attack.AM_Melee_Attack"));
    if (AttackMontage_Finder.Succeeded())
    {
        AttackMontage = AttackMontage_Finder.Object;
    }

    // 加载动画蓝图类（AnimBlueprint 需要用 _C 后缀加载 GeneratedClass）
    static ConstructorHelpers::FObjectFinder<UClass> ABP_Finder(
        TEXT("/Game/MyResource/Animations/BP_BattleAnimInstance.BP_BattleAnimInstance_C"));
    if (ABP_Finder.Succeeded())
    {
        GetMesh()->SetAnimInstanceClass(ABP_Finder.Object);
    }

    // 获取自定义移动组件指针
    BattleMovement = Cast<UBattleCharacterMovementComponent>(GetCharacterMovement()); 

    // GAS 能力系统组件
    // 创建 ASC 组件
    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    // 设置复制模型
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

    // 创建 HealthSet（属性集"挂"在 ASC 上）
    HealthSet = CreateDefaultSubobject<UHealthSet>(TEXT("HealthSet")); 

    // 加载测试扣血输入动作
    static ConstructorHelpers::FObjectFinder<UInputAction> TestTakeDamageAction_Finder(
        TEXT("/Game/MyResource/Input/IA_TestDamage.IA_TestDamage"));
    if (TestTakeDamageAction_Finder.Succeeded())
    {
        TestTakeDamageAction = TestTakeDamageAction_Finder.Object;
    } 

    // 加载测试加血输入动作
    static ConstructorHelpers::FObjectFinder<UInputAction> TestHealAction_Finder(
        TEXT("/Game/MyResource/Input/IA_TestHeal.IA_TestHeal"));
    if (TestHealAction_Finder.Succeeded())
    {
        TestHealAction = TestHealAction_Finder.Object;
    }

    // 加载伤害 GE 蓝图类
    static ConstructorHelpers::FClassFinder<UGameplayEffect> DamageEffectFinder(
        TEXT("/Game/MyResource/GameplayEffects/GE_Damage"));
    if (DamageEffectFinder.Succeeded())
    {
        DamageEffectClass = DamageEffectFinder.Class;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[BattleCharacter] 加载 GE_Damage 失败！请检查路径：/Game/MyResource/GameplayEffects/GE_Damage"));
    }

    // 加载治疗 GE 蓝图类
    static ConstructorHelpers::FClassFinder<UGameplayEffect> HealEffectFinder(
        TEXT("/Game/MyResource/GameplayEffects/GE_Heal"));
    if (HealEffectFinder.Succeeded())
    {
        HealEffectClass = HealEffectFinder.Class;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[BattleCharacter] 加载 GE_Heal 失败！请检查路径：/Game/MyResource/GameplayEffects/GE_Heal"));
    }
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

    // ===== 初始化血量 ===== //
    if (AbilitySystemComponent && HealthSet)
    {
        // 初始化最大血量（先设上限）
        HealthSet->InitMaxHealth(100.0f);
        // 初始化当前血量
        HealthSet->InitHealth(100.0f);
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
        EnhancedInput->BindAction(AttackAction, ETriggerEvent::Started, this, &ABattleCharacter::Attack);

        // 绑定测试扣血输入动作（H键）
        EnhancedInput->BindAction(TestTakeDamageAction, ETriggerEvent::Started, this, &ABattleCharacter::TestTakeDamage);

        // 绑定测试加血输入动作（J键）
        EnhancedInput->BindAction(TestHealAction, ETriggerEvent::Started, this, &ABattleCharacter::TestHeal);
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

// 攻击
void ABattleCharacter::Attack()
{
    // 如果正在攻击或没有蒙太奇，直接返回
    if (bIsAttacking || !AttackMontage) return;

    // 获取动画实例
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (!AnimInstance) return;

    // 标记正在攻击
    bIsAttacking = true;

    // 播放攻击蒙太奇
    float Duration = AnimInstance->Montage_Play(AttackMontage, 1.0f);
    
    if (Duration > 0.0f)
    {
        // 绑定蒙太奇结束回调
        FOnMontageEnded EndDelegate;
        EndDelegate.BindUObject(this, &ABattleCharacter::OnAttackMontageEnded);
        AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackMontage);
    }
    else
    {
        // 播放失败，重置状态
        bIsAttacking = false;
    }
}

// 攻击蒙太奇播放结束回调
void ABattleCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    bIsAttacking = false;
}

// 获取 ASC（其他类（比如敌人）要攻击本角色时需要拿到它） 
UAbilitySystemComponent* ABattleCharacter::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

// 扣血（通过 GE 流程，而不是直改属性）
void ABattleCharacter::TakeDamage(float Amount)
{
    // 安全检查：ASC / HealthSet / GE 类三者缺一不可
    if (!AbilitySystemComponent || !HealthSet || !DamageEffectClass) return;

    // 1. 创建 GE 的 Spec (处方)： 指定用哪张GE，谁施加的（自己），以及效果强度（伤害数值）
    FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
        DamageEffectClass, // GE 类
        1.0f, // 等级
        AbilitySystemComponent->MakeEffectContext() // 上下文
    );

    // Spec 创建失败就退出
    if (!SpecHandle.IsValid()) return;

    // 2.在处方上填入具体的扣血数值 （Data.Damage Tag 对应 GE 里的 SetByCaller）
    UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
        SpecHandle,  // Spec
        FGameplayTag::RequestGameplayTag(FName("Data.Damage")), // Tag 名称要和 GE 里一致
        -Amount // 取反：外部传正数伤害量，这里转成负数给 GE（因为 GE 是 Add 操作）
    );

    // 3.把处方应用到自己身上 （触发完整 GE 流程）
    AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

    // 打印
    const float Current = HealthSet->GetHealth();
    const float Max     = HealthSet->GetMaxHealth();
    UE_LOG(LogTemp, Warning, TEXT("[扣血] -%.1f → 当前：%.1f / %.1f"), Amount, Current, Max);
}

// 加血
void ABattleCharacter::Heal(float Amount)
{
    // 安全检查：ASC / HealthSet / GE 类三者缺一不可
    if (!AbilitySystemComponent || !HealthSet || !HealEffectClass) return;

    // ====== 调试打印：应用前 ======
    const int32 EffectCountBefore = AbilitySystemComponent->GetActiveEffects().Num();
    UE_LOG(LogTemp, Warning, TEXT("[加血前] Health=%.1f / %.1f, 活跃效果数=%d"), HealthSet->GetHealth(), HealthSet->GetMaxHealth(), EffectCountBefore);

    // 1. 创建 GE 的 Spec (处方)： 指定用哪张GE，谁施加的（自己），以及效果强度（治疗数值）
    FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
        HealEffectClass, // GE 类
        1.0f, // 等级
        AbilitySystemComponent->MakeEffectContext() // 上下文
    );

    // Spec 创建失败就退出
    if (!SpecHandle.IsValid()) return;

    // 2.在处方上填入具体的治疗数值 （Data.Heal Tag 对应 GE 里的 SetByCaller）
    UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
        SpecHandle,  // Spec
        FGameplayTag::RequestGameplayTag(FName("Data.Heal")), // Tag 名称要和 GE 里一致
        Amount // 治疗数值
    );

    // 3.把处方应用到自己身上 （触发完整 GE 流程）
    AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

    // ====== 调试打印：应用后 ======
    const int32 EffectCountAfter = AbilitySystemComponent->GetActiveEffects().Num();
    UE_LOG(LogTemp, Warning, TEXT("[加血后] Health=%.1f / %.1f, 活跃效果数=%d"), HealthSet->GetHealth(), HealthSet->GetMaxHealth(), EffectCountAfter);
}

// 测试扣血
void ABattleCharacter::TestTakeDamage()
{
    TakeDamage(10.0f);
}

// 测试加血
void ABattleCharacter::TestHeal()
{
    Heal(10.0f);
}