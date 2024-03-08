#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/LevelSelectLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>

using namespace geode::prelude;

GJGameLevel* orgLevel = nullptr;
bool jumpscare = false;
int type = 0; // 1: main level, 2: editor level, 3: online level
int mainLevels[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,1001,1002,1003,2001,2002,2003,2004,2005,2006,2007,2008,2009,2010,3001,4001,4002,4003,5001,5002,5003,5004};

class $modify(PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
		auto chance = Mod::get()->getSettingValue<double>("chance");
		if (rand()/(RAND_MAX+1.0) < chance/100) {
			orgLevel = level;
			level = GameLevelManager::get()->getSavedLevel(68668045);
			jumpscare = true;

			if (orgLevel->m_levelType == GJLevelType::Local || (std::find(mainLevels, mainLevels + sizeof(mainLevels)/sizeof(mainLevels[0]), orgLevel->m_levelID.value()) != mainLevels + sizeof(mainLevels)/sizeof(mainLevels[0])))
				type = 1;
			else if (orgLevel->m_levelType == GJLevelType::Editor)
				type = 2;
			else 
				type = 3;
		}

        return PlayLayer::init(level, useReplay, dontCreateObjects);
    }
};

class $modify(LevelInfoLayer) {
	bool init(GJGameLevel* p0, bool p1) {
		if (jumpscare && type == 3) {
			p0 = orgLevel;

			jumpscare = false;
			orgLevel = nullptr;
		}

		return LevelInfoLayer::init(p0, p1);
	}
};

class $modify(LevelSelectLayer) {
	bool init(int p0) {
		if (jumpscare && type == 1) {
			p0 = orgLevel->m_levelID.value() - 1;

			jumpscare = false;
			orgLevel = nullptr;
		}
		return LevelSelectLayer::init(p0);
	}
};

class $modify(PauseLayer) {
	void onQuit(CCObject* sender) {
		if (jumpscare && type == 2) {
			auto scene = CCScene::create();
			auto layer = EditLevelLayer::create(orgLevel);
			scene->addChild(layer);
			CCDirector::get()->replaceScene(scene);

			jumpscare = false;
			orgLevel = nullptr;
		} else {
			PauseLayer::onQuit(sender);
		}
	}
};


$on_mod(Loaded) {
	srand((unsigned int)time(NULL));
	auto GLM = GameLevelManager::get();
	auto MDM = MusicDownloadManager::sharedState();

	GLM->downloadLevel(68668045, false);
	#ifdef GEODE_IS_ANDROID
		std::filesystem::path p = MDM->pathForSong(895761).c_str();
		if (!std::filesystem::exists(p.parent_path() / "895761.mp3"))
			std::filesystem::copy(std::filesystem::path(Mod::get()->getResourcesDir() / "895761.mp3"), p.parent_path() / "895761.mp3");
	#else
		if (!MDM->isSongDownloaded(895761)) 
			std::filesystem::copy(std::filesystem::path(Mod::get()->getResourcesDir() / "895761.mp3"), std::filesystem::path(MDM->pathForSong(895761).c_str()));
	#endif

	// specifically for level id 2004!!
	GLM->storeUserName(5807651, 540196, "Presta");
}