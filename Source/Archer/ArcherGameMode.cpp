#include "ArcherGameMode.h"
#include "ArcherCharacter.h"
#include "UObject/ConstructorHelpers.h"

AArcherGameMode::AArcherGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Archer/BP_Player"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
