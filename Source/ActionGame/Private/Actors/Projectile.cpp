// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Projectile.h"

#include "ActionGameStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

static TAutoConsoleVariable<int32> CVarShowProjectiles(
	TEXT("ShowDebugProjectiles"),
	0,
	TEXT("Draw debug info about projectiles")
	TEXT(" 0: off/n")
	TEXT(" 1: on/n"),
	ECVF_Cheat);

// Sets default values
AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	SetReplicateMovement(true);
	bReplicates = true;

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
	ProjectileMovementComponent->Velocity = FVector::ZeroVector; 
	ProjectileMovementComponent->OnProjectileStop.AddDynamic(this, &AProjectile::OnProjectileStop);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(GetRootComponent());
	StaticMeshComponent->SetIsReplicated(true);
	StaticMeshComponent->SetCollisionProfileName(TEXT("Projectile"));
	StaticMeshComponent->bReceivesDecals = false;
}

const UProjectileStaticData* AProjectile::GetProjectileStaticData() const
{
	if (IsValid(ProjectileDataClass))
	{
		return GetDefault<UProjectileStaticData>(ProjectileDataClass);
	}
	return nullptr;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	const UProjectileStaticData* ProjectileData = GetProjectileStaticData();

	if (ProjectileData && ProjectileMovementComponent)
	{
		if (ProjectileData->StaticMesh)
		{
			StaticMeshComponent->SetStaticMesh(ProjectileData->StaticMesh);
		}

		ProjectileMovementComponent->bInitialVelocityInLocalSpace = false;
		ProjectileMovementComponent->InitialSpeed = ProjectileData->InitialSpeed;
		ProjectileMovementComponent->MaxSpeed = ProjectileData->MaxSpeed;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->bShouldBounce = false;
		ProjectileMovementComponent->Bounciness = 0.f;
		ProjectileMovementComponent->ProjectileGravityScale = ProjectileData->GravityMultiplayer;
		ProjectileMovementComponent->Velocity = ProjectileData->InitialSpeed * GetActorForwardVector();
	}

	const int32 DebugShowProjectile = CVarShowProjectiles.GetValueOnAnyThread();

	if (DebugShowProjectile)
	{
		DebugDrawPath();
	}
}

void AProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	const UProjectileStaticData* ProjectileData = GetProjectileStaticData();
	if (ProjectileData)
	{
		UGameplayStatics::PlaySoundAtLocation(this,ProjectileData->OnStopSfX, GetActorLocation(), 1.f);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ProjectileData->OnStopVfX, GetActorLocation());
	}
	Super::EndPlay(EndPlayReason);
}

void AProjectile::DebugDrawPath() const
{
	const UProjectileStaticData* ProjectileData = GetProjectileStaticData();
	if (ProjectileData)
	{
		FPredictProjectilePathParams PredictProjectilePathParams;
		PredictProjectilePathParams.StartLocation = GetActorLocation();
		PredictProjectilePathParams.LaunchVelocity = ProjectileData->InitialSpeed * GetActorForwardVector();
		PredictProjectilePathParams.TraceChannel = ECollisionChannel::ECC_Visibility;
		PredictProjectilePathParams.bTraceComplex = true;
		PredictProjectilePathParams.bTraceWithChannel = true;
		PredictProjectilePathParams.DrawDebugType = EDrawDebugTrace::ForDuration;
		PredictProjectilePathParams.DrawDebugTime = 3.f;
		PredictProjectilePathParams.OverrideGravityZ = ProjectileData->GravityMultiplayer == 0.f ? 0.0001f : ProjectileData->GravityMultiplayer;

		FPredictProjectilePathResult PredictProjectilePathResult;
		if (UGameplayStatics::PredictProjectilePath(this, PredictProjectilePathParams, PredictProjectilePathResult))
		{
			DrawDebugSphere(GetWorld(), PredictProjectilePathResult.HitResult.Location, 50, 10, FColor::Red);
		}
	}
}

void AProjectile::OnProjectileStop(const FHitResult& InpactResult)
{
	const UProjectileStaticData* ProjectileData = GetProjectileStaticData();
	if (ProjectileData)
	{
		UActionGameStatics::ApplyRadialDamage(this, GetOwner(), GetActorLocation(),
		ProjectileData->DamageRadius,
		ProjectileData->BaseDamage,
		(ProjectileData->Effects),
		ProjectileData->RadialDamageQueryTypes,
		ProjectileData->RadialDamageTraceType);
	}
	Destroy();
}

void AProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AProjectile, ProjectileDataClass);
}


