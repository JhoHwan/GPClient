// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GPGameInstance.h"
#include "ClientPacketHandler.h"
#include "Protocol.pb.h"

// Sets default values
AMyPlayer::AMyPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (bIsMoving == true)
    {
        MoveToTarget(DeltaTime);
    }
}

void AMyPlayer::RequestMove(FVector Location)
{
    UGPGameInstance* GameInstance = Cast<UGPGameInstance>(GetGameInstance());

    Protocol::CS_REQUEST_MOVE pkt;
    pkt.set_playerid(PlayerId);
    pkt.set_x(Location.X);
    pkt.set_y(Location.Y);

    GameInstance->SendPakcet(ClientPacketHandler::MakeSendBuffer(pkt));
}

void AMyPlayer::SetTargetLocation(const Protocol::MoveInfo& info)
{
	MoveInfo = info;
	if (State == Protocol::PLAYER_STATE::PLAYER_STATE_IDLE)
	{
		State = Protocol::PLAYER_STATE::PLAYER_STATE_MOVE;
		bIsMoving = true;
	}
}

void AMyPlayer::MoveToTarget(float DeltaTime)
{
    FVector CurrentLocation = GetActorLocation();
    
	FVector TargetLocation = FVector(MoveInfo.objectinfo().x(), MoveInfo.objectinfo().y(), CurrentLocation.Z);

    FVector MoveDirection = (TargetLocation - CurrentLocation);
    float DistanceToTarget = MoveDirection.Size2D();
    MoveDirection = MoveDirection.GetSafeNormal2D();

    float MoveDistance = (MoveDirection * MoveSpeed * DeltaTime).Size2D();
    MoveDistance = FMath::Min(MoveDistance, DistanceToTarget);

    FVector NewLocation = CurrentLocation + MoveDirection * MoveDistance;

    // 이동
    SetActorLocation(NewLocation);

    // 회전
    FRotator TargetRotation = MoveDirection.Rotation();
    SetActorRotation(TargetRotation);

    if (DistanceToTarget <= AcceptanceRadius)
    {
		if (MoveInfo.state() == Protocol::PLAYER_STATE::PLAYER_STATE_IDLE)
		{
			State = Protocol::PLAYER_STATE::PLAYER_STATE_IDLE;
			bIsMoving = false;
		}
    }
}