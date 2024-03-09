#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/LevelSelectLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>

using namespace geode::prelude;

GJGameLevel* orgLevel = nullptr;
std::string orgLevelString; // for the original Congregation level's string without the startpos
bool jumpscare = false;
int type = 0; // 1: main level, 2: editor level, 3: online level
int mainLevels[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,1001,1002,1003,2001,2002,2003,2004,2005,2006,2007,2008,2009,2010,3001,4001,4002,4003,5001,5002,5003,5004};
std::string startPos = "1,31,2,24525,3,1605,155,3,36,1,kA2,0,kA3,0,kA8,0,kA4,1,kA9,1,kA10,0,kA22,0,kA23,0,kA24,0,kA27,1,kA40,1,kA41,1,kA42,1,kA28,0,kA29,0,kA31,1,kA32,1,kA36,0,kA43,0,kA44,0,kA45,1,kA33,1,kA34,1,kA35,0,kA37,1,kA38,1,kA39,1,kA19,0,kA26,0,kA20,0,kA21,0,kA11,0;";


class $modify(PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
		auto chance = Mod::get()->getSettingValue<double>("chance");
		if (rand()/(RAND_MAX+1.0) < chance/100) {
			orgLevel = level;
			level = GameLevelManager::get()->getSavedLevel(68668045);
			
			if (Mod::get()->getSettingValue<bool>("drop")) {
				orgLevelString = level->m_levelString;
				std::string levelString = ZipUtils::decompressString(level->m_levelString, true, 0);
				// add a startpos at the drop of the level
				level->m_levelString = ZipUtils::compressString(levelString + startPos, true, 0);
			}
			
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
			// for exiting to the original level's LevelInfoLayer
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
			// for exiting to the original main level's LevelSelectLayer
			p0 = orgLevel->m_levelID.value() - 1;

			jumpscare = false;
			orgLevel = nullptr;
		}
		return LevelSelectLayer::init(p0);
	}
};

class $modify(EditorPauseLayer) {
	void onExitEditor(CCObject* sender) {
		EditorPauseLayer::onExitEditor(sender);
		// for exiting to the original level's EditLevelLayer from the editor's pause menu
		if (jumpscare) {
			if (type == 2) {
				auto scene = CCScene::create();
				auto layer = EditLevelLayer::create(orgLevel);
				scene->addChild(layer);
				CCDirector::get()->replaceScene(scene);

				orgLevel = nullptr;
			}

			jumpscare = false;
		}
	}
};

class $modify(PauseLayer) {
	void onEdit(CCObject* sender) {
		PauseLayer::onEdit(sender);
		if (jumpscare) {
			if (Mod::get()->getSettingValue<bool>("drop")) 
				GameLevelManager::get()->getSavedLevel(68668045)->m_levelString = orgLevelString;
		}
	}
	void onQuit(CCObject* sender) {
		PauseLayer::onQuit(sender);
		if (jumpscare) {
			if (Mod::get()->getSettingValue<bool>("drop")) 
				GameLevelManager::get()->getSavedLevel(68668045)->m_levelString = orgLevelString;

			if (type == 2) {
				// for exiting to the original level's EditLevelLayer from the pause menu
				auto scene = CCScene::create();
				auto layer = EditLevelLayer::create(orgLevel);
				scene->addChild(layer);
				CCDirector::get()->replaceScene(scene);

				orgLevel = nullptr;
			}
			jumpscare = false;
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