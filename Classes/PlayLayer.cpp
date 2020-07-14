#include "PlayLayer.h"
#include "SushiSprite.h"
#include "Pause.h"
//#include <SushiSprite.h>

#define MATRIX_WIDTH (7)
#define MATRIX_HEIGHT (9)

#define SUSHI_GAP (1)

PlayLayer::PlayLayer()
    : spriteSheet(NULL)
    , m_matrix(NULL)
    , m_width(0)
    , m_height(0)
    , m_matrixLeftBottomX(0)
    , m_matrixLeftBottomY(0)
    , m_isNeedFillV(false)
    , m_isAnimationing(true)//start with drop animation程序锁
    , m_isTouchEnable(true)
    , m_srcSushi(NULL)
    , m_destSushi(NULL)
    , m_movingVertical(true)//drop animation is vertical
{
}

PlayLayer::~PlayLayer()
{
    if (m_matrix) {
        free(m_matrix);
    }
}

Scene* PlayLayer::createScene()
{
    auto scene = Scene::create();//创建一个场景（scene）
    auto layer = PlayLayer::create();//创建一个PlayLayer层（layer）
    scene->addChild(layer);//把PlayLayer层加入刚刚创建的场景中
    return scene;//返回这个场景
}

bool PlayLayer::init()
{
    if (!Layer::init()) {
        return false;
    }

    //游戏背景
    const Size winSize = Director::getInstance()->getWinSize();
    const Size VisibleSize = Director::getInstance()->getVisibleSize();
    const Vec2 kScreenOrigin = Director::getInstance()->getVisibleOrigin();

    auto background = Sprite::create("3timg.jpg");
    background->setAnchorPoint(Point(0, 1));
    background->setPosition(Point(0, winSize.height));
    this->addChild(background, -1);

    // 初始化精灵表单
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("sprite.plist");
    spriteSheet = SpriteBatchNode::create("sprite.pvr.ccz");
    addChild(spriteSheet);

    //初始化.矩阵的宽和高
    m_width = MATRIX_WIDTH;
    m_height = MATRIX_HEIGHT;

    // 初始游戏分数
    _score = 0;
    _animation_score = 0;
    _score_label = Label::createWithTTF(StringUtils::format("score: %d", _score), "fonts/Marker Felt.ttf", 20);
    _score_label->setTextColor(cocos2d::Color4B::YELLOW);
    _score_label->setPosition(kScreenOrigin.x + VisibleSize.width / 2, kScreenOrigin.y + VisibleSize.height * 0.85);
    _score_label->setName("score");
    addChild(_score_label, 0);
   


    // 初始化寿司矩阵左下角的点
    m_matrixLeftBottomX = (winSize.width - SushiSprite::getContentWidth() * m_width - (m_width - 1) * SUSHI_GAP) / 2;
    m_matrixLeftBottomY = (winSize.height - SushiSprite::getContentWidth() * m_height - (m_height - 1) * SUSHI_GAP) / 2;

    //初始化数组
    int arraySize = sizeof(SushiSprite*) * m_width * m_height;//数组尺寸大小
    m_matrix = (SushiSprite**)malloc(arraySize);//为m_matrix分配内存，这里m_matrix是指向指针类型的指针
    //其定义为：SushiSprite **m_matrix。所以可以理解为m_matrix是一个指针数组
    memset((void*)m_matrix, 0, arraySize);//初始化数组m_matrix
   
    //暂停
    MenuItemImage* pCloseItem = MenuItemImage::create(
        "CloseNormal.png",
        "CloseSelected.png",
        this,
        menu_selector(PlayLayer::menuPauseCallback));
    pCloseItem->setPosition(ccp(VisibleSize.width - pCloseItem->getContentSize().width / 2,
        VisibleSize.height - pCloseItem->getContentSize().height / 2));
    // create menu
    Menu* pMenu = Menu::create(pCloseItem, NULL);
    pMenu->setPosition(Point::ZERO);
    this->addChild(pMenu, 1);
    
    //初始化寿司矩阵
    for (int row = 0; row < m_height; row++) 
        for (int col = 0; col < m_width; col++) 
            createAndDropSushi(row, col);
        
    scheduleUpdate();
    

    // create、绑定触摸事件
    //1.创建一个事件监听器
    auto touchListener = EventListenerTouchOneByOne::create();//OneByOne单点触摸事件
    // 2 绑定触摸事件（处理函数）
    touchListener->onTouchBegan = CC_CALLBACK_2(PlayLayer::onTouchBegan, this);// 触摸开始时触发
    touchListener->onTouchMoved = CC_CALLBACK_2(PlayLayer::onTouchMoved, this);// 触摸移动时触发
    // 3 添加监听器
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
    return true;
}

