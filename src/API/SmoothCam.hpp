#pragma once

#include "API/Impl/SmoothCamAPI.hpp"

namespace GTS {

	class SmoothCam final {

		public:
		static void Register();
		static void RequestConrol();
		static void ReturnControl();
		static bool Enabled();
		static bool HaveCamera();

		[[nodiscard]] static bool Loaded() {
			return SmoothCamAPI != nullptr;
		}

	private:

		static inline SmoothCamAPI::IVSmoothCam3* SmoothCamAPI = nullptr;

	};


}