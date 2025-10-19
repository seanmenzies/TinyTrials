// Copyright Epic Games, Inc. All Rights Reserved.

#include "TinyTrialsGameMode.h"
#include "TinyTrialsGameState.h"
#include "TinyTrialsCharacter.h"
#include "TinyTrialsPlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ATinyTrialsGameMode::ATinyTrialsGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
    bUseSeamlessTravel = true;
}

void ATinyTrialsGameMode::RequestJoinSession(ATinyTrialsPlayerController* NewPlayer)
{
    // keep a queue for new logins so we only process one at a time
    ProcessedControllersQueue.AddUnique(TWeakObjectPtr<ATinyTrialsPlayerController>(NewPlayer));
    if (ProcessedControllersQueue.Num() == 1) ProcessLogin(NewPlayer);
}

void ATinyTrialsGameMode::BeginPlay()
{
    Super::BeginPlay();

    InitGameState();
}

void ATinyTrialsGameMode::PostLogin(APlayerController* NewLogin)
{
	Super::PostLogin(NewLogin);

    if (!NewLogin->IsA<ATinyTrialsPlayerController>())
    {
        UE_LOG(LogTemp, Warning, TEXT("[Game Mode] New log in not of expected class."));
        return;
    }
}

void ATinyTrialsGameMode::InitGameState()
{
    if (!GS)
    {
        GS = GetGameState<ATinyTrialsGameState>();
        if (!GS)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("[Game Mode] Game State not initialised. Problem!"));
            return;
        }
        GS->OnGameReady.AddUObject(this, &ATinyTrialsGameMode::StartSession);
    }
}

void ATinyTrialsGameMode::ProcessLogin(ATinyTrialsPlayerController* NewLogin)
{
    // Build the payload
    auto Json = GenerateJsonObj(NewLogin);
    FString Body;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Body);
    FJsonSerializer::Serialize(Json, Writer);

    // Create request
    auto& Http = FHttpModule::Get();
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Req = Http.CreateRequest();
    BuildHttpRequest(Req, Body);

    Req->OnProcessRequestComplete().BindUObject(this, &ATinyTrialsGameMode::OnRequestComplete);
    Req->ProcessRequest();

    PutPlayerInPendingAuthState(NewLogin);
}

void ATinyTrialsGameMode::ManageLoginQueue()
{
    ProcessedControllersQueue.RemoveAt(0);
    if (ProcessedControllersQueue.IsEmpty()) return;
    // remove any stale ptrs
    ProcessedControllersQueue.RemoveAllSwap([](const TWeakObjectPtr<ATinyTrialsPlayerController>& P)
    {
        return !P.IsValid();
    });

    if (!ProcessedControllersQueue.IsEmpty()) ProcessLogin(ProcessedControllersQueue[0].Get());
}

TSharedRef<FJsonObject> ATinyTrialsGameMode::GenerateJsonObj(ATinyTrialsPlayerController* Controller) const
{
    int32 LocalIdx = Controller ? Controller->NetPlayerIndex : 0;
    const FString DeviceId = DeviceIdHelper::GetOrMakeStableDeviceId(LocalIdx);
    const FString DisplayName = Controller->GetPlayerDisplayName();

    TSharedRef<FJsonObject> Json = MakeShared<FJsonObject>();
    Json->SetStringField(TEXT("device_id"), DeviceId);
    Json->SetStringField(TEXT("display_name"), DisplayName);
    return Json;
}

void ATinyTrialsGameMode::BuildHttpRequest(TSharedRef<class IHttpRequest, ESPMode::ThreadSafe> Req, const FString& Body)
{
    // in reality, we'd make a getter/generator but for this project we just set some default base values
    FHttpProfile Profile;

    Req->SetURL(Profile.Url());
    Req->SetVerb(Profile.Verb);
    Req->SetHeader(TEXT("Content-Type"), Profile.ContentType);
    for (const auto& Kvp : Profile.Headers)
    {
        Req->SetHeader(Kvp.Key, Kvp.Value);
    }
    Req->SetTimeout(Profile.TimeoutSeconds);
    Req->SetContentAsString(Body);
}

void ATinyTrialsGameMode::OnRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded)
{
    if (ProcessedControllersQueue.IsEmpty()) return;

    if (!ProcessedControllersQueue[0].IsValid())
    {
        //HandleStaleLogin();
        bSucceeded = false;
    }
    if (!bSucceeded || !Response.IsValid())
    {
        bSucceeded = false;
        //HandleAuthFailure(ControllerBeingProcessed.Get(), TEXT("Network/No response"));
    }

    if (Response->GetResponseCode() != 200)
    {
        bSucceeded = false;
        //HandleAuthFailure(ControllerBeingProcessed.Get(), FString::Printf(TEXT("HTTP %d"), Response->GetResponseCode()));
    }

    if (!bSucceeded)
    {
        ManageLoginQueue();
        return;
    }

    auto ProcessingController = ProcessedControllersQueue[0].Get();

    // Parse JSON
    TSharedPtr<FJsonObject> JsonResp;
    const auto Payload = Response->GetContentAsString();
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Payload);
    if (!FJsonSerializer::Deserialize(Reader, JsonResp) || !JsonResp.IsValid())
    {
        //HandleAuthFailure(ProcessedControllersQueue.Get(), TEXT("Bad JSON"));
        return;
    }

    const FString PlayerId = JsonResp->GetStringField(TEXT("playerId"));
    const FString Jwt = JsonResp->GetStringField(TEXT("jwt"));

    InitializeAuthorizedPlayer(ProcessingController, PlayerId, Jwt);

    ManageLoginQueue();
}

void ATinyTrialsGameMode::PutPlayerInPendingAuthState(ATinyTrialsPlayerController* PendingController)
{
    PendingController->OnStartJoin();
}

void ATinyTrialsGameMode::InitializeAuthorizedPlayer(ATinyTrialsPlayerController* AuthController, const FString& ID, const FString& Token)
{
    AuthController->OnPlayerAuthorised(ID, Token);
    GS->ServerAddOrUpdatePlayer(AuthController->GetPlayerProfile());
}

void ATinyTrialsGameMode::StartSession()
{
    if (!bSessionStarted)
    {
        const int32 LevelSeed = FMath::Rand();
        const FString TravelURL = FString::Printf(TEXT("/Game/FirstPerson/Maps/FirstPersonMap.FirstPersonMap?seed=%d"), LevelSeed);
        GetWorld()->ServerTravel(TravelURL);
        bSessionStarted = true;
    }
}

