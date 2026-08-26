# 为什么需要 Experience 系统

## 一个场景引入

假设你要做一款射击游戏，策划说：

> "我们要三种模式：团队竞技、个人竞技、占点模式。每种模式的计分规则不同、UI 不同、甚至玩家用的角色也不同。"

**传统做法**：写三个 GameMode 子类——`ATeamGameMode`、`APersonalGameMode`、`ACapturePointGameMode`。每个子类里重复写大量相似的代码，只是细节不同。

```cpp
// ❌ 传统做法：每个模式一个类，代码大量重复
class ATeamGameMode : public AGameModeBase {
    // 队伍计分逻辑...
    // 队伍 UI 生成...
    // 但 90% 的代码和其他模式一样
};

class APersonalGameMode : public AGameModeBase {
    // 个人计分逻辑...
    // 个人 UI 生成...
    // 又重复了一遍
};
```

**Lyra 做法**：只写一套 C++ 代码（`ALyraGameMode`），然后创建不同的**数据资产实例**来配置差异。

```cpp
// ✅ Lyra 做法：一套代码 + 不同配置 = 不同模式
// 团队竞技 → 创建一个 ExperienceDefinition 实例，勾选"Teams"插件
// 个人竞技 → 创建另一个实例，不勾"Teams"，换一套 PawnData
// 占点模式 → 再创建一个实例，加"CapturePoint"Action
```

---

## 核心思想

> **"组合优于继承"** —— 用数据资产配置（Composition）替代类继承（Inheritance）。

| 维度 | 传统做法 | Lyra 做法 |
|------|----------|-----------|
| 新增模式 | 写新的 GameMode 子类 | 复制一份 ExperienceDefinition，改配置 |
| 代码复用 | 靠继承，容易写出深层继承链 | 靠组合（Action + ActionSet），平铺直叙 |
| 谁来做 | 程序员 | 策划（编辑器里拖拽配置） |
| 运行时切换 | 不支持（类是编译时确定的） | 支持（WorldSettings 可指定不同 Experience） |
| 测试 | 每个子类单独测 | 同一套代码，只测配置组合 |

---

## 相关源码

> 📂 代码位置：`../代码/` 文件夹（所有子章节共享同一份源码）

### 文件总览

| 文件 | 角色 | 说明 |
|------|------|------|
| `LyraExperienceDefinition.h` | 数据资产声明 | 定义配置表有哪些字段 |
| `LyraExperienceDefinition.cpp` | 数据资产实现 | 验证配置是否合法 |
| `LyraGameMode.h` | GameMode 声明 | 读取 Experience 决定 Pawn 类型 |
| `LyraGameMode.cpp` | GameMode 实现 | 加载流程 + Pawn 生成 |

---

### 1. `LyraExperienceDefinition.h` — 配置表的"骨架"

**继承链**：`UObject → UDataAsset → UPrimaryDataAsset → ULyraExperienceDefinition`

`UPrimaryDataAsset` 意味着它是一个可以在编辑器中右键创建的独立资产（和蓝图一样有生命周期）。

#### 关键字段

| 行号 | 字段 | 类型 | 说明 |
|------|------|------|------|
| 第 65~66 行 | `GameFeaturesToEnable` | `TArray<FString>` | 要启用的 GameFeature 插件列表（如 `["ShooterCore", "Teams"]`） |
| 第 74~75 行 | `DefaultPawnData` | `TObjectPtr<const ULyraPawnData>` | 默认 Pawn 数据（决定玩家用什么角色） |
| 第 83~84 行 | `Actions` | `TArray<TObjectPtr<UGameFeatureAction>>` | 内联 Action 列表（Instanced + EditInlineNew，专属本 Experience） |
| 第 90~91 行 | `ActionSets` | `TArray<TObjectPtr<ULyraExperienceActionSet>>` | 引用的外部 ActionSet（可复用组合包） |

#### 完整头文件