void PlayLayer::menuPauseCallback(Object* pSender)
{
    //得到窗口的大小
    Size visibleSize = Director::sharedDirector()->getVisibleSize();
    RenderTexture* renderTexture = RenderTexture::create(visibleSize.width, visibleSize.height);

    //遍历当前类的所有子节点信息，画入renderTexture中。
    //这里类似截图。
    renderTexture->begin();
    this->getParent()->visit();
    renderTexture->end();

    //将游戏界面暂停，压入场景堆栈。并切换到GamePause界面
    Director::sharedDirector()->pushScene(Gamepause::scene(renderTexture));
}

//到精灵所在矩形的范围(与触摸有关）
//只在onTouchBegan中被引用
SushiSprite* PlayLayer::sushiOfPoint(Point* point)
{
    SushiSprite* sushi = NULL;
    Rect rect = Rect(0, 0, 0, 0);

    for (int i = 0; i < m_height * m_width; i++) {
        sushi = m_matrix[i];
        if (sushi) 
        {
            rect.origin.x = sushi->getPositionX() - (sushi->getContentSize().width / 2);//所求x/y是rect左下角坐标值
            rect.origin.y = sushi->getPositionY() - (sushi->getContentSize().height / 2);//通过getPosition获得锚点坐标（默认中心点）
            rect.size = sushi->getContentSize();
            if (rect.containsPoint(*point)) //containsPoint检测point点在不在rect内
            {
                return sushi;
            }
        }
    }
    return NULL;
}

bool PlayLayer::onTouchBegan(Touch* touch, Event* unused)
{
    //初始化
    m_srcSushi = NULL;
    m_destSushi = NULL;

    if (m_isTouchEnable) 
    {
        auto location = touch->getLocation();;//获取触摸屏幕时的坐标
        m_srcSushi = sushiOfPoint(&location);//那个被触摸到的点所在精灵
    }
    return m_isTouchEnable;
}

