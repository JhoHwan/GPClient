// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"

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

    if (PathPoints.Num() > 0 && CurrentPathIndex < PathPoints.Num())
    {
        MoveAlongPath(DeltaTime);
    }
}

void AMyPlayer::SetPath(const TArray<FVector>& Path)
{
	if (Path.Num() == 0)
	{
		return;
	}

	bIsMoving = true;
	PathPoints = Path;
	CurrentPathIndex = 0;
}

void AMyPlayer::MoveAlongPath(float DeltaTime)
{
    FVector CurrentLocation = GetActorLocation();
    FVector TargetLocation = PathPoints[CurrentPathIndex];

    FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal2D();
    FVector NewLocation = CurrentLocation + Direction * MoveSpeed * DeltaTime;

    // 이동 (Root Motion이 없으면 SetActorLocation 사용 가능)
    SetActorLocation(NewLocation);

    // 회전
    FRotator TargetRotation = Direction.Rotation();
    SetActorRotation(TargetRotation);

    float DistanceToTarget = FVector::Dist2D(NewLocation, TargetLocation);

    if (DistanceToTarget <= AcceptanceRadius)
    {
        CurrentPathIndex++;

        if (CurrentPathIndex >= PathPoints.Num())
        {
			bIsMoving = false;
        }
    }
}