```cpp
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "LyraExperienceDefinition.generated.h"

// 前向声明：告诉编译器这些类存在，但不需要完整定义（避免循环引用）
class UGameFeatureAction;        // GameFeature 动作基类
class ULyraPawnData;             // Pawn 数据资产（定义角色配置）
class ULyraExperienceActionSet;  // Action 集合（可复用的 Action 组合）

/**
 * 体验定义（Experience Definition）
 * 
 * 一份"游戏模式配置文件"，策划在编辑器里创建实例、填好参数，就定义了一种玩法。
 * 
 * 三个关键字段：
 * - GameFeaturesToEnable: 要启用的插件列表
 * - DefaultPawnData: 玩家用什么角色
 * - Actions/ActionSets: 要执行的逻辑
 */
UCLASS(BlueprintType, Const)  // BlueprintType=蓝图可见, Const=运行时不可修改
class ULyraExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	ULyraExperienceDefinition();

	//~UObject interface
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	//~End of UObject interface

	//~UPrimaryDataAsset interface
#ifdef WITH_EDITORONLY_DATA
	virtual void UpdateAssetBundleData() override;
#endif
	//~End of UPrimaryDataAsset interface

public:
	// ① 要启用的 GameFeature 插件列表
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Experience")
	TArray<FString> GameFeaturesToEnable;

	// ② 默认 Pawn 数据
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Experience")
	TObjectPtr<const ULyraPawnData> DefaultPawnData;

	// ③ 内联 Action 列表（Instanced=每个实例独立, EditInlineNew=直接在详情面板添加子对象）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, EditInlineNew, Category = "Lyra|Experience")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;

	// ④ 引用的 ActionSet（可复用的外部 Action 组合包）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Experience")
	TArray<TObjectPtr<ULyraExperienceActionSet>> ActionSets;
};
```

---

### 2. `LyraExperienceDefinition.cpp` — 配置验证

这个文件做了两件重要的事：

1. **数据验证**（`IsDataValid`）：确保配置不出错——Action 不能为空、不允许"蓝图的蓝图"继承
2. **资源收集**（`UpdateAssetBundleData`）：告诉引擎这个 Experience 需要哪些资源

```cpp
// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraExperienceDefinition.h"
#include "LyraExperienceActionSet.h"
#include "GameFeatureAction.h"
#include "LyraLogChannels.h"
#include "Misc/DataValidation.h"

#define LOCTEXT_NAMESPACE "LyraExperienceDefinition"

ULyraExperienceDefinition::ULyraExperienceDefinition()
{
	// 空实现，所有配置通过编辑器 Details 面板完成
}

#if WITH_EDITOR

EDataValidationResult ULyraExperienceDefinition::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(
		Super::IsDataValid(Context), EDataValidationResult::Valid);

	int32 ActionIndex = 0;
	for (const TObjectPtr<UGameFeatureAction>& Action : Actions)
	{
		// 检查 1：Action 不能为空指针
		if (Action == nullptr)
		{
			Context.AddError(FText::Format(
				LOCTEXT("NullAction", "Actions[{0}] is null"),
				FText::AsNumber(ActionIndex)));
			Result = EDataValidationResult::Invalid;
		}
		else
		{
			// 检查 2：递归验证每个 Action 的子对象
			Result = CombineDataValidationResults(Result, Action->IsDataValid(Context));
		}

		// 检查 3：禁止"蓝图的蓝图"继承——应用组合而非继承
		if (!Action->GetClass()->IsNative())
		{
			const UClass* SuperClass = Action->GetClass()->GetSuperClass();
			if ((SuperClass != nullptr) && !SuperClass->IsNative())
			{
				Context.AddError(FText::Format(
					LOCTEXT("NonNativeParent",
						"Actions[{0}] has a non-native parent class '{1}'. "
						"Use ActionSet composition instead of BP inheritance."),
					FText::AsNumber(ActionIndex),
					SuperClass->GetDisplayName()));
				Result = EDataValidationResult::Invalid;
			}
		}

		ActionIndex++;
	}

	return Result;
}

#endif // WITH_EDITOR

#ifdef WITH_EDITORONLY_DATA

void ULyraExperienceDefinition::UpdateAssetBundleData()
{
	Super::UpdateAssetBundleData();

	// 遍历所有 Action，收集额外资源需求
	for (const TObjectPtr<UGameFeatureAction>& Action : Actions)
	{
		if (Action != nullptr)
		{
			Action->AddGameFeatureResources(*this);
		}
	}
}

#endif // WITH_EDITORONLY_DATA

#undef LOCTEXT_NAMESPACE
```

