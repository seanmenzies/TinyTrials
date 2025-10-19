#include "TinyTrialsGameState.h"
#include "Net/UnrealNetwork.h"

void ATinyTrialsGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ATinyTrialsGameState, ConnectedPlayers);
    DOREPLIFETIME(ATinyTrialsGameState, NumMinAuthorisedPlayers);
    DOREPLIFETIME(ATinyTrialsGameState, LevelSeed);
    DOREPLIFETIME(ATinyTrialsGameState, bMatchConfigReady);
}

void ATinyTrialsGameState::OnRep_ConnectedPlayers()
{
    // UI notification using profile ref
}

void ATinyTrialsGameState::ServerAddOrUpdatePlayer(const FConnectedPlayerInfo& Info)
{
    ensure(HasAuthority());

    int32 Found = ConnectedPlayers.IndexOfByPredicate([&](const FConnectedPlayerInfo& E)
    {
        return E.PlayerId == Info.PlayerId && !Info.PlayerId.IsEmpty();
    });
    if (Found != INDEX_NONE)
    {
        ConnectedPlayers[Found] = Info;
    }
    else
    {
        ConnectedPlayers.Add(Info);
    }
    OnRep_ConnectedPlayers();

    if (!bMatchConfigReady)
    {
        if (ConnectedPlayers.Num() >= NumMinAuthorisedPlayers) OnGameReady.Broadcast();
    }
    else
    {

    }
}

void ATinyTrialsGameState::OnSessionStarted(const int32& Seed)
{
    LevelSeed = Seed;
    bMatchConfigReady = true;
    ForceNetUpdate();
}

void ATinyTrialsGameState::OnRep_MatchConfig()
{
    // UI notification using profile ref
}
