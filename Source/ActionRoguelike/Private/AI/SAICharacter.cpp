// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "SAttributeComponent.h"


ASAICharacter::ASAICharacter()
{
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");

	AtrributeComp = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");
	
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	TimeToHitParamName = "TimeToHit";
}


void ASAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PawnSensingComp->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnSeen);
	AtrributeComp->OnHealthChanged.AddDynamic(this, &ASAICharacter::OnHealthChanged);
}


void ASAICharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth,	float Delta)
{
	if (Delta < 0.f)
	{
		if (InstigatorActor != this)
		{
			SetTargetActor(InstigatorActor);
		}

		GetMesh()->SetScalarParameterValueOnMaterials("HitPlayerTime", GetWorld()->TimeSeconds);
	
		if (NewHealth <= 0.f)
		{
			//	Stop BT
			AAIController* AIC = Cast<AAIController>(GetController());
			if (AIC)
			{
				AIC->GetBrainComponent()->StopLogic("Killed");
			}
			
			//	Ragdoll
			GetMesh()->SetAllBodiesSimulatePhysics(true);
			GetMesh()->SetCollisionProfileName("Ragdoll");

			//	Set lifespan
			SetLifeSpan(10.f);
		}
	}
}


void ASAICharacter::SetTargetActor(AActor* NewTarget)
{
	AAIController* AIC = Cast<AAIController>(GetController());

	if (AIC)
	{
		AIC->GetBlackboardComponent()->SetValueAsObject("TargetActor", NewTarget);
	}
}


void ASAICharacter::OnPawnSeen(APawn* Pawn)
{
	SetTargetActor(Pawn);

	DrawDebugString(GetWorld(), GetActorLocation(), "PLAYER SPOTTED", nullptr, FColor::White, 4.f, true);
}
