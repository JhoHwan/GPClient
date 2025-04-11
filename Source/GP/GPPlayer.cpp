// Fill out your copyright notice in the Description page of Project Settings.


#include "GPPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GPGameInstance.h"
#include "ClientPacketHandler.h"
#include "Protocol.pb.h"

// Sets default values
AGPPlayer::AGPPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGPPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGPPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (bIsMoving == true)
    {
        MoveToTarget(DeltaTime);
    }
}

void AGPPlayer::RequestMove(FVector Location)
{
    UGPGameInstance* GameInstance = Cast<UGPGameInstance>(GetGameInstance());

    Protocol::CS_REQUEST_MOVE pkt;
    pkt.set_playerid(PlayerId);
    pkt.set_x(Location.X);
    pkt.set_y(Location.Y);
    pkt.set_z(Location.Z);

    GameInstance->SendPakcet(ClientPacketHandler::MakeSendBuffer(pkt));
}

void AGPPlayer::RequestStop()
{
    UGPGameInstance* GameInstance = Cast<UGPGameInstance>(GetGameInstance());

    Protocol::CS_REQUEST_STOP pkt;
	pkt.set_playerid(PlayerId);
	GameInstance->SendPakcet(ClientPacketHandler::MakeSendBuffer(pkt));
}

void AGPPlayer::SetTargetLocation(const Protocol::MoveInfo& info)
{
	MoveInfo.CopyFrom(info);
    TargetLocation = FVector(MoveInfo.objectinfo().x(), MoveInfo.objectinfo().y(), MoveInfo.objectinfo().z() + 88);

    double Yaw = MoveInfo.objectinfo().rotate();
    TargetRotation = GetActorRotation();
    TargetRotation.Yaw = MoveInfo.objectinfo().rotate();

    switch (MoveInfo.state())
    {
    case Protocol::PLAYER_STATE::PLAYER_STATE_IDLE:

        bIsMoving = false;
        SetActorLocation(TargetLocation);
        break;

    case Protocol::PLAYER_STATE::PLAYER_STATE_MOVE:
        bIsMoving = true;
        break;
    }
}

void AGPPlayer::MoveToTarget(float DeltaTime)
{
    FVector CurrentLocation = GetActorLocation();
    
    float Dist = FVector::Dist(CurrentLocation, TargetLocation);
    if (Dist <= AcceptanceRadius)
    {
        return;
    }

    // 회전 보정
    FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, RotateInterpSpeed);
    SetActorRotation(NewRotation);

    FVector MoveDirection = (TargetLocation - CurrentLocation);
    float DistanceToTarget = MoveDirection.Size();
    MoveDirection = MoveDirection.GetSafeNormal();

    float MoveDistance = (MoveDirection * MoveSpeed * DeltaTime).Size();
    MoveDistance = FMath::Min(MoveDistance, DistanceToTarget);

    FVector NewLocation = CurrentLocation + MoveDirection * MoveDistance;

    // 이동
    SetActorLocation(NewLocation);
}
