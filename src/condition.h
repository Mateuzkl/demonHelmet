/**
 * Demon Helmet - TFS 1.4 - a free and open-source MMORPG server emulator
 * Copyright (C) 2019  Mark Samman <mark.samman@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef FS_CONDITION_H_F92FF8BDDD5B4EA59E2B1BB5C9C0A086
#define FS_CONDITION_H_F92FF8BDDD5B4EA59E2B1BB5C9C0A086

#include <list>  

#include "fileloader.h"
#include "enums.h"

class Creature;
class Player;
class PropStream;

enum ConditionAttr_t {
	CONDITIONATTR_TYPE = 1,
	CONDITIONATTR_ID,
	CONDITIONATTR_TICKS,
	CONDITIONATTR_HEALTHTICKS,
	CONDITIONATTR_HEALTHGAIN,
	CONDITIONATTR_MANATICKS,
	CONDITIONATTR_MANAGAIN,
	CONDITIONATTR_DELAYED,
	CONDITIONATTR_OWNER,
	CONDITIONATTR_INTERVALDATA,
	CONDITIONATTR_SPEEDDELTA,
	CONDITIONATTR_FORMULA_MINA,
	CONDITIONATTR_FORMULA_MINB,
	CONDITIONATTR_FORMULA_MAXA,
	CONDITIONATTR_FORMULA_MAXB,
	CONDITIONATTR_LIGHTCOLOR,
	CONDITIONATTR_LIGHTLEVEL,
	CONDITIONATTR_LIGHTTICKS,
	CONDITIONATTR_LIGHTINTERVAL,
	CONDITIONATTR_SOULTICKS,
	CONDITIONATTR_SOULGAIN,
	CONDITIONATTR_SKILLS,
	CONDITIONATTR_STATS,
  	CONDITIONATTR_BUFFS,
	CONDITIONATTR_OUTFIT,
	CONDITIONATTR_PERIODDAMAGE,
	CONDITIONATTR_ISBUFF,
	CONDITIONATTR_SUBID,
  	CONDITIONATTR_MANASHIELD,
	CONDITIONATTR_MANASHIELD_BREAKABLE_MANA,
	CONDITIONATTR_MANASHIELD_BREAKABLE_MAXMANA,

	//reserved for serialization
	CONDITIONATTR_END = 254,
};

struct IntervalInfo {
	int32_t timeLeft;
	int32_t value;
	int32_t interval;
};

class Condition
{
	public:
		Condition() = default;
		Condition(ConditionId_t initId, ConditionType_t initType, int32_t initTicks, bool initBuff = false, uint32_t initSubId = 0) :
			endTime(initTicks == -1 ? std::numeric_limits<int64_t>::max() : 0),
			subId(initSubId), ticks(initTicks),	conditionType(initType), id(initId), isBuff(initBuff) {}
		virtual ~Condition() = default;

		virtual bool startCondition(Creature* creature);
		virtual bool executeCondition(Creature* creature, int32_t interval);
		virtual void endCondition(Creature* creature) = 0;
		virtual void addCondition(Creature* creature, const Condition* condition) = 0;
		virtual uint32_t getIcons() const;
		ConditionId_t getId() const {
			return id;
		}
		uint32_t getSubId() const {
			return subId;
		}

		virtual Condition* clone() const = 0;

		ConditionType_t getType() const {
			return conditionType;
		}
		int64_t getEndTime() const {
			return endTime;
		}
		int32_t getTicks() const {
			return ticks;
		}
		void setTicks(int32_t newTicks);

		static Condition* createCondition(ConditionId_t id, ConditionType_t type, int32_t ticks, int32_t param = 0, bool buff = false, uint32_t subId = 0);
		static Condition* createCondition(PropStream& propStream);

		virtual bool setParam(ConditionParam_t param, int32_t value);

		//serialization
		bool unserialize(PropStream& propStream);
		virtual void serialize(PropWriteStream& propWriteStream);
		virtual bool unserializeProp(ConditionAttr_t attr, PropStream& propStream);

		bool isPersistent() const;

	protected:
		int64_t endTime;
		uint32_t subId;
		int32_t ticks;
		ConditionType_t conditionType;
		ConditionId_t id;
		bool isBuff;

		virtual bool updateCondition(const Condition* addCondition);
};

class ConditionGeneric : public Condition
{
	public:
		ConditionGeneric(ConditionId_t initId, ConditionType_t initType, int32_t initTicks, bool initBuff = false, uint32_t initSubId = 0):
			Condition(initId, initType, initTicks, initBuff, initSubId) {}

		bool startCondition(Creature* creature) override;
		bool executeCondition(Creature* creature, int32_t interval) override;
		void endCondition(Creature* creature) override;
		void addCondition(Creature* creature, const Condition* condition) override;
		uint32_t getIcons() const override;

		ConditionGeneric* clone() const override {
			return new ConditionGeneric(*this);
		}
};

class ConditionAttributes final : public ConditionGeneric
{
	public:
		ConditionAttributes(ConditionId_t initId, ConditionType_t initType, int32_t initTicks, bool initBuff = false, uint32_t initSubId = 0) :
			ConditionGeneric(initId, initType, initTicks, initBuff, initSubId) {}

		bool startCondition(Creature* creature) final;
		bool executeCondition(Creature* creature, int32_t interval) final;
		void endCondition(Creature* creature) final;
		void addCondition(Creature* creature, const Condition* condition) final;

		bool setParam(ConditionParam_t param, int32_t value) final;

		ConditionAttributes* clone() const final {
			return new ConditionAttributes(*this);
		}

		//serialization
		void serialize(PropWriteStream& propWriteStream) final;
		bool unserializeProp(ConditionAttr_t attr, PropStream& propStream) final;

	private:
		int32_t skills[SKILL_LAST + 1] = {};
		int32_t skillsPercent[SKILL_LAST + 1] = {};
		int32_t stats[STAT_LAST + 1] = {};
		int32_t statsPercent[STAT_LAST + 1] = {};
    	int32_t buffsPercent[BUFF_LAST + 1] = {};
    	int32_t buffs[BUFF_LAST + 1] = {};
		int32_t currentSkill = 0;
		int32_t currentStat = 0;
    	int32_t currentBuff = 0;

		bool disableDefense = false;

		void updatePercentStats(Player* player);
		void updateStats(Player* player);
		void updatePercentSkills(Player* player);
		void updateSkills(Player* player);
    	void updatePercentBuffs(Creature* creature);
    	void updateBuffs(Creature* creature);
};

class ConditionRegeneration final : public ConditionGeneric
{
	public:
		ConditionRegeneration(ConditionId_t initId, ConditionType_t initType, int32_t iniTicks, bool initBuff = false, uint32_t initSubId = 0):
			ConditionGeneric(initId, initType, iniTicks, initBuff, initSubId) {}

		bool startCondition(Creature* creature) override;
		void endCondition(Creature* creature) override;
		void addCondition(Creature* creature, const Condition* addCondition) override;
		bool executeCondition(Creature* creature, int32_t interval) override;

		bool setParam(ConditionParam_t param, int32_t value) override;

		ConditionRegeneration* clone() const override {
			return new ConditionRegeneration(*this);
		}

		//serialization
		void serialize(PropWriteStream& propWriteStream) override;
 		bool unserializeProp(ConditionAttr_t attr, PropStream& propStream) override;

	private:
		uint32_t internalHealthTicks = 0;
		uint32_t internalManaTicks = 0;

		uint32_t healthTicks = 1000;
		uint32_t manaTicks = 1000;
		uint32_t healthGain = 0;
		uint32_t manaGain = 0;
};

class ConditionManaShield final : public Condition
{
public:
	ConditionManaShield(ConditionId_t initId, ConditionType_t initType, int32_t iniTicks, bool initBuff = false,
	                    uint32_t initSubId = 0) :
	    Condition(initId, initType, iniTicks, initBuff, initSubId)
	{}

	bool startCondition(Creature* creature) override;
	void endCondition(Creature* creature) override;
	void addCondition(Creature* creature, const Condition* addCondition) override;
	uint32_t getIcons() const override;

	bool setParam(ConditionParam_t param, int32_t value) override;

	ConditionManaShield* clone() const override { return new ConditionManaShield(*this); }

	// serialization
	void serialize(PropWriteStream& propWriteStream) override;
	bool unserializeProp(ConditionAttr_t attr, PropStream& propStream) override;
	int32_t onDamageTaken(Player* player, int32_t manaChange);

	uint16_t getManaShield() { return manaShield; }
	uint16_t getMaxManaShield() { return maxManaShield; }

private:
	uint16_t manaShield = 0;
	uint16_t maxManaShield = 0;
};

class ConditionSoul final : public ConditionGeneric
{
	public:
		ConditionSoul(ConditionId_t initId, ConditionType_t initType, int32_t iniTicks, bool initBuff = false, uint32_t initSubId = 0) :
			ConditionGeneric(initId, initType, iniTicks, initBuff, initSubId) {}

		void addCondition(Creature* creature, const Condition* addCondition) override;
		bool executeCondition(Creature* creature, int32_t interval) override;

		bool setParam(ConditionParam_t param, int32_t value) override;

		ConditionSoul* clone() const override {
			return new ConditionSoul(*this);
		}

		//serialization
		void serialize(PropWriteStream& propWriteStream) override;
		bool unserializeProp(ConditionAttr_t attr, PropStream& propStream) override;

	private:
		uint32_t internalSoulTicks = 0;
		uint32_t soulTicks = 0;
		uint32_t soulGain = 0;
};

class ConditionInvisible final : public ConditionGeneric
{
	public:
		ConditionInvisible(ConditionId_t initId, ConditionType_t initType, int32_t initTicks, bool initBuff = false, uint32_t initSubId = 0) :
			ConditionGeneric(initId, initType, initTicks, initBuff, initSubId) {}

		bool startCondition(Creature* creature) override;
		void endCondition(Creature* creature) override;

		ConditionInvisible* clone() const override {
			return new ConditionInvisible(*this);
		}
};

class ConditionDamage final : public Condition
{
	public:
		ConditionDamage() = default;
		ConditionDamage(ConditionId_t intiId, ConditionType_t initType, bool initBuff = false, uint32_t initSubId = 0) :
			Condition(intiId, initType, 0, initBuff, initSubId) {}

		static void generateDamageList(int32_t amount, int32_t start, std::list<int32_t>& list);

		bool startCondition(Creature* creature) override;
		bool executeCondition(Creature* creature, int32_t interval) override;
		void endCondition(Creature* creature) override;
		void addCondition(Creature* creature, const Condition* condition) override;
		uint32_t getIcons() const override;

		ConditionDamage* clone() const override {
			return new ConditionDamage(*this);
		}

		bool setParam(ConditionParam_t param, int32_t value) override;

		bool addDamage(int32_t rounds, int32_t time, int32_t value);
		bool doForceUpdate() const {
			return forceUpdate;
		}
		int32_t getTotalDamage() const;

		//serialization
		void serialize(PropWriteStream& propWriteStream) override;
		bool unserializeProp(ConditionAttr_t attr, PropStream& propStream) override;

	private:
		int32_t maxDamage = 0;
		int32_t minDamage = 0;
		int32_t startDamage = 0;
		int32_t periodDamage = 0;
		int32_t periodDamageTick = 0;
		int32_t tickInterval = 2000;

		bool forceUpdate = false;
		bool delayed = false;
		bool field = false;
		uint32_t owner = 0;

		bool init();

		std::list<IntervalInfo> damageList;

		bool getNextDamage(int32_t& damage);
		bool doDamage(Creature* creature, int32_t healthChange);

		bool updateCondition(const Condition* addCondition) override;
};

class ConditionSpeed final : public Condition
{
	public:
		ConditionSpeed(ConditionId_t initId, ConditionType_t initType, int32_t initTicks, bool initBuff, uint32_t initSubId, int32_t initChangeSpeed) :
			Condition(initId, initType, initTicks, initBuff, initSubId), speedDelta(initChangeSpeed) {}

		bool startCondition(Creature* creature) override;
		bool executeCondition(Creature* creature, int32_t interval) override;
		void endCondition(Creature* creature) override;
		void addCondition(Creature* creature, const Condition* condition) override;
		uint32_t getIcons() const override;

		ConditionSpeed* clone() const override {
			return new ConditionSpeed(*this);
		}

		bool setParam(ConditionParam_t param, int32_t value) override;

		void setFormulaVars(float mina, float minb, float maxa, float maxb);

		//serialization
		void serialize(PropWriteStream& propWriteStream) override;
		bool unserializeProp(ConditionAttr_t attr, PropStream& propStream) override;

	private:
		void getFormulaValues(int32_t var, int32_t& min, int32_t& max) const;

		int32_t speedDelta;

		//formula variables
		float mina = 0.0f;
		float minb = 0.0f;
		float maxa = 0.0f;
		float maxb = 0.0f;
};

class ConditionOutfit final : public Condition
{
	public:
		ConditionOutfit(ConditionId_t initId, ConditionType_t initType, int32_t initTicks, bool initBuff = false, uint32_t initSubId = 0) :
			Condition(initId, initType, initTicks, initBuff, initSubId) {}

		bool startCondition(Creature* creature) override;
		bool executeCondition(Creature* creature, int32_t interval) override;
		void endCondition(Creature* creature) override;
		void addCondition(Creature* creature, const Condition* condition) override;

		ConditionOutfit* clone() const override {
			return new ConditionOutfit(*this);
		}

		void setOutfit(const Outfit_t& outfit);
		void setLazyMonsterOutfit(const std::string& monsterName);

		//serialization
		void serialize(PropWriteStream& propWriteStream) override;
		bool unserializeProp(ConditionAttr_t attr, PropStream& propStream) override;

	private:
		Outfit_t outfit;
		std::string monsterName;
};

class ConditionLight final : public Condition
{
	public:
		ConditionLight(ConditionId_t initId, ConditionType_t initType, int32_t initTicks, bool initBuff, uint32_t initSubId, uint8_t initLightlevel, uint8_t initLightcolor) :
			Condition(initId, initType, initTicks, initBuff, initSubId), lightInfo(initLightlevel, initLightcolor) {}

		bool startCondition(Creature* creature) override;
		bool executeCondition(Creature* creature, int32_t interval) override;
		void endCondition(Creature* creature) override;
		void addCondition(Creature* creature, const Condition* addCondition) override;

		ConditionLight* clone() const override {
			return new ConditionLight(*this);
		}

		bool setParam(ConditionParam_t param, int32_t value) override;

		//serialization
		void serialize(PropWriteStream& propWriteStream) override;
		bool unserializeProp(ConditionAttr_t attr, PropStream& propStream) override;

	private:
		LightInfo lightInfo;
		uint32_t internalLightTicks = 0;
		uint32_t lightChangeInterval = 0;
};

class ConditionSpellCooldown final : public ConditionGeneric
{
	public:
		ConditionSpellCooldown(ConditionId_t initId, ConditionType_t initType, int32_t initTicks, bool initBuff = false, uint32_t initSubId = 0) :
			ConditionGeneric(initId, initType, initTicks, initBuff, initSubId) {}

		bool startCondition(Creature* creature) override;
		void addCondition(Creature* creature, const Condition* condition) override;

		ConditionSpellCooldown* clone() const override {
			return new ConditionSpellCooldown(*this);
		}
};

class ConditionSpellGroupCooldown final : public ConditionGeneric
{
	public:
		ConditionSpellGroupCooldown(ConditionId_t initId, ConditionType_t initType, int32_t initTicks, bool initBuff = false, uint32_t initSubId = 0) :
			ConditionGeneric(initId, initType, initTicks, initBuff, initSubId) {}

		bool startCondition(Creature* creature) override;
		void addCondition(Creature* creature, const Condition* condition) override;

		ConditionSpellGroupCooldown* clone() const override {
			return new ConditionSpellGroupCooldown(*this);
		}
};

#endif
