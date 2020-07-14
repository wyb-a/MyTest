#include "SushiSprite.h"
#include <random>


USING_NS_CC;
//实现整行或列的消除。

#define TOTAL_SUSHI (6)

static const char* sushiNormal[TOTAL_SUSHI] = {
    "untitled1.png",
    "untitled2.png",
    "untitled3.png",
    "untitled4.png",
    "untitled5.png",
    "untitled6.png"
};

static const char* sushiVertical[TOTAL_SUSHI] = {
    "untitled1-v.png",
    "untitled2-v.png",
    "untitled3-v.png",
    "untitled4-v.png",
    "untitled5-v.png",
    "untitled6-v.png"
};

static const char* sushiHorizontal[TOTAL_SUSHI] = {
    "untitled1-h.png",
    "untitled2-h.png",
    "untitled3-h.png",
    "untitled4-h.png",
    "untitled5-h.png",
    "untitled6-h.png"
};

float SushiSprite::getContentWidth()
{
    static float itemWidth = 0;
    if (0 == itemWidth) {
        Sprite* sprite = Sprite::createWithSpriteFrameName(sushiNormal[0]);
        itemWidth = sprite->getContentSize().width;
    }
    return itemWidth;
}

SushiSprite::SushiSprite()
    : m_col(0)
    , m_row(0)
    , m_imgIndex(0)
    , m_isNeedRemove(false)
    , m_ignoreCheck(false)
    , m_displayMode(DISPLAY_MODE_NORMAL)
{
}

SushiSprite* SushiSprite::create(int row, int col)//寿司精灵的创建
{
    SushiSprite* sushi = new SushiSprite();
    sushi->m_row = row;
    sushi->m_col = col;
    //srand(rand()%100);
    sushi->m_imgIndex = rand() % TOTAL_SUSHI;//生成随机数
    sushi->initWithSpriteFrameName(sushiNormal[sushi->m_imgIndex]);
    sushi->autorelease();
    return sushi;
}

//四消的产生
void SushiSprite::setDisplayMode(DisplayMode mode)
{
    m_displayMode = mode;

    SpriteFrame* frame;
    switch (mode) 
    {
    case DISPLAY_MODE_VERTICAL:
        frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(sushiVertical[m_imgIndex]);
        break;
    case DISPLAY_MODE_HORIZONTAL:
        frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(sushiHorizontal[m_imgIndex]);
        break;
    default:
        return;
    }
    setDisplayFrame(frame);
}