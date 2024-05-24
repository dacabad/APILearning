// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSubsystem.h"

#include "Online/OnlineSessionNames.h"

void UMultiplayerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

bool UMultiplayerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer);
}

void UMultiplayerSubsystem::Login()
{
	const uint32 LocalId = GetLocalPlayer()->GetControllerId();
	
	const IOnlineIdentityPtr Identity = Online::GetIdentityInterface(GetWorld());
	const FUniqueNetIdPtr UniqueId = Identity->GetUniquePlayerId(LocalId);
	
	if(UniqueId && Identity->GetLoginStatus(LocalId) == ELoginStatus::LoggedIn)
		return;

	static FDelegateHandle Handle;
	
	Handle = Identity->AddOnLoginCompleteDelegate_Handle(LocalId,
		FOnLoginCompleteDelegate::CreateLambda([this, Identity](int32 InLocalUserId, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Err)
		{
			if(bWasSuccessful)
			{
				UE_LOG(LogTemp, Display, TEXT("User with UniqueID %s logged in"), *UserId.ToString())
				Identity->ClearOnLoginCompleteDelegate_Handle(InLocalUserId, Handle);
				Handle.Reset();
			}
			else
			{
				UE_LOG(LogTemp, Display, TEXT("Login failed misserably"))
			}
		}));

	bool bLoginAttemptSucceeded = false;
	
	FString AuthType; 
	FParse::Value(FCommandLine::Get(), TEXT("AUTH_TYPE="), AuthType);
	if(!AuthType.IsEmpty())
	{
		bLoginAttemptSucceeded = Identity->AutoLogin(LocalId);
	}
	else
	{
		FOnlineAccountCredentials AccountCredentials(TEXT("accountportal"), TEXT(""), TEXT(""));
		bLoginAttemptSucceeded = Identity->Login(LocalId, AccountCredentials);
	}
	
	if(!bLoginAttemptSucceeded)
	{
		Identity->ClearOnLoginCompleteDelegate_Handle(LocalId, Handle);
		Handle.Reset();
	}
}

