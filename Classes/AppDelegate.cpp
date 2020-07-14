#include "AppDelegate.h"
#include "MenuScene.h"

USING_NS_CC;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate()
{
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    /*if (!glview) {
        glview = GLView::create("Sushi crush");
        director->setOpenGLView(glview);
    }*/
    if (!glview) {
        glview = GLViewImpl::createWithRect("Sushi crush", cocos2d::Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
        director->setOpenGLView(glview);
    }

    glview->setDesignResolutionSize(448.0f, 672.0f, ResolutionPolicy::SHOW_ALL);
    std::vector<std::string> searchPath;
    searchPath.push_back("w640");
    CCFileUtils::getInstance()->setSearchPaths(searchPath);
    director->setContentScaleFactor(600.0f / 320.0f);

    // turn on display FPS
    director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

    
    
    //第一个启动的游戏场景
    auto scene = welcomeScene::createScene();

    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
}