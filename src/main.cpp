#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/CCScheduler.hpp>

using namespace geode::prelude;

GJGameLevel* orgLevel = nullptr;
bool jumpscare = false;

class $modify(PlayLayer) {
    static PlayLayer* create(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
		auto chance = Mod::get()->getSettingValue<double>("chance");
		if (rand()/(RAND_MAX+1.0) < chance/100) {
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

class $modify(CCScheduler) {
	void update(float dt) {
		CCScheduler::update(dt);
		log::warn("{}", MusicDownloadManager::sharedState()->pathForSong(128390));
	}
};

$on_mod(Loaded) {
	srand((unsigned int)time(NULL));
	auto GLM = GameLevelManager::get();
	auto MDM = MusicDownloadManager::sharedState();

	GLM->downloadLevel(68668045, false);
	if (!MDM->isSongDownloaded(895761)) 
		ghc::filesystem::copy(Mod::get()->getResourcesDir() / "895761.mp3", ghc::filesystem::path(MDM->pathForSong(895761).c_str()));
}