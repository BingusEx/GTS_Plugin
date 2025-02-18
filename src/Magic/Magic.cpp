#include "Magic/Magic.hpp"

#include "Magic/Effects/Common.hpp"
#include "Magic/Effects/absorb_effect.hpp"
#include "Magic/Effects/GrowthSpurt.hpp"
#include "Magic/Effects/Enchantments/EnchGigantism.hpp"
#include "Magic/Effects/grow_other.hpp"
#include "Magic/Effects/growth.hpp"
#include "Magic/Effects/shrink.hpp"
#include "Magic/Effects/restore_size.hpp"
#include "Magic/Effects/restore_size_other.hpp"
#include "Magic/Effects/shrink_rune.hpp"
#include "Magic/Effects/shrink_cloak.hpp"
#include "Magic/Effects/shrink_foe.hpp"
#include "Magic/Effects/shrink_other.hpp"
#include "Magic/Effects/slow_grow.hpp"
#include "Magic/Effects/sword_of_size.hpp"
#include "Magic/Effects/TinyCalamity.hpp"
#include "Magic/Effects/Potions/MightPotion.hpp"
#include "Magic/Effects/Potions/GrowthPotion.hpp"
#include "Magic/Effects/Potions/MaxSizePotion.hpp"
#include "Magic/Effects/Potions/SizeHunger.hpp"
#include "Magic/Effects/Potions/EssencePotion.hpp"
#include "Magic/Effects/Potions/ExperiencePotion.hpp"
#include "Magic/Effects/Poisons/Poison_Of_Shrinking.hpp"
#include "Magic/Effects/Potions/ShrinkResistPotion.hpp"
#include "Magic/Effects/Potions/ShrinkPotion.hpp"


namespace GTS {

	void Magic::OnStart() {}

	void Magic::OnUpdate() {}

	void Magic::OnFinish() {}

	std::string Magic::GetName() {
		return "";
	}

	Magic::Magic(ActiveEffect* effect) : activeEffect(effect) {
		if (this->activeEffect) {
			auto spell = this->activeEffect->spell;
			this->effectSetting = this->activeEffect->GetBaseObject();
			MagicTarget* m_target = this->activeEffect->target;
			if (m_target) {
				if (m_target->MagicTargetIsActor()) {
					this->target = skyrim_cast<Actor*>(m_target);
				}
			}
			if (this->activeEffect->caster) {
				this->caster = this->activeEffect->caster.get().get();
			}
			this->hasDuration = this->HasDuration();
		}
	}

	bool Magic::HasDuration()  {
		if (this->activeEffect) {
			auto spell = this->activeEffect->spell;
			if (spell) {
				// switch (this->activeEffect->spell->GetSpellType()) {
				// 	case  MagicSystem::SpellType::kEnchantment: {
				// 		return false;
				// 	}
				// }
				switch (spell->GetCastingType()) {
					case  MagicSystem::CastingType::kConstantEffect: {
						return false;
					}
				}
			}
		}
		auto effectSetting = this->effectSetting;
		if (effectSetting) {
			if (effectSetting->data.flags.all(EffectSetting::EffectSettingData::Flag::kNoDuration)) {
				return false;
			}
		}
		return true;
	}

	void Magic::poll() {
		switch (this->state) {
			case State::Init:
			{
				this->dual_casted = this->IsDualCasting();

				this->state = State::Start;
				break;
			}
			case State::Start: {
				this->OnStart();
				this->state = State::Update;
				break;
			}
			case State::Update: {
				if (this->activeEffect->flags & ActiveEffect::Flag::kInactive) {
					break;
				}
				this->OnUpdate();
				if ((this->activeEffect->flags & ActiveEffect::Flag::kDispelled)
				    || (this->hasDuration && (this->activeEffect->elapsedSeconds >= this->activeEffect->duration))) {
					this->state = State::Finish;
				}
				break;
			}
			case State::Finish: {
				this->OnFinish();
				this->state = State::CleanUp;
				break;
			}
			case State::CleanUp: {
				break;
			}
		}
	}

	Actor* Magic::GetTarget() {
		return this->target;
	}

	Actor* Magic::GetCaster() {
		return this->caster;
	}

	ActiveEffect* Magic::GetActiveEffect() {
		return this->activeEffect;
	}

	EffectSetting* Magic::GetBaseEffect() {
		return this->effectSetting;
	}

	void Magic::Dispel() {
		if (this->activeEffect) {
			this->activeEffect->Dispel(false); // Not forced
			// Seems to be CTD prone for some reason, best to not use it
		}
	}

	bool Magic::IsDualCasting() {
		if (this->caster) {
			auto casting_type = GetActiveEffect()->castingSource;
			if (casting_type == MagicSystem::CastingSource::kLeftHand || casting_type == MagicSystem::CastingSource::kRightHand) {
				auto source = this->caster->GetMagicCaster(casting_type);
				if (source) {
					return source->GetIsDualCasting();
				}
			}
		}
		return false;
	}

	MagicManager& MagicManager::GetSingleton() noexcept {
		static MagicManager instance;
		return instance;
	}

	void MagicManager::ProcessActiveEffects(Actor* actor) {
		auto effect_list = actor->AsMagicTarget()->GetActiveEffectList();
		if (!effect_list) {
			return;
		}
		for (auto effect: (*effect_list)) {
			this->numberOfEffects += 1;
			if (this->active_effects.find(effect) == this->active_effects.end()) {
				EffectSetting* base_spell = effect->GetBaseObject();
				Profilers::Start("MagicRuntime");
				auto factorySearch = this->factories.find(base_spell);
				Profilers::Stop("MagicRuntime");
				if (factorySearch != this->factories.end()) {
					auto &[key, factory] = (*factorySearch);
					auto magic_effect = factory->MakeNew(effect);
					if (magic_effect) {
						this->active_effects.try_emplace(effect, std::move(magic_effect));
					}
				}
			}
		}
	}