void UMultiplayerSubsystem::CreateSession(int32 NumPublicConnections, const FName& MatchType)
{
	if(const IOnlineSessionPtr SessionPtr = Online::GetSessionInterface(GetWorld()))
	{
		if (SessionPtr->IsPlayerInSession(NAME_GameSession, *GetLocalPlayer()->GetPreferredUniqueNetId()))
		{
			bCreateSessionOnDestroy = true;
			LastNumPublicConnections = NumPublicConnections;
			LastMatchType = MatchType;
			DestroySession();
		}	
	}
	
	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	LastSessionSettings->bIsLANMatch = IsLanSession();
	LastSessionSettings->NumPublicConnections = NumPublicConnections;
	LastSessionSettings->bAllowJoinInProgress = true;
	LastSessionSettings->bShouldAdvertise = true;
	LastSessionSettings->bUsesPresence = true;
	LastSessionSettings->bAllowJoinViaPresence = true;
	LastSessionSettings->Set(FName(TEXT("MatchType")), MatchType.ToString(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	
	static FDelegateHandle Handle;
	const IOnlineSessionPtr SessionPtr = Online::GetSessionInterface(GetWorld());
	
	Handle = SessionPtr->AddOnCreateSessionCompleteDelegate_Handle(
		FOnCreateSessionCompleteDelegate::CreateLambda([this, SessionPtr](const FName& SessionName, bool bSessionWasCreated)
		{
			SessionPtr->ClearOnCreateSessionCompleteDelegate_Handle(Handle);
			OnSessionCreated.Broadcast();
		}));
	
	const bool bWasSuccessful = SessionPtr->CreateSession(*GetLocalPlayer()->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings);
	if (!bWasSuccessful)
	{
		SessionPtr->ClearOnCreateSessionCompleteDelegate_Handle(Handle);		
	}
}

void UMultiplayerSubsystem::FindSession(int32 MaxSearchResults)
{
	LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	LastSessionSearch->MaxSearchResults = MaxSearchResults;
	LastSessionSearch->bIsLanQuery = IsLanSession();
	LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	
	static FDelegateHandle Handle;

	const IOnlineSessionPtr SessionPtr = Online::GetSessionInterface(GetWorld());
	
	Handle = SessionPtr->AddOnFindSessionsCompleteDelegate_Handle(
		FOnFindSessionsCompleteDelegate::CreateLambda([this, SessionPtr](bool bWasSuccessful)
		{
			SessionPtr->ClearOnFindSessionsCompleteDelegate_Handle(Handle);
			OnSessionFound.Broadcast();
			
			const FOnlineSession FoundSession = LastSessionSearch->SearchResults[0].Session; 
					
			FString SettingsVal;
			FoundSession.SessionSettings.Get(FName(TEXT("MatchType")), SettingsVal);
			
			JoinSession(LastSessionSearch->SearchResults[0]);
		}));
		
	if (!SessionPtr->FindSessions(*GetLocalPlayer()->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
	{
		SessionPtr->ClearOnFindSessionsCompleteDelegate_Handle(Handle);
	}
}

void UMultiplayerSubsystem::JoinSession(const FOnlineSessionSearchResult& SearchResult) const
{
	static FDelegateHandle Handle;

	const IOnlineSessionPtr SessionPtr = Online::GetSessionInterface(GetWorld());

	Handle = SessionPtr->AddOnJoinSessionCompleteDelegate_Handle(
		FOnJoinSessionCompleteDelegate::CreateLambda([this, SessionPtr](const FName SessionName, EOnJoinSessionCompleteResult::Type JoinSessionInfo)
		{
			SessionPtr->ClearOnJoinSessionCompleteDelegate_Handle(Handle);
			OnSessionJoined.Broadcast();
			
			FString Address;
			SessionPtr->GetResolvedConnectString(NAME_GameSession, Address);
			
			if(APlayerController* PC = GetLocalPlayer()->GetPlayerController(GetWorld()))
			{
				PC->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
			}
		}));
	
	if(!SessionPtr->JoinSession(*GetLocalPlayer()->GetPreferredUniqueNetId(), NAME_GameSession, SearchResult))
	{
		SessionPtr->ClearOnJoinSessionCompleteDelegate_Handle(Handle);
	}
}

void UMultiplayerSubsystem::DestroySession() const
{
	const IOnlineSessionPtr SessionPtr = Online::GetSessionInterface(GetWorld());

	static FDelegateHandle Handle;
	Handle = SessionPtr->AddOnDestroySessionCompleteDelegate_Handle(
		FOnDestroySessionCompleteDelegate::CreateLambda([this, SessionPtr](const FName& SessionName, bool bWasSuccessful)
		{
			SessionPtr->ClearOnDestroySessionCompleteDelegate_Handle(Handle);
			OnSessionDestroyed.Broadcast();
		}));

	
	if(!SessionPtr->DestroySession(NAME_GameSession))
	{
		
	}
}

void UMultiplayerSubsystem::ServerTravel(const TSoftObjectPtr<UWorld>& WorldSoftRef)
{
	if(WorldSoftRef.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to enter world; empty reference"))
		return;
	}
	
	const FString FullPath = FString::Printf(TEXT("%s.%s?listen"), *WorldSoftRef.GetLongPackageName(), *WorldSoftRef.GetAssetName());
	if(!GetWorld() || !GetWorld()->ServerTravel(FullPath))
	{
		UE_LOG(LogTemp, Error, TEXT("Server travel failed!!"))
	}
}

void UMultiplayerSubsystem::UnlockAchievement(const FName& AchievementName)
{
	const uint32 LocalId = GetLocalPlayer()->GetControllerId();
	
	const IOnlineIdentityPtr IdentityInterface = Online::GetIdentityInterface(GetWorld());
	const FUniqueNetIdPtr UniqueId = IdentityInterface->GetUniquePlayerId(LocalId);
	
	if(UniqueId || IdentityInterface->GetLoginStatus(LocalId) != ELoginStatus::LoggedIn)
		return;

	const IOnlineAchievementsPtr AchievementsInterface = Online::GetAchievementsInterface(GetWorld());

	static FDelegateHandle Handle;
	if(!Handle.IsValid())
	{
		Handle = AchievementsInterface->AddOnAchievementUnlockedDelegate_Handle(FOnAchievementUnlockedDelegate::CreateLambda(
			[this, AchievementsInterface](const FUniqueNetId& UniqueId, const FString& AchievementName)
		{
			UE_LOG(LogTemp, Display, TEXT("Unlocked: %s"), *AchievementName)
		}));
	}
	
	AchievementsInterface->QueryAchievements(*UniqueId,
		FOnQueryAchievementsCompleteDelegate::CreateLambda([this, AchievementsInterface, AchievementName](const FUniqueNetId& UniqueId, const bool bSuccesfulQuery)
		{
			AchievementsInterface->GetCachedAchievements(UniqueId, QueriedAchievements);
			if(QueriedAchievements.ContainsByPredicate([AchievementName](const FOnlineAchievement& InAchievement)
			{
				return AchievementName == InAchievement.Id;
			}))
			{
				FOnlineAchievementsWriteRef AchievementsWriteRef = MakeShared<FOnlineAchievementsWrite, ESPMode::ThreadSafe>();
				AchievementsWriteRef.Get().Properties.Add(AchievementName);
				
				// AchievementsInterface->WriteAchievements(UniqueId, AchievementsWriteRef);
			}
			else
			{
				UE_LOG(LogTemp, Display, TEXT("Could not find achievement : %s"), *AchievementName.ToString())
			}
		}));
}

// void UMultiplayerSubsystem::UnlockAchievementWithCallback(const FName& AchievementName, FOnLocalAchievementUnlocked& OnAchievementUnlockedEvent)
// {
// }