void PlayLayer::onTouchMoved(Touch* touch, Event* unused)
{
    //无源或不需要处理触摸
    if (!m_srcSushi || !m_isTouchEnable) 
    {
        return;
    }

    int row = m_srcSushi->getRow();
    int col = m_srcSushi->getCol();

    auto location = touch->getLocation();
    auto halfSushiWidth = m_srcSushi->getContentSize().width / 2;
    auto halfSushiHeight = m_srcSushi->getContentSize().height / 2;

    auto  upRect = Rect(m_srcSushi->getPositionX() - halfSushiWidth,
        m_srcSushi->getPositionY() + halfSushiHeight,
        m_srcSushi->getContentSize().width,
        m_srcSushi->getContentSize().height);

    if (upRect.containsPoint(location)) 
    {
        row++;
        if (row < m_height) {
            m_destSushi = m_matrix[row * m_width + col];
        }
        m_movingVertical = true;
        swapSushi();//可交换才换
        return;
    }

    auto  downRect = Rect(m_srcSushi->getPositionX() - halfSushiWidth,
        m_srcSushi->getPositionY() - (halfSushiHeight * 3),
        m_srcSushi->getContentSize().width,
        m_srcSushi->getContentSize().height);

    if (downRect.containsPoint(location)) 
    {
        row--;
        if (row >= 0) {
            m_destSushi = m_matrix[row * m_width + col];
        }
        m_movingVertical = true;
        swapSushi();
        return;
    }

    auto  leftRect = Rect(m_srcSushi->getPositionX() - (halfSushiWidth * 3),
        m_srcSushi->getPositionY() - halfSushiHeight,
        m_srcSushi->getContentSize().width,
        m_srcSushi->getContentSize().height);

    if (leftRect.containsPoint(location)) 
    {
        col--;
        if (col >= 0) {
            m_destSushi = m_matrix[row * m_width + col];
        }
        m_movingVertical = false;
        swapSushi();
        return;
    }

    auto  rightRect = Rect(m_srcSushi->getPositionX() + halfSushiWidth,
        m_srcSushi->getPositionY() - halfSushiHeight,
        m_srcSushi->getContentSize().width,
        m_srcSushi->getContentSize().height);

    if (rightRect.containsPoint(location)) 
    {
        col++;
        if (col < m_width) {
            m_destSushi = m_matrix[row * m_width + col];
        }
        m_movingVertical = false;
        swapSushi();
        return;
    }

    // not a useful movement
}
//交换位置
void PlayLayer::swapSushi()
{
    m_isAnimationing = true;
    m_isTouchEnable = false;
    if (!m_srcSushi || !m_destSushi) 
    {
        m_movingVertical = true;//默认
        return;
    }

    Point SrcPos = m_srcSushi->getPosition();
    Point DestPos = m_destSushi->getPosition();
    float time = 0.2;

    // 1.交换后可以被消除
    m_matrix[m_srcSushi->getRow() * m_width + m_srcSushi->getCol()] = m_destSushi;
    m_matrix[m_destSushi->getRow() * m_width + m_destSushi->getCol()] = m_srcSushi;
    int tmpRow = m_srcSushi->getRow();
    int tmpCol = m_srcSushi->getCol();
    m_srcSushi->setRow(m_destSushi->getRow());
    m_srcSushi->setCol(m_destSushi->getCol());
    m_destSushi->setRow(tmpRow);
    m_destSushi->setCol(tmpCol);

    //检测可消除 
    std::list<SushiSprite*> colChainList1;
    getColChain(m_srcSushi, colChainList1);

    std::list<SushiSprite*> rowChainList1;
    getRowChain(m_srcSushi, rowChainList1);

    std::list<SushiSprite*> colChainList2;
    getColChain(m_destSushi, colChainList2);

    std::list<SushiSprite*> rowChainList2;
    getRowChain(m_destSushi, rowChainList2);
    // 进行交换，然后结束
    if (colChainList1.size() >= 3|| rowChainList1.size() >= 3 || colChainList2.size() >= 3|| rowChainList2.size() >= 3) 
    {       
        m_srcSushi->runAction(MoveTo::create(time, DestPos));
        m_destSushi->runAction(MoveTo::create(time, SrcPos));
        return;
    }

    //2. 交换后不能消除，又交换回来
    m_matrix[m_srcSushi->getRow() * m_width + m_srcSushi->getCol()] = m_destSushi;
    m_matrix[m_destSushi->getRow() * m_width + m_destSushi->getCol()] = m_srcSushi;
    tmpRow = m_srcSushi->getRow();
    tmpCol = m_srcSushi->getCol();
    m_srcSushi->setRow(m_destSushi->getRow());
    m_srcSushi->setCol(m_destSushi->getCol());
    m_destSushi->setRow(tmpRow);
    m_destSushi->setCol(tmpCol);

    m_srcSushi->runAction(Sequence::create(MoveTo::create(time, DestPos), MoveTo::create(time, SrcPos),NULL));
    m_destSushi->runAction(Sequence::create(MoveTo::create(time, SrcPos),MoveTo::create(time, DestPos),NULL));
}

