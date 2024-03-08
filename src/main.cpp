#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;

GJGameLevel* orgLevel = nullptr;
bool jumpscare = false;

class $modify(PlayLayer) {
    static PlayLayer* create(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
		auto chance = Mod::get()->getSettingValue<double>("chance");
		if (rand()/(RAND_MAX+1.0) > chance/100) {
			orgLevel = level;
			level = GameLevelManager::get()->getSavedLevel(68668045);
			jumpscare = true;
		}

        return PlayLayer::create(level, useReplay, dontCreateObjects);
    }
};

class $modify(LevelInfoLayer) {
	static LevelInfoLayer* create(GJGameLevel* p0, bool p1) {
		if (jumpscare) {
			p0 = orgLevel;
			jumpscare = false;
			orgLevel = nullptr;
		}

		return LevelInfoLayer::create(p0, p1);
	}
};

$on_mod(Loaded) {
	srand((unsigned int)time(NULL));
	auto glm = GameLevelManager::get();

	GameLevelManager::get()->downloadLevel(68668045, false);
	if (!MusicDownloadManager::sharedState()->isSongDownloaded(895761)) {
		if (GameManager::get()->getIntGameVariable("0033")) {
			ghc::filesystem::copy(Mod::get()->getResourcesDir() / "895761.mp3", dirs::getGameDir() / "Resources" / "895761.mp3");
		} else {
			ghc::filesystem::copy(Mod::get()->getResourcesDir() / "895761.mp3", dirs::getSaveDir() / "895761.mp3");
		}
	}

	// glm->storeUserName(5807651, 540196, "Presta");
}