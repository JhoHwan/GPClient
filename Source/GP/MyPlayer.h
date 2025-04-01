// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyPlayer.generated.h"

UCLASS()
class GP_API AMyPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE void SetPlayerId(uint64 Id) { PlayerId = Id; }

	UFUNCTION(BlueprintCallable)
	void RequestMove(FVector Location);

	void SetTargetLocation(FVector Location);

private:
	void MoveToTarget(float DeltaTime);


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AcceptanceRadius = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveSpeed = 600.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsMoving = false;

private:
	UPROPERTY()
	FVector TargetLocation;

	uint64 PlayerId;
};
