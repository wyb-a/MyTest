#pragma once
#include "cocos2d.h"     

using namespace cocos2d;    

class welcomeScene : public Layer
{
public:
    static Scene* createScene();  //welcomeScene场景创建实现
    virtual bool init();    //welcomeScene层的初始化
    CREATE_FUNC(welcomeScene);   //创建welcomeScene的Create方法
    void EnterPlayLayer(Ref* pSender); //跳转到游戏界面
    void EnterSetLayer(Ref* pSender); //跳转到暂停（设置）界面

};

#include"SimpleAudioEngine.h" //包含声音引擎头文件
using namespace CocosDenshion;//使用该声音引擎的命名空间

class SetScene : public Layer//设置兼暂停界面
{
public:
    SetScene();
    static Scene* createScene();  //welcomeScene场景创建实现
    virtual bool init();    //welcomeScene层的初始化
    CREATE_FUNC(SetScene);   //创建welcomeScene的Create方法
    void EnterPlayLayer(Ref* pSender); //恢复到游戏界面

    virtual void onEnter();//重写onEnter方法
    virtual void onExit();//重写onExit方法

private:
    SimpleAudioEngine* _engine;//声音引擎单例指针
    unsigned int _audioID;//声音文件ID
    bool _loop;//是否循环播放
};