**验证逻辑要点**：
- 递归验证：不仅检查 Action 本身，还检查 Action 内部的子对象
- 禁止"蓝图的蓝图"：如果 Action 是蓝图类，且它的父类也是蓝图类，报错。原因是要鼓励用 ActionSet 组合而非蓝图继承链

---

### 3. `LyraGameMode.h` — 读取 Experience 的 GameMode

**核心职责**：
1. 游戏启动时确定使用哪个 Experience
2. Experience 加载完成后为在场玩家生成 Pawn
3. 动态决定每个玩家的 Pawn 类型（由 Experience 配置决定）

#### 关键方法

| 方法 | 职责 |
|------|------|
| `InitGame()` | 游戏初始化入口，触发 Experience 加载 |
| `HandleMatchAssignmentIfNotExpectingOne()` | 按优先级确定用哪个 Experience |
| `OnExperienceLoaded()` | Experience 加载完成回调，为在场玩家生成 Pawn |
| `GetPawnDataForController()` | 从 Experience 中查找 Controller 对应的 PawnData |
| `GetDefaultPawnClassForController_Implementation()` | 返回 Pawn 的类（由 Experience 配置决定） |

```cpp
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ModularGameMode.h"
#include "LyraGameMode.generated.h"

class ULyraExperienceDefinition;
class ULyraPawnData;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnLyraExperienceLoaded,
	const ULyraExperienceDefinition* /*Experience*/);

/**
 * ALyraGameMode
 * 
 * 核心职责：
 * 1. 游戏启动时确定使用哪个 Experience
 * 2. Experience 加载完成后为在场玩家生成 Pawn
 * 3. 动态决定每个玩家的 Pawn 类型
 */
UCLASS(Config = Game)
class ALyraGameMode : public AModularGameMode
{
	GENERATED_BODY()

public:
	ALyraGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~AGameModeBase interface
	virtual void InitGame(const FURL& URL, UGameInstance* GameInstance) override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	//~End of AGameModeBase interface

	// Experience 加载完成的委托
	FOnLyraExperienceLoaded OnExperienceLoadedDelegate;

	// 获取当前加载的 Experience
	const ULyraExperienceDefinition* GetCurrentExperience() const { return CurrentExperience; }

protected:
	// 处理 Match 分配（确定用哪个 Experience）
	void HandleMatchAssignmentIfNotExpectingOne();

	// Experience 加载完成回调
	void OnExperienceLoaded(const ULyraExperienceDefinition* Experience);

	// 从 Experience 中查找 Controller 对应的 PawnData
	ULyraPawnData* GetPawnDataForController(AController* InController) const;

	// 当前加载的 Experience
	UPROPERTY(Transient)
	TObjectPtr<const ULyraExperienceDefinition> CurrentExperience;
};
```

---

### 4. `LyraGameMode.cpp` — 核心流程实现

#### 4.1 构造函数 — 注册 ExperienceManagerComponent

```cpp
ALyraGameMode::ALyraGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 不在这里创建组件，GameState 上已经有 ExperienceManagerComponent
}
```

#### 4.2 `InitGame()` — 触发 Experience 加载

```cpp
void ALyraGameMode::InitGame(const FURL& URL, UGameInstance* GameInstance)
{
	Super::InitGame(URL, GameInstance);

	// 游戏初始化时，确定并加载 Experience
	HandleMatchAssignmentIfNotExpectingOne();
}
```

#### 4.3 `HandleMatchAssignmentIfNotExpectingOne()` — 确定用哪个 Experience

