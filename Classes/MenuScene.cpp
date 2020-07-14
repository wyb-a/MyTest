#include "MenuScene.h"
#include"PlayLayer.h"

Scene* welcomeScene::createScene()
{
    auto scene = Scene::create(); 
    auto layer = welcomeScene::create();
    scene->addChild(layer); 
    return scene;   
}

bool welcomeScene::init()
{
    if (!Layer::init()) //先初始化父类的init方法，如何初始化失败，则创建MyFirstScene层失败
    {
        return false;
    }
    Size VisibleSize = Director::getInstance()->getVisibleSize(); //获得屏幕大小
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto label = Label::createWithSystemFont("Start Game", "fonts/arial.ttf", 35);//创建一个Start Game标签
    //菜单条目的创建有多种，之前的HelloWorld.cpp中实现的是图片菜单条目(MenuItemImage)，即根据图片来创建菜单条目
    //label->setPosition(Vec2(origin.x + VisibleSize.width / 2, origin.y + VisibleSize.height - label->getContentSize().height));
    auto menuitem = MenuItemLabel::create(label, CC_CALLBACK_1(welcomeScene::EnterPlayLayer, this));
   
    auto label1 = Label::createWithSystemFont("Setting", "fonts/arial.ttf", 35);//创建一个Setting标签
    auto menuitem1 = MenuItemLabel::create(label1, CC_CALLBACK_1(welcomeScene::EnterSetLayer, this));

    auto menu = Menu::create(menuitem, menuitem1, NULL);
    menu->alignItemsVertically();//设置菜单项的排列方式（垂直排列）
    menu->setPosition(Vec2(VisibleSize.width / 2, VisibleSize.height / 2));//设置菜单的位置
    this->addChild(menu,1);
  
    //background
    auto sprite = Sprite::create("chicken.png");
    sprite->setPosition(Vec2(VisibleSize.width / 2 + origin.x, VisibleSize.height / 2 + origin.y));
    this->addChild(sprite, 0);

    return true;
}
//菜单回调函数的实现
void welcomeScene::EnterPlayLayer(Ref* pSender)
{
   
    Director::getInstance()->replaceScene(PlayLayer::createScene());
}
void welcomeScene::EnterSetLayer(Ref* pSender)
{
   
    Director::getInstance()->replaceScene(SetScene::createScene());
}
void SetScene::EnterPlayLayer(Ref* pSender) //恢复到游戏界面
{
    //得到窗口的大小
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCRenderTexture* renderTexture = CCRenderTexture::create(visibleSize.width, visibleSize.height);

    //遍历当前类的所有子节点信息，画入renderTexture中。
    //这里类似截图。
    renderTexture->begin();
    this->getParent()->visit();
    renderTexture->end();
    CCDirector::sharedDirector()->popScene();
}

Scene* SetScene::createScene()
{
    auto scene = Scene::create();//创建一个场景（scene）
    auto layer = SetScene::create();//创建一个PlayLayer层（layer）
    scene->addChild(layer);//把PlayLayer层加入刚刚创建的场景中
    return scene;//返回这个场景
}


SetScene::SetScene() {
    //变量初始化
    _engine = nullptr;
    _audioID = 0;
    _loop = false;

}
bool SetScene::init()
{
    if (!Layer::init()) //先初始化父类的init方法，如何初始化失败，则创建SetScene层失败
    {
        return false;
    }
    Size VisibleSize = Director::getInstance()->getVisibleSize(); //获得屏幕大小
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto label = Label::createWithSystemFont("Setting", "fonts/arial.ttf", 30);//创建一个标签
    label->setPosition(Vec2(VisibleSize.width / 2, VisibleSize.height - label->getContentSize().height));
    this->addChild(label);
    //Play
    auto Play_Item = MenuItemFont::create("Play music", [&](Ref* sender) { _audioID = _engine->playEffect("Lemonade.mp3", _loop);});
    //条目被点击时，调用播放音效的方法，返回一个文件ID
    Play_Item->setPosition(Vec2(-VisibleSize.width / 4, VisibleSize.height / 4));

    //Stop
    auto Stop_Item = MenuItemFont::create("Stop music", [&](Ref* sender) {
        if (_audioID != 0) {
            _engine->stopEffect(_audioID);//条目被点击时，调用停止播放音效方法，参数为音乐文件ID
            _audioID = 0; }});
    Stop_Item->setPosition(Vec2(VisibleSize.width / 4, VisibleSize.height / 4));

    //Pause
    auto Pause_Item = MenuItemFont::create("Pause music", [&](Ref* sender) {
        if (_audioID != 0) {
            _engine->pauseEffect(_audioID);//条目被点击时，调用暂停播放音效方法，参数为音乐文件ID
        }
        });
    Pause_Item->setPosition(Vec2(-VisibleSize.width / 4, 0));
    //Resume
    auto Quit_Item = MenuItemFont::create("Quit the game", [&](Ref* sender) 
        {
            CCDirector::sharedDirector()->end();
        });
    Quit_Item->setPosition(Vec2(VisibleSize.width / 4, 0));
    auto menu = Menu::create(Play_Item, Stop_Item, Pause_Item,Quit_Item, NULL);
    addChild(menu, 10);
    auto sprite = Sprite::create("1timg.jpg");
    sprite->setPosition(Vec2(VisibleSize.width / 2 + origin.x, VisibleSize.height / 2 + origin.y));
    this->addChild(sprite, 0);

    return true;
}

void SetScene::onEnter() {
   Layer::onEnter();

    //声音引擎初始化
    _engine = SimpleAudioEngine::getInstance();

    //预加载声音文件
    _engine->preloadEffect("Lemonade.mp3");
    CCLOG("OnEnter....");
}
void SetScene::onExit() {
    if (_engine) 
    {
        _engine->unloadEffect("Lemonade.mp3");//清除内部声音文件缓存
    }
    Layer::onExit();
}