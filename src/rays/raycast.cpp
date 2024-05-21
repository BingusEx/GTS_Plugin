#include "UI/DebugAPI.hpp"
#include "rays/raycast.hpp"
#include "rays/allcollector.hpp"

using namespace Gts;
using namespace RE;

namespace {

	std::vector<string> Layers {
		"kInvalid",
		"kSphere",
		"kFirstType",
		"kCylinder",
		"kTriangle",
		"kBox",
		"kCapsule",
		"kConvexVertices",
		"kCollection",
		"kBVTree",
		"kList",
		"kMOPP",
		"kConvexTranslate",
		"kConvexTransform",
		"kSampledHeightField",
		"kExtendedMesh",
		"kTransform",
		"kCompressedMesh",
		"kCompound",
		"kTotalSPU",
		"kConvex",
		"kMOPPEmbedded",
		"kConvexPiece",
		"kMultiSphere",
		"kConvexList",
		"kTriangleCollection",
		"kMultiRay",
		"kHeightField",
		"kSphereRep",
		"kBV",
		"kPlane",
		"kPhantomCallback",
		"kUser0",
		"kUser1",
		"kUser2",
		"kTotal",
		"kAll",
	};
	

	void CastRayImpl(TESObjectREFR* ref, const NiPoint3& in_origin, const NiPoint3& direction, const float& unit_length, AllRayCollector* collector) {
		float length = unit_to_meter(unit_length);
		if (!ref) {
			return;
		}
		auto cell = ref->GetParentCell();
		if (!cell) {
			return;
		}
		auto collision_world = cell->GetbhkWorld();
		if (!collision_world) {
			return;
		}
		bhkPickData pick_data;

		NiPoint3 origin = unit_to_meter(in_origin);
		pick_data.rayInput.from = origin;

		NiPoint3 normed = direction / direction.Length();
		NiPoint3 end = origin + normed * length;
		pick_data.rayInput.to = end;

		NiPoint3 delta = end - origin;
		pick_data.ray = delta; // Length in each axis to travel

		pick_data.rayInput.enableShapeCollectionFilter = false; // Don't bother testing child shapes
		pick_data.rayInput.filterInfo = collector->filterInfo;

		pick_data.rayHitCollectorA8 = collector;

		collision_world->PickObject(pick_data);

		for (auto& ray_result: collector->GetHits()) {
			ray_result.position = meter_to_unit(origin + normed * length * ray_result.hitFraction);
		}
		std::ranges::sort(collector->GetHits(), [](const AllRayCollectorOutput &a, const AllRayCollectorOutput &b)
		{
			return a.hitFraction < b.hitFraction;
		});
	}
}

namespace Gts {

	NiPoint3 CastRay(TESObjectREFR* ref, const NiPoint3& origin, const NiPoint3& direction, const float& length, bool& success) {
		auto collector = AllRayCollector::Create();
		collector->Reset();
		collector->filterInfo = bhkCollisionFilter::GetSingleton()->GetNewSystemGroup() << 16 | stl::to_underlying(COL_LAYER::kLOS);
		CastRayImpl(ref, origin, direction, length, collector.get());

		if (collector->HasHit()) {
			for (auto& hit: collector->GetHits()) {
				// This varient just returns the first result
				success = true;
				return hit.position;
			}
		}

		success = false;
		return NiPoint3();
	}

	NiPoint3 CastRayStatics(TESObjectREFR* ref, const NiPoint3& origin, const NiPoint3& direction, const float& length, bool& success) {
		auto collector = AllRayCollector::Create();
		collector->Reset();
		collector->filterInfo = bhkCollisionFilter::GetSingleton()->GetNewSystemGroup() << 16 | stl::to_underlying(COL_LAYER::kLOS);
		CastRayImpl(ref, origin, direction, length, collector.get());

		if (collector->HasHit()) {
			for (auto& hit: collector->GetHits()) {
				// This varient filters out the char ones
				
				auto collision_layer = static_cast<COL_LAYER>(hit.rootCollidable->broadPhaseHandle.collisionFilterInfo & 0x7F);
				int layer_as_int = static_cast<int>(collision_layer);

				if (collision_layer != COL_LAYER::kCharController && collision_layer != COL_LAYER::kWeapon && 
					layer_as_int != 56) {
					/*if (ref->formID == 0x14) {
						log::info("------Hitting Layer: {}, as int: {}", collision_layer, layer_as_int); // Weapons hit "unknown" layer :/
					}*/
					success = true;
					return hit.position;
				}
			}
		}

		success = false;
		return NiPoint3();
	}
}