void PlayLayer::update(float dt)
{
    // check if animationing
    if (m_isAnimationing) 
    {
        // init with false
        m_isAnimationing = false;//先开锁，然后执行update
        for (int i = 0; i < m_height * m_width; i++) 
        {
            SushiSprite* sushi = m_matrix[i];
            if (sushi && sushi->getNumberOfRunningActions() > 0) //获取sushi正在执行中的动作个数
            {
                m_isAnimationing = true;//还有动作，锁
                break;
            }
        }
    }

    unschedule(schedule_selector(PlayLayer::tickProgress));
    m_isTouchEnable = !m_isAnimationing;
    // 还有动作，锁，不能触碰
    if (!m_isAnimationing) 
    {
        if (m_isNeedFillV) 
        {
            //爆炸效果结束后才掉落新寿司，增强打击感
            fillV();//填充
            m_isNeedFillV = false;
        }
        else {
            checkAndRemoveChain();//检测，消除
        }
    }
}

//横向chain检查
void PlayLayer::getColChain(SushiSprite* sushi, std::list<SushiSprite*>& chainList)
{
    chainList.push_back(sushi);// add first sushi

    int neighborCol = sushi->getCol() - 1;
    while (neighborCol >= 0) 
    {
        SushiSprite* neighborSushi = m_matrix[sushi->getRow() * m_width + neighborCol];
        if (neighborSushi&& (neighborSushi->getImgIndex() == sushi->getImgIndex())&& !neighborSushi->getIsNeedRemove()&& !neighborSushi->getIgnoreCheck()) 
        {
            chainList.push_back(neighborSushi);
            neighborCol--;
        }
        else 
            break;
    }
    neighborCol = sushi->getCol() + 1;
    while (neighborCol < m_width) {
        SushiSprite* neighborSushi = m_matrix[sushi->getRow() * m_width + neighborCol];
        if (neighborSushi&& (neighborSushi->getImgIndex() == sushi->getImgIndex()) && !neighborSushi->getIsNeedRemove()&& !neighborSushi->getIgnoreCheck()) 
        {
            chainList.push_back(neighborSushi);
            neighborCol++;
        }
        else 
            break;
    }
}

void PlayLayer::getRowChain(SushiSprite* sushi, std::list<SushiSprite*>& chainList)
{
    chainList.push_back(sushi);
    int neighborRow = sushi->getRow() - 1;
    while (neighborRow >= 0) {
        SushiSprite* neighborSushi = m_matrix[neighborRow * m_width + sushi->getCol()];
        if (neighborSushi&& (neighborSushi->getImgIndex() == sushi->getImgIndex())&& !neighborSushi->getIsNeedRemove() && !neighborSushi->getIgnoreCheck()) 
        {
            chainList.push_back(neighborSushi);
            neighborRow--;
        }
        else 
            break;
    }

    neighborRow = sushi->getRow() + 1;
    while (neighborRow < m_height) {
        SushiSprite* neighborSushi = m_matrix[neighborRow * m_width + sushi->getCol()];
        if (neighborSushi&& (neighborSushi->getImgIndex() == sushi->getImgIndex())&& !neighborSushi->getIsNeedRemove()&& !neighborSushi->getIgnoreCheck()) 
        {
            chainList.push_back(neighborSushi);
            neighborRow++;
        }
        else {
            break;
        }
    }
}
//填补空缺
void PlayLayer::fillV()
{
    // 重置移动方向标志
    m_movingVertical = true;
    m_isAnimationing = true;

    Size size = CCDirector::getInstance()->getWinSize();
    int* colEmptyInfo = (int*)malloc(sizeof(int) * m_width);
    memset((void*)colEmptyInfo, 0, sizeof(int) * m_width);

    // 1. drop exist sushi
    SushiSprite* sushi = NULL;
    for (int col = 0; col < m_width; col++) {
        int removedSushiOfCol = 0;
        // from buttom to top
        for (int row = 0; row < m_height; row++) {
            sushi = m_matrix[row * m_width + col];
            if (NULL == sushi) {
                removedSushiOfCol++;
            }
            else {
                if (removedSushiOfCol > 0) {
                    // evey item have its own drop distance
                    int newRow = row - removedSushiOfCol;
                    // switch in matrix
                    m_matrix[newRow * m_width + col] = sushi;
                    m_matrix[row * m_width + col] = NULL;
                    // move to new position
                    Point startPosition = sushi->getPosition();
                    Point endPosition = positionOfItem(newRow, col);
                    float speed = (startPosition.y - endPosition.y) / size.height;
                    sushi->stopAllActions();// must stop pre drop action
                    sushi->runAction(CCMoveTo::create(speed, endPosition));
                    // set the new row to item
                    sushi->setRow(newRow);
                }
            }
        }

        // record empty info
        colEmptyInfo[col] = removedSushiOfCol;
    }

    // 2. create new item and drop down.
    for (int col = 0; col < m_width; col++) {
        for (int row = m_height - colEmptyInfo[col]; row < m_height; row++) {
            createAndDropSushi(row, col);
        }
    }

    free(colEmptyInfo);
}

