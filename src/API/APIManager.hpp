#pragma once
#include "API/SmoothCamAPI.hpp"

namespace GTS {

	class APIManager {
		public:

		// Static method to access the singleton instance
		[[nodiscard]] static inline APIManager& GetSingleton() noexcept {
			static APIManager instance;
			return instance;
		}

		void Register();

		//Smoothcam
		void ReqControlFromSC();
		void RetControlToSC();
	
		[[nodiscard]] inline bool SmoothCamLoaded() {
			return SmoothCam != nullptr;
		}

		[[nodiscard]] inline bool SmoothCamEnabled() {
			auto Enable = SmoothCamLoaded() ? SmoothCam->IsCameraEnabled() : false;
			if (!Enable && Smoothcam_HaveCamera){
				RetControlToSC();
				Smoothcam_HaveCamera = false;
			}
			return Enable;
		}

		[[nodiscard]] inline bool SmoohthCamHaveCamera() {
			return Smoothcam_HaveCamera;
		}

		private:
		static inline SmoothCamAPI::IVSmoothCam3* SmoothCam = nullptr;
		bool Smoothcam_HaveCamera = false;
		bool Smoothcam_Enable = true;
	};
}