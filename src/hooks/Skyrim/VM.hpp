#pragma once
#include "Hooks/Hooks.hpp"




namespace Hooks
{
	
	
	using namespace RE::BSScript;
	using namespace RE::BSScript::Internal;
	
	class Hook_VM
	{
		public:
			static void Hook();
		private:
			static void SendEvent(VirtualMachine* a_this, VMHandle a_handle, const BSFixedString& a_eventName, IFunctionArguments* a_args);
			static inline REL::Relocation<decltype(SendEvent)> _SendEvent;
	};
}
