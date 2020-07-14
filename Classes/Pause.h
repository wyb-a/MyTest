#ifndef __Pause__H__
#define __Pause__H__
#include "cocos2d.h"
USING_NS_CC;
class Gamepause : public cocos2d::Layer
{
public:
    virtual bool init();
    static cocos2d::CCScene* scene(RenderTexture* sqr);
    CREATE_FUNC(Gamepause);
    //¼ÌÐøÓÎÏ·
    void menuContinueCallback(Object* pSender);

private:

};

#endif // __Gamepause_H__
