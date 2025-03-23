// Fill out your copyright notice in the Description page of Project Settings.


#include "GPGameInstance.h"
#include "Sockets.h"
#include "Common/TcpSocketBuilder.h"
#include "Serialization/ArrayWriter.h"
#include "SocketSubsystem.h"
#include "PacketSession.h"

#include "Protocol.pb.h"
#include "ClientPacketHandler.h"

void UGPGameInstance::ConnectToGame()
{
	if (Socket != nullptr) return;
	Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(TEXT("Stream"), TEXT("Client Socket"));

	FIPv4Address Ip;
	FIPv4Address::Parse(IpAddress, Ip);

	TSharedRef<FInternetAddr> InternetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	InternetAddr->SetIp(Ip.Value);
	InternetAddr->SetPort(Port);

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Connecting To Server...")));

	bool Connected = Socket->Connect(*InternetAddr);
	
	if (Connected)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Connection Success")));

		GameServerSession = MakeShared<PacketSession>(Socket);
		GameServerSession->Run();

		{
			Protocol::CS_ENTER_GAME Packet;
			SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(Packet);
			SendPakcet(SendBuffer);
		}

	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Connection Failed")));
	}
}

void UGPGameInstance::DisconnectFromGameServer()
{
	if (Socket)
	{
		ISocketSubsystem* SocketSubSystem = ISocketSubsystem::Get();
		SocketSubSystem->DestroySocket(Socket);
		Socket = nullptr;
	}


}

void UGPGameInstance::HandleRecvPackets()
{
	if (Socket == nullptr || GameServerSession == nullptr)
	{
		return;
	}

	GameServerSession->HandleRecvPackets();
}

void UGPGameInstance::SendPakcet(TSharedPtr<SendBuffer> SendBuffer)
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	GameServerSession->SendPacket(SendBuffer);
}

void UGPGameInstance::Shutdown()
{
	Super::Shutdown();

	Players.Empty();

	if (GameServerSession == nullptr) return;

	GameServerSession->Disconnect();

	DisconnectFromGameServer();
}

void UGPGameInstance::HandleSpawn(const Protocol::PlayerInfo& PlayerInfo)
{
	if (Socket == nullptr || GameServerSession == nullptr) return;

	auto* World = GetWorld();
	if (World == nullptr) return;

	const uint32 ObjectId = PlayerInfo.object_id();
	if (Players.Find(ObjectId) != nullptr)
		return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	FVector SpawnLocation(PlayerInfo.x(), PlayerInfo.y(), 140);

	AActor* Actor = World->SpawnActor<AActor>(PlayerClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
	if (Actor == nullptr) return;

	Players.Add(PlayerInfo.object_id(), Actor);
}

void UGPGameInstance::HandleSpawn(const Protocol::SC_ENTER_GAME& EnterGame)
{
	HandleSpawn(EnterGame.player());

	if (Players.Find(EnterGame.player().object_id()) == nullptr) return;
	AActor* player = Players[EnterGame.player().object_id()];
	
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC == nullptr) return;

	APawn* PawnToPossess = Cast<APawn>(player);
	if (PawnToPossess == nullptr) return;

	PC->Possess(PawnToPossess);
}

void UGPGameInstance::HandleSpawn(const Protocol::SC_SPAWN& SpawnGame)
{
}
