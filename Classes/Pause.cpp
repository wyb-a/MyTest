#include "Pause.h"

Scene* Gamepause::scene(RenderTexture* sqr)
{

    Scene* scene = Scene::create();
    Gamepause* layer = Gamepause::create();
    scene->addChild(layer, 1);
    
    Size visibleSize = Director::sharedDirector()->getVisibleSize();
    Sprite* back_spr = Sprite::createWithTexture(sqr->getSprite()->getTexture());
    back_spr->setPosition(ccp(visibleSize.width / 2, visibleSize.height / 2)); //����λ��,������������λ�á�
    back_spr->setFlipY(true);            //��ת����ΪUI�����OpenGL���겻ͬ
    scene->addChild(back_spr);
    return scene;
}

bool Gamepause::init()
{

    if (!Layer::init())
    {
        return false;
    }
    //�õ����ڵĴ�С
    Size visibleSize = Director::sharedDirector()->getVisibleSize();
    //ԭ������
    Point origin = Director::sharedDirector()->getVisibleOrigin();

    //������Ϸ��ť
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
