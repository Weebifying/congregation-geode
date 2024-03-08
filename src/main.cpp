#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/LevelSelectLayer.hpp>

using namespace geode::prelude;

GJGameLevel* orgLevel = nullptr;
bool jumpscare = false;
bool isMainLevel = false;
int mainLevels[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,1001,1002,1003,2001,2002,2003,2004,2005,2006,2007,2008,2009,2010,3001,4001,4002,4003,5001,5002,5003,5004};

class $modify(PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
		auto chance = Mod::get()->getSettingValue<double>("chance");
		if (rand()/(RAND_MAX+1.0) < chance/100) {
			orgLevel = level;
			level = GameLevelManager::get()->getSavedLevel(68668045);
			jumpscare = true;

			if (level->m_levelType == GJLevelType::Local || (std::find(mainLevels, mainLevels + sizeof(mainLevels)/sizeof(mainLevels[0]), orgLevel->m_levelID.value()) != mainLevels + sizeof(mainLevels)/sizeof(mainLevels[0])))
				isMainLevel = true;
			else isMainLevel = false;
		}

        return PlayLayer::init(level, useReplay, dontCreateObjects);
    }
};

class $modify(LevelInfoLayer) {
	static LevelInfoLayer* create(GJGameLevel* p0, bool p1) {
		if (jumpscare && !isMainLevel) {
			p0 = orgLevel;
			jumpscare = false;
			orgLevel = nullptr;
		}

		return LevelInfoLayer::create(p0, p1);
	}
};

class $modify(LevelSelectLayer) {
	bool init(int p0) {
		if (jumpscare && isMainLevel) {
			p0 = orgLevel->m_levelID.value() - 1;
			jumpscare = false;
			orgLevel = nullptr;
		}
		return LevelSelectLayer::init(p0);
	}
};


$on_mod(Loaded) {
	srand((unsigned int)time(NULL));
	auto GLM = GameLevelManager::get();
	auto MDM = MusicDownloadManager::sharedState();

	GLM->downloadLevel(68668045, false);
	if (!MDM->isSongDownloaded(895761)) 
		ghc::filesystem::copy(Mod::get()->getResourcesDir() / "895761.mp3", ghc::filesystem::path(MDM->pathForSong(895761).c_str()));

	// specifically for level id 2004!!
	GLM->storeUserName(5807651, 540196, "Presta");
}