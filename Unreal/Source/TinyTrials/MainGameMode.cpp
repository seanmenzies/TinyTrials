
#include "MainGameMode.h"
#include "TinyTrialsGameState.h"
#include "Kismet/GameplayStatics.h"

// MainGameMode.cpp
void AMainGameMode::InitGame(const FString& Map, const FString& Options, FString& Error)
{
    Super::InitGame(Map, Options, Error);

    const FString SeedStr = UGameplayStatics::ParseOption(Options, TEXT("seed"));
    const int32 LevelSeed = SeedStr.IsEmpty() ? 0 : FCString::Atoi(*SeedStr);
    if (auto GS = GetGameState<ATinyTrialsGameState>())
    {
        GS->OnSessionStarted(LevelSeed);
    }
}

// Carried-over players with seamless travel:
void AMainGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
    Super::HandleSeamlessTravelPlayer(C);

    if (auto PC = Cast<APlayerController>(C)) InitPlayerAfterJoin(PC);
}

// Late joiners:
void AMainGameMode::PostLogin(APlayerController* NewPC)
{
    Super::PostLogin(NewPC);

    InitPlayerAfterJoin(NewPC);
}

void AMainGameMode::InitPlayerAfterJoin(APlayerController* PC)
{
}
