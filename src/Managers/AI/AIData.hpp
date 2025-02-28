#pragma once

namespace GTS {

	class AIData {
		public:

		AIData(Actor* a_Actor);

		private:

		ActorHandle TargetActor;
		bool ActorsAreDead = false;
		bool IsThighSandwiching = false;

	};

}