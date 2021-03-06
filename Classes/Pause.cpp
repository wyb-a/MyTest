#include "Pause.h"

Scene* Gamepause::scene(RenderTexture* sqr)
{

    Scene* scene = Scene::create();
    Gamepause* layer = Gamepause::create();
    scene->addChild(layer, 1);
    
    Size visibleSize = Director::sharedDirector()->getVisibleSize();
    Sprite* back_spr = Sprite::createWithTexture(sqr->getSprite()->getTexture());
    back_spr->setPosition(ccp(visibleSize.width / 2, visibleSize.height / 2)); //放置位置,这个相对于中心位置。
    back_spr->setFlipY(true);            //翻转，因为UI坐标和OpenGL坐标不同
    scene->addChild(back_spr);
    return scene;
}

bool Gamepause::init()
{

    if (!Layer::init())
    {
        return false;
    }
    //得到窗口的大小
    Size visibleSize = Director::sharedDirector()->getVisibleSize();
    //原点坐标
    Point origin = Director::sharedDirector()->getVisibleOrigin();

    //继续游戏按钮
    MenuItemImage* pContinueItem = MenuItemImage::create(
        "CloseSelected.png",
        "CloseSelected.png",
        this,
        menu_selector(Gamepause::menuContinueCallback));

    pContinueItem->setPosition(ccp(visibleSize.width / 2, visibleSize.height / 2 + 30));


    Menu* pMenu = Menu::create(pContinueItem, NULL);
    pMenu->setPosition(Point::ZERO);
    this->addChild(pMenu, 2);

    return true;
}
void Gamepause::menuContinueCallback(Object* pSender)
{
    Director::sharedDirector()->popScene();

}