//只在checkAndRemoveChain()中引用
void PlayLayer::removeSushi()//消除
{
    // make sequence remove
    m_isAnimationing = true;

    for (int i = 0; i < m_height * m_width; i++) {
        SushiSprite* sushi = m_matrix[i];
        if (!sushi) {
            continue;
        }

        if (sushi->getIsNeedRemove()) 
        {
            m_isNeedFillV = true;//需要掉落
            // 检查类型，并播放消失动画
            if (sushi->getDisplayMode() == DISPLAY_MODE_HORIZONTAL)
            {
                explodeSpecialH(sushi->getPosition());
            }
            else if (sushi->getDisplayMode() == DISPLAY_MODE_VERTICAL)
            {
                explodeSpecialV(sushi->getPosition());
            }
            explodeSushi(sushi);

        }
    }
}

void PlayLayer::explodeSpecialH(Point point)
{
    Size size = Director::getInstance()->getWinSize();
    float scaleX = 4;
    float scaleY = 0.7;
    float time = 0.3;
    Point startPosition = point;
    float speed = 0.6f;

    auto colorSpriteRight = Sprite::create("hdisappear.png");//四消特效
    addChild(colorSpriteRight, 10);
    Point endPosition1 = Point(point.x - size.width, point.y);
    colorSpriteRight->setPosition(startPosition);
    colorSpriteRight->runAction(Sequence::create(ScaleTo::create(time, scaleX, scaleY),
        MoveTo::create(speed, endPosition1),
        CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, colorSpriteRight)),
        NULL));

    auto colorSpriteLeft = Sprite::create("hdisappear.png");
    addChild(colorSpriteLeft, 10);
    Point endPosition2 = Point(point.x + size.width, point.y);
    colorSpriteLeft->setPosition(startPosition);
    colorSpriteLeft->runAction(Sequence::create(ScaleTo::create(time, scaleX, scaleY),
        MoveTo::create(speed, endPosition2),
        CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, colorSpriteLeft)),
        NULL));
    

}

void PlayLayer::explodeSpecialV(Point point)
{
    Size size = Director::getInstance()->getWinSize();
    float scaleY = 4;
    float scaleX = 0.7;
    float time = 0.3;
    Point startPosition = point;
    float speed = 0.6f;

    auto colorSpriteDown = Sprite::create("vdisappear.png");
    addChild(colorSpriteDown, 10);
    Point endPosition1 = Point(point.x, point.y - size.height);
    colorSpriteDown->setPosition(startPosition);
    colorSpriteDown->runAction(Sequence::create(ScaleTo::create(time, scaleX, scaleY),
        MoveTo::create(speed, endPosition1),
        CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, colorSpriteDown)),
        NULL));

    auto colorSpriteUp = Sprite::create("vdisappear.png");
    addChild(colorSpriteUp, 10);
    Point endPosition2 = Point(point.x, point.y + size.height);
    colorSpriteUp->setPosition(startPosition);
    colorSpriteUp->runAction(Sequence::create(ScaleTo::create(time, scaleX, scaleY),
        MoveTo::create(speed, endPosition2),
        CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, colorSpriteUp)),
        NULL));
}