	std::string MagicManager::DebugName() {
		return "MagicManager";
	}

	void MagicManager::Update() {
		Profilers::Start("MagicLookup");
		for (auto actor: find_actors()) {
			this->ProcessActiveEffects(actor);
		}
		Profilers::Stop("MagicLookup");

		for (auto i = this->active_effects.begin(); i != this->active_effects.end();) {
			this->numberOfOurEffects += 1;
			auto& magic = (*i);


			Profilers::Start(magic.second->GetName());
			magic.second->poll();
			Profilers::Stop(magic.second->GetName());
			if (magic.second->IsFinished()) {
				i = this->active_effects.erase(i);
			} else {
				++i;
			}
		}
	}

	void MagicManager::Reset() {
		this->active_effects.clear();
	}

	void MagicManager::DataReady() {

		// Potions
		
		RegisterMagic<SizeHunger>("EffectSizeHungerPotion");

		RegisterMagic<MaxSizePotion>("EffectSizePotionWeak");
		RegisterMagic<MaxSizePotion>("EffectSizePotionNormal");
		RegisterMagic<MaxSizePotion>("EffectSizePotionStrong");
		RegisterMagic<MaxSizePotion>("EffectSizePotionExtreme");
		RegisterMagic<MaxSizePotion>("AlchEffectSize");

		RegisterMagic<MightPotion>("EffectMightPotionWeak");
		RegisterMagic<MightPotion>("EffectMightPotionNormal");
		RegisterMagic<MightPotion>("EffectMightPotionStrong");
		RegisterMagic<MightPotion>("EffectMightPotionExtreme");
		RegisterMagic<MightPotion>("AlchEffectMight");

		RegisterMagic<ExperiencePotion>("SizeMasteryPotionBasic");

		RegisterMagic<EssencePotion>("EffectEssencePotionWeak");
		RegisterMagic<EssencePotion>("EffectEssencePotionNormal");
		RegisterMagic<EssencePotion>("EffectEssencePotionStrong");
		RegisterMagic<EssencePotion>("EffectEssencePotionExtreme");
		RegisterMagic<EssencePotion>("AlchEffectEssence");

		RegisterMagic<ShrinkResistPotion>("ResistShrinkPotionWeak");
		RegisterMagic<ShrinkResistPotion>("ResistShrinkPotionNormal");
		RegisterMagic<ShrinkResistPotion>("ResistShrinkPotionStrong");
		RegisterMagic<ShrinkResistPotion>("ResistShrinkPotionExtreme");
		RegisterMagic<ShrinkResistPotion>("AlchResistShrink");

		RegisterMagic<GrowthPotion>("EffectGrowthPotionWeak");
		RegisterMagic<GrowthPotion>("EffectGrowthPotionNormal");
		RegisterMagic<GrowthPotion>("EffectGrowthPotionStrong");
		RegisterMagic<GrowthPotion>("EffectGrowthPotionExtreme");
		RegisterMagic<GrowthPotion>("AlchEffectGrowth");

		RegisterMagic<ShrinkPotion>("SizeDrainPotion");
		RegisterMagic<Shrink_Poison>("ShrinkPoison");

		RegisterMagic<ShrinkFoe>("ShrinkEnemy");
		RegisterMagic<ShrinkFoe>("ShrinkEnemyAOE");
		RegisterMagic<ShrinkFoe>("ShrinkEnemyAOEMast");
		RegisterMagic<ShrinkFoe>("ShrinkBolt");
		RegisterMagic<ShrinkFoe>("ShrinkStorm");

		RegisterMagic<Gigantism>("EnchGigantism");

		RegisterMagic<SwordOfSize>("SwordEnchant");
		RegisterMagic<SlowGrow>("SlowGrowth");
		RegisterMagic<SlowGrow>("SlowGrowthDual");
		RegisterMagic<TinyCalamity>("TinyCalamity");
		RegisterMagic<Growth>("GrowthSpell");
		RegisterMagic<Growth>("GrowthSpellAdept");
		RegisterMagic<Growth>("GrowthSpellExpert");
		RegisterMagic<Shrink>("ShrinkSpell");
		RegisterMagic<GrowOther>("GrowAlly");
		RegisterMagic<GrowOther>("GrowAllyAdept");
		RegisterMagic<GrowOther>("GrowAllyExpert");

		RegisterMagic<ShrinkOther>("ShrinkAlly");
		RegisterMagic<ShrinkOther>("ShrinkAllyAdept");
		RegisterMagic<ShrinkOther>("ShrinkAllyExpert");
		RegisterMagic<RestoreSize>("RestoreSize");
		RegisterMagic<RestoreSizeOther>("RestoreSizeNPC");
		//RegisterMagic<ShrinkRune>("ShrinkRuneEffect");
		//RegisterMagic<ShrinkCloak>("ShrinkCloakEffect");
		RegisterMagic<Absorb>("AbsorbMGEF");
		RegisterMagic<Absorb>("TrueAbsorb");
		RegisterMagic<GrowthSpurt>("GrowthSpurt1");
		RegisterMagic<GrowthSpurt>("GrowthSpurt2");
		RegisterMagic<GrowthSpurt>("GrowthSpurt3");
	}
}
