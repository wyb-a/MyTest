#ifndef __SushiSprite_H__
#define __SushiSprite_H__

#include "cocos2d.h"

//实现整行或列的消除。

USING_NS_CC;

typedef enum//精灵的显示模式
{
    DISPLAY_MODE_NORMAL = 0,
    DISPLAY_MODE_HORIZONTAL,//横向的四消球
    DISPLAY_MODE_VERTICAL,//纵向的四消球
} DisplayMode;

class SushiSprite : public Sprite
{
public:
    SushiSprite();
    static SushiSprite* create(int row, int col);
    static float getContentWidth();

    CC_SYNTHESIZE(int, m_row, Row);
    CC_SYNTHESIZE(int, m_col, Col);
    CC_SYNTHESIZE(int, m_imgIndex, ImgIndex);//精灵图片种类
    CC_SYNTHESIZE(bool, m_isNeedRemove, IsNeedRemove);
    CC_SYNTHESIZE(bool, m_ignoreCheck, IgnoreCheck);//产生的4消寿司，本轮不被消除
    CC_SYNTHESIZE_READONLY(DisplayMode, m_displayMode, DisplayMode);
    void setDisplayMode(DisplayMode mode);
};

#endif /* defined(__SushiSprite_H__) */
