#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "PlayerProfile.h"
#include "TinyTrialsGameState.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnGameReady);

UCLASS()
class ATinyTrialsGameState : public AGameStateBase
{
    GENERATED_BODY()

public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    // @TODO it would be better to make this a hashable container 
    UPROPERTY(ReplicatedUsing=OnRep_ConnectedPlayers, BlueprintReadOnly)
    TArray<FConnectedPlayerInfo> ConnectedPlayers;
    UPROPERTY(Replicated, BlueprintReadOnly, EditDefaultsOnly)
    int32 NumMinAuthorisedPlayers = 4;
    UFUNCTION()
    void OnRep_ConnectedPlayers();
    void ServerAddOrUpdatePlayer(const FConnectedPlayerInfo& Info);

    FOnGameReady OnGameReady;
    void OnSessionStarted(const int32& Seed);
    UPROPERTY(BlueprintReadOnly)
    int32 LevelSeed = 0;
    UPROPERTY(ReplicatedUsing=OnRep_MatchConfig, BlueprintReadOnly)
    bool bMatchConfigReady = false;
    UFUNCTION()
    void OnRep_MatchConfig();
};
