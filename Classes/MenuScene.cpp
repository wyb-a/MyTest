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
    if (!Layer::init()) //�ȳ�ʼ�������init��������γ�ʼ��ʧ�ܣ��򴴽�MyFirstScene��ʧ��
    {
        return false;
    }
    Size VisibleSize = Director::getInstance()->getVisibleSize(); //�����Ļ��С
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto label = Label::createWithSystemFont("Start Game", "fonts/arial.ttf", 35);//����һ��Start Game��ǩ
    //�˵���Ŀ�Ĵ����ж��֣�֮ǰ��HelloWorld.cpp��ʵ�ֵ���ͼƬ�˵���Ŀ(MenuItemImage)��������ͼƬ�������˵���Ŀ
    //label->setPosition(Vec2(origin.x + VisibleSize.width / 2, origin.y + VisibleSize.height - label->getContentSize().height));
    auto menuitem = MenuItemLabel::create(label, CC_CALLBACK_1(welcomeScene::EnterPlayLayer, this));
   
    auto label1 = Label::createWithSystemFont("Setting", "fonts/arial.ttf", 35);//����һ��Setting��ǩ
    auto menuitem1 = MenuItemLabel::create(label1, CC_CALLBACK_1(welcomeScene::EnterSetLayer, this));

    auto menu = Menu::create(menuitem, menuitem1, NULL);
    menu->alignItemsVertically();//���ò˵�������з�ʽ����ֱ���У�
    menu->setPosition(Vec2(VisibleSize.width / 2, VisibleSize.height / 2));//���ò˵���λ��
    this->addChild(menu,1);
  
    //background
    auto sprite = Sprite::create("chicken.png");
    sprite->setPosition(Vec2(VisibleSize.width / 2 + origin.x, VisibleSize.height / 2 + origin.y));
    this->addChild(sprite, 0);

    return true;
}
//�˵��ص�������ʵ��
void welcomeScene::EnterPlayLayer(Ref* pSender)
{
   
    Director::getInstance()->replaceScene(PlayLayer::createScene());
}
void welcomeScene::EnterSetLayer(Ref* pSender)
{
   
    Director::getInstance()->replaceScene(SetScene::createScene());
}
void SetScene::EnterPlayLayer(Ref* pSender) //�ָ�����Ϸ����
{
    //�õ����ڵĴ�С
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCRenderTexture* renderTexture = CCRenderTexture::create(visibleSize.width, visibleSize.height);

    //������ǰ��������ӽڵ���Ϣ������renderTexture�С�
    //�������ƽ�ͼ��
    renderTexture->begin();
    this->getParent()->visit();
    renderTexture->end();
    CCDirector::sharedDirector()->popScene();
}

Scene* SetScene::createScene()
{
    auto scene = Scene::create();//����һ��������scene��
    auto layer = SetScene::create();//����һ��PlayLayer�㣨layer��
    scene->addChild(layer);//��PlayLayer�����ոմ����ĳ�����
    return scene;//�����������
}


SetScene::SetScene() {
    //������ʼ��
    _engine = nullptr;
    _audioID = 0;
    _loop = false;

}
bool SetScene::init()
{
    if (!Layer::init()) //�ȳ�ʼ�������init��������γ�ʼ��ʧ�ܣ��򴴽�SetScene��ʧ��
    {
        return false;
    }
    Size VisibleSize = Director::getInstance()->getVisibleSize(); //�����Ļ��С
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto label = Label::createWithSystemFont("Setting", "fonts/arial.ttf", 30);//����һ����ǩ
    label->setPosition(Vec2(VisibleSize.width / 2, VisibleSize.height - label->getContentSize().height));
    this->addChild(label);
    //Play
    auto Play_Item = MenuItemFont::create("Play music", [&](Ref* sender) { _audioID = _engine->playEffect("Lemonade.mp3", _loop);});
    //��Ŀ�����ʱ�����ò�����Ч�ķ���������һ���ļ�ID
    Play_Item->setPosition(Vec2(-VisibleSize.width / 4, VisibleSize.height / 4));

    //Stop
    auto Stop_Item = MenuItemFont::create("Stop music", [&](Ref* sender) {
        if (_audioID != 0) {
            _engine->stopEffect(_audioID);//��Ŀ�����ʱ������ֹͣ������Ч����������Ϊ�����ļ�ID
            _audioID = 0; }});
    Stop_Item->setPosition(Vec2(VisibleSize.width / 4, VisibleSize.height / 4));

    //Pause
    auto Pause_Item = MenuItemFont::create("Pause music", [&](Ref* sender) {
        if (_audioID != 0) {
            _engine->pauseEffect(_audioID);//��Ŀ�����ʱ��������ͣ������Ч����������Ϊ�����ļ�ID
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

    //���������ʼ��
    _engine = SimpleAudioEngine::getInstance();

    //Ԥ���������ļ�
    _engine->preloadEffect("Lemonade.mp3");
    CCLOG("OnEnter....");
}
void SetScene::onExit() {
    if (_engine) 
    {
        _engine->unloadEffect("Lemonade.mp3");//����ڲ������ļ�����
    }
    Layer::onExit();
}