void PlayLayer::actionEndCallback(Node* node)
{
    SushiSprite* sushi = (SushiSprite*)node;
    m_matrix[sushi->getRow() * m_width + sushi->getCol()] = NULL;
    sushi->removeFromParent();
}

//只在emoveSushi()应用
void PlayLayer::explodeSushi(SushiSprite* sushi)
{
    float time = 0.3;

    // 1.逐渐变小并消失的寿司
    auto scale_to = ScaleTo::create(time, 0.0);
    auto call_func = CallFuncN::create(CC_CALLBACK_1(PlayLayer::actionEndCallback, this));
    auto seq = Sequence::create(scale_to, call_func, NULL);
    sushi->runAction(seq);

    // 2. 逐渐增大并消失的效果
    auto circleSprite = Sprite::create("disappear.png");
    addChild(circleSprite, 10);
    circleSprite->setPosition(sushi->getPosition());
    circleSprite->setScale(0);// start size
    circleSprite->runAction(Sequence::create(ScaleTo::create(time, 1.0),
        CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, circleSprite)),
        NULL));

    // 3. 随机的粒子特效
    auto particleStars = ParticleSystemQuad::create("stars.plist");
    particleStars->setAutoRemoveOnFinish(true);
    particleStars->setBlendAdditive(false);
    particleStars->setPosition(sushi->getPosition());
    particleStars->setScale(0.3);
    addChild(particleStars, 20);
}

//最重要
void PlayLayer::checkAndRemoveChain()
{
    SushiSprite* sushi;
    // 1. reset ingnore flag
    for (int i = 0; i < m_height * m_width; i++) {
        sushi = m_matrix[i];
        if (!sushi) {
            continue;
        }
        sushi->setIgnoreCheck(false);
    }

    // 2. check chain
    for (int i = 0; i < m_height * m_width; i++) {
        sushi = m_matrix[i];
        if (!sushi) {
            continue;
        }

        if (sushi->getIsNeedRemove()) {
            continue;// 已标记过的跳过检查
        }
        if (sushi->getIgnoreCheck()) {
            continue;// 新变化的特殊寿司，不消除
        }

        // start count chain
        std::list<SushiSprite*> colChainList;
        getColChain(sushi, colChainList);

        std::list<SushiSprite*> rowChainList;
        getRowChain(sushi, rowChainList);

        std::list<SushiSprite*>& longerList = colChainList.size() > rowChainList.size() ? colChainList : rowChainList;
        if (longerList.size() < 3) {
            continue;// 小于3个不消除
        }
        
        /*四消*/
        //在产生四消球过程中仅是替换它的图片，并不真的消去
        std::list<SushiSprite*>::iterator itList;
        bool isSetedIgnoreCheck = false;
        for (itList = longerList.begin(); itList != longerList.end(); itList++) {
            sushi = (SushiSprite*)*itList;
            if (!sushi) 
            {
                continue;
            }
            if (longerList.size() > 3) 
            {
                // 4消产生特殊寿司
                if (sushi == m_srcSushi || sushi == m_destSushi) 
                {
                    isSetedIgnoreCheck = true;
                    sushi->setIgnoreCheck(true);
                    sushi->setIsNeedRemove(false);
                    sushi->setDisplayMode(m_movingVertical ? DISPLAY_MODE_VERTICAL : DISPLAY_MODE_HORIZONTAL);
                    continue;
                }
            }

            markRemove(sushi);
        }

        // 如何是自由掉落产生的4消, 取最后一个变化为特殊寿司
        if (!isSetedIgnoreCheck && longerList.size() > 3) {
            sushi->setIgnoreCheck(true);
            sushi->setIsNeedRemove(false);
            sushi->setDisplayMode(m_movingVertical ? DISPLAY_MODE_VERTICAL : DISPLAY_MODE_HORIZONTAL);
        }
    }

    // 3.消除标记了的寿司
    removeSushi();
}

