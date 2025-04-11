// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Network\Protocol.pb.h"

#include "GPPlayer.generated.h"

UCLASS()
class GP_API AGPPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGPPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE void SetPlayerId(uint64 Id) { PlayerId = Id; }

	UFUNCTION(BlueprintCallable)
	void RequestMove(FVector Location);

	UFUNCTION(BlueprintCallable)
	void RequestStop();

	void SetTargetLocation(const Protocol::MoveInfo& info);

private:
	void MoveToTarget(float DeltaTime);


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AcceptanceRadius = 0.01f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RotateInterpSpeed = 10.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsMoving = false;

private:
	uint64 PlayerId;
	FVector TargetLocation;
	Protocol::PLAYER_STATE State = Protocol::PLAYER_STATE::PLAYER_STATE_IDLE;
	Protocol::MoveInfo MoveInfo;

	FRotator TargetRotation;
};