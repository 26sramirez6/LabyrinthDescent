// Fill out your copyright notice in the Description page of Project Settings.

#include "MobHandler.h"

AMobHandler::AMobHandler() {
	PrimaryActorTick.bCanEverTick = true;

}

void AMobHandler::BeginPlay() {
	Super::BeginPlay();
	
}

void AMobHandler::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

template<uint16_t _count_per_zone_x, uint16_t _count_per_zone_y>
void AMobHandler::SpawnMobsAllZones() {
	UWorld * world = GetWorld();
	constexpr uint16_t _x_dist_per_mob = ATopologyTracer::Tracer::ZoneDims_WU::x / (_count_per_zone_x + 1);
	constexpr uint16_t _y_dist_per_mob = ATopologyTracer::Tracer::ZoneDims_WU::y / (_count_per_zone_y + 1);
	
	//int y = ATopologyTracer::Tracer::GetZoneBLP_WU::value<2>::x;
	//tuple_for_each(ATopologyTracer::Tracer::TGetZoneBLP_WU, );
	for (uint8_t z = 0; z < ATopologyTracer::Tracer::zone_count; z++) {
		for (uint16_t i = 1; i <= _count_per_zone_y; i++) {
			for (uint16_t j = 1; j <= _count_per_zone_x; j++) {
				const FVector _spawn_loc {
				std::tuple_element<0, ATopologyTracer::Tracer::TupleZoneBLP_WU>::type::x + i * _x_dist_per_mob,
				std::tuple_element<0, ATopologyTracer::Tracer::TupleZoneBLP_WU>::type::y + j * _y_dist_per_mob,
				0 };

				world->SpawnActor<AMob>(_spawn_loc, FRotator::ZeroRotator);
			}
		}
	}
}

//template<uint8_t _zone_id, uint16_t _count_x, uint16_t _count_y>
//constexpr void AMobHandler::SpawnMobSingleZone() {
//	ATopologyTracer::Tracer::GetZoneBLP_WU<_zone_id>::value
//}
