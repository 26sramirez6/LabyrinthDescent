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

template<uint16_t _x_dist_per_mob, uint16_t _y_dist_per_mob,
	uint16_t _count_per_zone_x, uint16_t _count_per_zone_y>
struct SpawnMobsInSingleZone {
	template<std::size_t _zone_id, typename ZoneBLP_WU>
	static inline void calc(ATopologyTracer const * _topology_tracer, UWorld * world, AMob ** mobs_ptr) {
		constexpr uint16_t _mob_start_idx = _zone_id * (_count_per_zone_x*_count_per_zone_y);
		uint16_t count = 0;
		for (uint16_t i = 1; i <= _count_per_zone_y; i++) {
			for (uint16_t j = 1; j <= _count_per_zone_x; j++) {
				const FVector _spawn_loc = {
					static_cast<float>(ZoneBLP_WU::x + j * _x_dist_per_mob),
					static_cast<float>(ZoneBLP_WU::y + i * _y_dist_per_mob),
					0.f
				};
				AMob * mob = world->SpawnActor<AMob>(_spawn_loc, FRotator::ZeroRotator);
				mobs_ptr[_mob_start_idx + count++] = mob;
				mob->initializeMob(_topology_tracer, _zone_id);
			}
		}
	}
};

template<uint16_t _count_per_zone_x, uint16_t _count_per_zone_y>
void AMobHandler::SpawnMobsAllZones() {
	UWorld * world = GetWorld();
	constexpr uint16_t _x_dist_per_mob = EnabledPathFinderConfig::ZoneDims_WU::x / (_count_per_zone_x + 1);
	constexpr uint16_t _y_dist_per_mob = EnabledPathFinderConfig::ZoneDims_WU::y / (_count_per_zone_y + 1);
	using Functor = SpawnMobsInSingleZone<_x_dist_per_mob, _y_dist_per_mob, _count_per_zone_x, _count_per_zone_y>;
	m_mobs = new AMob*[_count_per_zone_x * _count_per_zone_y * EnabledPathFinderConfig::zone_count];
	tuple_for_each_type<Functor, EnabledPathFinderConfig::TupleZoneBLP_WU>(m_topology_tracer, world, m_mobs);
}

template void AMobHandler::SpawnMobsAllZones<MOB_COUNT_PER_ZONE_X, MOB_COUNT_PER_ZONE_Y>();