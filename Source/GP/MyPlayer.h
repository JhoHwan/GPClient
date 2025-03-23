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

	UFUNCTION(BlueprintCallable)
	void SetPath(const TArray<FVector>& Path);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AcceptanceRadius = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveSpeed = 600.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsMoving = false;

private:
	void MoveAlongPath(float DeltaTime);

	UPROPERTY()
	TArray<FVector> PathPoints;

	int32 CurrentPathIndex = 0;

};
