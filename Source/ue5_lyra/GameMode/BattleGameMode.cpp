// 游戏模型cpp
#include "BattleGameMode.h"
#include "Character/BattleCharacter.h"

ABattleGameMode::ABattleGameMode()
{
    // 后面创建完 BattleCharacter 后再设置 DefaultPawnClass
	DefaultPawnClass = ABattleCharacter::StaticClass();
}