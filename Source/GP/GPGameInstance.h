// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Network/Protocol.pb.h"

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GP.h"
#include "GPGameInstance.generated.h"
/**
 * 
 */
UCLASS()
class GP_API UGPGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void ConnectToGame();

	UFUNCTION(BlueprintCallable)
	void DisconnectFromGameServer();

	UFUNCTION(BlueprintCallable)
	void HandleRecvPackets();

	void SendPakcet(TSharedPtr<SendBuffer> SendBuffer);

	virtual void Shutdown() override;

public:
	void HandleSpawn(const Protocol::PlayerInfo& PlayerInfo);
	void HandleSpawn(const Protocol::SC_ENTER_GAME& EnterGame);
	void HandleSpawn(const Protocol::SC_SPAWN& SpawnGame);

public:
	class FSocket* Socket;
	FString IpAddress = TEXT("127.0.0.1");
	int16 Port = 7777;

	TSharedPtr<class PacketSession> GameServerSession;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> PlayerClass;

	TMap<uint32, AActor*> Players;
};
