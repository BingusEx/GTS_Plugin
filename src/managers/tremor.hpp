#pragma once
// Module that handles footsteps



using namespace std;
using namespace SKSE;


namespace GTS {
	class TremorManager : public EventListener {
		public:
			[[nodiscard]] static TremorManager& GetSingleton() noexcept;

			virtual std::string DebugName() override;
			virtual void OnImpact(const Impact& impact) override;
	};
}