按优先级查找：
1. WorldSettings 里配置的 Experience（关卡级别）
2. 命令行参数指定的 Experience
3. 项目设置里的默认 Experience

```cpp
void ALyraGameMode::HandleMatchAssignmentIfNotExpectingOne()
{
	// 如果已经在加载中或已加载，跳过
	if (CurrentExperience)
	{
		return;
	}

	// 从 GameState 上获取 ExperienceManagerComponent
	if (ALyraGameState* GameState = GetGameState<ALyraGameState>())
	{
		if (ULyraExperienceManagerComponent* Component = GameState->FindComponentByClass<ULyraExperienceManagerComponent>())
		{
			// 委托给 Component 异步加载 Experience
			Component->SetCurrentExperience(/* 从 WorldSettings 等来源获取 */ ...);
		}
	}
}
```

#### 4.4 `OnExperienceLoaded()` — 加载完成后为在场玩家生成 Pawn

```cpp
void ALyraGameMode::OnExperienceLoaded(const ULyraExperienceDefinition* Experience)
{
	CurrentExperience = Experience;

	// 广播委托（其他系统可以监听）
	OnExperienceLoadedDelegate.Broadcast(Experience);

	// 为所有已经存在的玩家生成 Pawn
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && !PC->GetPawn())
		{
			// 根据 Experience 配置生成 Pawn
			SpawnDefaultPawnAtTransform(PC, PC->GetSpawnLocation());
		}
	}
}
```

#### 4.5 `GetDefaultPawnClassForController_Implementation()` — 动态决定 Pawn 类型

```cpp
UClass* ALyraGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	// 从 Experience 配置中查找 PawnData
	if (const ULyraPawnData* PawnData = GetPawnDataForController(InController))
	{
		if (PawnData->PawnClass)
		{
			// 用 Experience 配置的 Pawn 类型！
			return PawnData->PawnClass;
		}
	}

	// 回退到父类默认行为
	return Super::GetDefaultPawnClassForController_Implementation(InController);
}
```

#### 4.6 `GetPawnDataForController()` — 查找链

```cpp
ULyraPawnData* ALyraGameMode::GetPawnDataForController(AController* InController) const
{
	// 优先用 Experience 配置的 DefaultPawnData
	if (CurrentExperience && CurrentExperience->DefaultPawnData)
	{
		return const_cast<ULyraPawnData*>(CurrentExperience->DefaultPawnData.Get());
	}

	// TODO: 后续可以通过 PlayerState 上的 PawnDataComponent 覆盖
	return nullptr;
}
```

---

## 完整数据流

```
┌─────────────────────────────────────────────────────────────┐
│  游戏启动                                                    │
│     │                                                       │
│     ▼                                                       │
│  InitGame()                                                 │
│     │  触发 Experience 加载                                  │
│     ▼                                                       │
│  HandleMatchAssignmentIfNotExpectingOne()                   │
│     │  按优先级确定 Experience：                             │
│     │  WorldSettings > 命令行 > 项目设置                     │
│     ▼                                                       │
│  ExperienceManagerComponent.SetCurrentExperience()          │
│     │  异步加载 GameFeature 插件 + 资源                      │
│     ▼                                                       │
│  OnExperienceLoaded()                                       │
│     │  广播委托 + 为在场玩家生成 Pawn                        │
│     ▼                                                       │
│  GetDefaultPawnClassForController()                         │
│     │  从 Experience.DefaultPawnData 获取 Pawn 类型          │
│     ▼                                                       │
│  SpawnDefaultPawnAtTransform()                              │
│     │  生成玩家角色                                          │
│     ▼                                                       │
│  玩家拥有角色！                                              │
└─────────────────────────────────────────────────────────────┘
```

---

## 一句话总结

> **"同一套代码 + 不同配置 = 不同游戏模式"**

这就是 Experience 系统的核心价值。不需要为每个游戏模式写单独的 C++ 类，只需要创建不同的 `ExperienceDefinition` 数据资产实例即可。
