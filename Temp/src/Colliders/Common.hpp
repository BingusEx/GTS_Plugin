#pragma once



namespace GTS {

	

	RE::COL_LAYER GetCollidesWith(const std::uint32_t& collisionFilterInfo);
	RE::COL_LAYER GetCollidesWith(const hkpCollidable* collidable);
	RE::COL_LAYER GetCollidesWith(const hkpWorldObject* entity);

	void SetCollidesWith(std::uint32_t& collisionFilterInfo, const RE::COL_LAYER& newLayer);
	void SetCollidesWith(hkpCollidable* collidable, const RE::COL_LAYER& newLayer);
	void SetCollidesWith(hkpWorldObject* entity, const RE::COL_LAYER& newLayer);

	class ColliderData {
		public:
			void DisableCollisions();
			void EnableCollisions();

			void Activate();
			void UpdateCollisionFilter();

		protected:
			virtual std::vector<ColliderData*> GetChildren();
			std::vector<hkpWorldObject*> GetWorldObjects();
			std::vector<hkpRigidBody*> GetRigidBodies();
			std::vector<hkpPhantom*> GetPhantoms();

			void AddRB(hkpRigidBody* rb);
			void AddPhantom(hkpPhantom* phantom);

		private:
			std::unordered_map<hkpRigidBody*, hkRefPtr<hkpRigidBody> > rbs;
			std::unordered_map<hkpPhantom*, hkRefPtr<hkpPhantom> > phantoms;
	};
}
