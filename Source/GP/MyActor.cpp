// Fill out your copyright notice in the Description page of Project Settings.


#include "MyActor.h"
#include "Protocol.pb.h"
#include "GPGameInstance.h"
#include "ClientPacketHandler.h"

// Sets default values
AMyActor::AMyActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Protocol::CS_CHAT Pkt;
	Pkt.set_msg("Hello Server");
	auto SendBuffer = ClientPacketHandler::MakeSendBuffer(Pkt);
	Cast<UGPGameInstance>(GetGameInstance())->SendPakcet(SendBuffer);
	
}