//整行整列的消除
void PlayLayer::markRemove(SushiSprite* sushi)
{
    if (sushi->getIsNeedRemove()|| sushi->getIgnoreCheck() || !sushi)
    {
        return;
    }

    // 标记自己
    sushi->setIsNeedRemove(true);
    // 检查neighbor类型和标志（四消）
    if (sushi->getDisplayMode() == DISPLAY_MODE_VERTICAL) 
    {
        for (int row = 0; row < m_height; row++) 
        {
            SushiSprite* tmp = m_matrix[row * m_width + sushi->getCol()];
            if (!tmp || tmp == sushi) 
                continue;
            if (tmp->getDisplayMode() == DISPLAY_MODE_NORMAL) 
                tmp->setIsNeedRemove(true);
            else 
                markRemove(tmp);
        }
    }
    else if (sushi->getDisplayMode() == DISPLAY_MODE_HORIZONTAL) 
    {
        for (int col = 0; col < m_width; col++) {
            SushiSprite* tmp = m_matrix[(sushi->getRow()) * m_width + col];
            if (!tmp || tmp == sushi) 
                continue;
            if (tmp->getDisplayMode() == DISPLAY_MODE_NORMAL) 
                tmp->setIsNeedRemove(true);
            else
                markRemove(tmp);
        }
    }
}
//布局
//创建精灵 下落到指定位置
void PlayLayer::createAndDropSushi(int row, int col)
{
    Size const size = Director::getInstance()->getWinSize();

    SushiSprite* sushi = SushiSprite::create(row, col);

    // 创建并执行下落动画
    Point endPosition = positionOfItem(row, col);
    Point startPosition = Point(endPosition.x, endPosition.y + size.height / 2);
    sushi->setPosition(startPosition);
    float const speed = startPosition.y / (1.5 * size.height);//以一定速度
    sushi->runAction(MoveTo::create(speed, endPosition));
    //将寿司添加到精灵表单里。注意，如果没有添加到精灵表单里，
    //而是添加到层里的话，就不会得到性能的优化。
    spriteSheet->addChild(sushi);
    //给指定位置的数组赋值
    m_matrix[row * m_width + col] = sushi;//第row行第col列的坐标（二维数组）
}

//得到对应行列精灵的坐标值
Point PlayLayer::positionOfItem(int row, int col)
{
     float const x = m_matrixLeftBottomX + (SushiSprite::getContentWidth() + SUSHI_GAP) * col + SushiSprite::getContentWidth() / 2;
     float const y = m_matrixLeftBottomY + (SushiSprite::getContentWidth() + SUSHI_GAP) * row + SushiSprite::getContentWidth() / 2;
    return Point(x, y);
}

void PlayLayer::addScoreCallback(float dt)
{
    _animation_score++;
    _score_label->setString(StringUtils::format("score: %d", _animation_score));

    // 加分到位了，停止计时器
    if (_animation_score == _score)
    {
        unschedule(schedule_selector(PlayLayer::addScoreCallback));
    }
    if (_score == task_score1)
    {
        //进入下一关
        auto label1 = Label::createWithSystemFont("Next round", "fonts/arial.ttf", 25);//创建一个Setting标签
        auto menuitem1 = MenuItemLabel::create(label1, CC_CALLBACK_1(PlayLayer::restart, this));
        auto menu = Menu::create( menuitem1, NULL);

    }
}

void PlayLayer::restart(Object* psender)
{
    Director::getInstance()->replaceScene(PlayLayer::createScene());
}

void PlayLayer::addScore(int delta_score)
{
    // 获得记分牌，更新分数和进度条
    _score += delta_score;
    _progress_timer->setPercentage(_progress_timer->getPercentage() + 3.0);
    if (_progress_timer->getPercentage() > 100.0)
        _progress_timer->setPercentage(100.0);

    // 进入计分加分动画
    schedule(schedule_selector(PlayLayer::addScoreCallback), 0.03);
}

void PlayLayer::tickProgress(float dt)
{
    // 根据时间衰减进度条到0
    if (_progress_timer->getPercentage() > 0.0)
        _progress_timer->setPercentage(_progress_timer->getPercentage() - 1.0);
    else
    {
        unschedule(schedule_selector(PlayLayer::tickProgress));
    }

}
