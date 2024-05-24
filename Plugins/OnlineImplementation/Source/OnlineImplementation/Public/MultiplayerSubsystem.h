// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineAchievementsInterface.h"
#include "OnlineSubsystemTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MultiplayerSubsystem.generated.h"


struct FOnlineAchievement;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSessionReady);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFindSession);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnJoinSession);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDestroySession);

/**
 * 
 */
UCLASS()
class ONLINEIMPLEMENTATION_API UMultiplayerSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
	
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;
		
	bool bCreateSessionOnDestroy = false;
	int32 LastNumPublicConnections;
	FName LastMatchType;

	TArray<FOnlineAchievement> QueriedAchievements;

protected:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	
public:

	UPROPERTY(BlueprintAssignable)	FOnSessionReady OnSessionCreated;
	UPROPERTY(BlueprintAssignable)	FOnFindSession OnSessionFound;
	UPROPERTY(BlueprintAssignable)	FOnJoinSession OnSessionJoined;
	UPROPERTY(BlueprintAssignable)	FOnDestroySession OnSessionDestroyed;

	UFUNCTION(BlueprintCallable) void Login();
	
	UFUNCTION(BlueprintCallable)	
	void CreateSession(int32 NumPublicConnections, const FName& MatchType);
	
	UFUNCTION(BlueprintCallable)
	void FindSession(int32 MaxSearchResults);
	
	void JoinSession(const FOnlineSessionSearchResult& SearchResult) const;
	void DestroySession() const;

	UFUNCTION(BlueprintCallable) void ServerTravel(const TSoftObjectPtr<UWorld>& WorldSoftRef);

	DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnLocalAchievementUnlocked, uint32, UniqueId, const FName&, AchievementId);
	
	UFUNCTION(BlueprintCallable) void UnlockAchievement(const FName& AchievementName);
	// UFUNCTION(BlueprintCallable) void UnlockAchievementWithCallback(const FName& AchievementName, FOnLocalAchievementUnlocked& OnAchievementUnlockedEvent);
	
		
protected:
	
	FORCEINLINE bool IsLanSession() const
	{
		return IOnlineSubsystem::Get()->GetSubsystemName() == TEXT("NULL");
	}

	FORCEINLINE ULocalPlayer* GetFirstLocalPlayer() const
	{
		return GetWorld()->GetFirstLocalPlayerFromController();
	}
};
