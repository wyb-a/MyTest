#pragma once
#include "cocos2d.h"     

using namespace cocos2d;    

class welcomeScene : public Layer
{
public:
    static Scene* createScene();  //welcomeScene��������ʵ��
    virtual bool init();    //welcomeScene��ĳ�ʼ��
    CREATE_FUNC(welcomeScene);   //����welcomeScene��Create����
    void EnterPlayLayer(Ref* pSender); //��ת����Ϸ����
    void EnterSetLayer(Ref* pSender); //��ת����ͣ�����ã�����

};

#include"SimpleAudioEngine.h" //������������ͷ�ļ�
using namespace CocosDenshion;//ʹ�ø���������������ռ�

class SetScene : public Layer//���ü���ͣ����
{
public:
    SetScene();
    static Scene* createScene();  //welcomeScene��������ʵ��
    virtual bool init();    //welcomeScene��ĳ�ʼ��
    CREATE_FUNC(SetScene);   //����welcomeScene��Create����
    void EnterPlayLayer(Ref* pSender); //�ָ�����Ϸ����

    virtual void onEnter();//��дonEnter����
    virtual void onExit();//��дonExit����

private:
    SimpleAudioEngine* _engine;//�������浥��ָ��
    unsigned int _audioID;//�����ļ�ID
    bool _loop;//�Ƿ�ѭ������
};




