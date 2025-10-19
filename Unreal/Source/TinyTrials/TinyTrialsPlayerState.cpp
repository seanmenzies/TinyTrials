#include "TinyTrialsPlayerState.h"
#include "TinyTrialsPlayerController.h"
#include "Net/UnrealNetwork.h"

void ATinyTrialsPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ATinyTrialsPlayerState, MyProfile);
    DOREPLIFETIME_CONDITION(ATinyTrialsPlayerState, Jwt, COND_OwnerOnly);
}

void ATinyTrialsPlayerState::Server_SetUsernameAndController_Implementation(const FString& InDisplayName, ATinyTrialsPlayerController* INRef)
{
    MyProfile.DisplayName = InDisplayName.Left(32).TrimStartAndEnd(); // basic sanitize: better to have shared rules
    MyProfile.PlayerRef = TWeakObjectPtr<ATinyTrialsPlayerController>(INRef);
}

void ATinyTrialsPlayerState::Server_ReceiveServerAuthorisation_Implementation(const FString& INPlayerID, const FString& INJwt)
{
    MyProfile.PlayerId = INPlayerID;
    SetJwt_ServerInternal(INJwt);
}

void ATinyTrialsPlayerState::SetJwt_ServerInternal(const FString& InJwt)
{
    check(HasAuthority());
    Jwt = InJwt;
}
