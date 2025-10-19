#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PlayerProfile.h"
#include "TinyTrialsPlayerState.generated.h"

UCLASS()
class ATinyTrialsPlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    UPROPERTY(Replicated, BlueprintReadOnly)
    FConnectedPlayerInfo MyProfile;

    UFUNCTION(Server, Reliable)
    void Server_SetUsernameAndController(const FString& InDisplayName, class ATinyTrialsPlayerController* INRef);
    UFUNCTION(Server, Reliable)
    void Server_ReceiveServerAuthorisation(const FString& INPlayerID, const FString& INJwt);
    void SetJwt_ServerInternal(const FString& InJwt);

    UFUNCTION()
    void OnRep_Jwt() {}

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    UPROPERTY(ReplicatedUsing=OnRep_Jwt, SaveGame)
    FString Jwt;
};
