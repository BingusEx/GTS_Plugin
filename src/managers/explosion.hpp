#pragma once
// Module that handles footsteps



using namespace std;
using namespace SKSE;


namespace GTS {
	class ExplosionManager : public EventListener {
		public:
			[[nodiscard]] static ExplosionManager& GetSingleton() noexcept;

			virtual std::string DebugName() override;
			virtual void OnImpact(const Impact& impact) override;
	};
}
