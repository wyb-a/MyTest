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
    , m_isAnimationing(true)//start with drop animation������
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
    auto scene = Scene::create();//����һ��������scene��
    auto layer = PlayLayer::create();//����һ��PlayLayer�㣨layer��
    scene->addChild(layer);//��PlayLayer�����ոմ����ĳ�����
    return scene;//�����������
}

bool PlayLayer::init()
{
    if (!Layer::init()) {
        return false;
    }

    //��Ϸ����
    const Size winSize = Director::getInstance()->getWinSize();
    const Size VisibleSize = Director::getInstance()->getVisibleSize();
    const Vec2 kScreenOrigin = Director::getInstance()->getVisibleOrigin();

    auto background = Sprite::create("3timg.jpg");
    background->setAnchorPoint(Point(0, 1));
    background->setPosition(Point(0, winSize.height));
    this->addChild(background, -1);

    // ��ʼ�������
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("sprite.plist");
    spriteSheet = SpriteBatchNode::create("sprite.pvr.ccz");
    addChild(spriteSheet);

    //��ʼ��.����Ŀ�͸�
    m_width = MATRIX_WIDTH;
    m_height = MATRIX_HEIGHT;

    // ��ʼ��Ϸ����
    _score = 0;
    _animation_score = 0;
    _score_label = Label::createWithTTF(StringUtils::format("score: %d", _score), "fonts/Marker Felt.ttf", 20);
    _score_label->setTextColor(cocos2d::Color4B::YELLOW);
    _score_label->setPosition(kScreenOrigin.x + VisibleSize.width / 2, kScreenOrigin.y + VisibleSize.height * 0.85);
    _score_label->setName("score");
    addChild(_score_label, 0);
   


    // ��ʼ����˾�������½ǵĵ�
    m_matrixLeftBottomX = (winSize.width - SushiSprite::getContentWidth() * m_width - (m_width - 1) * SUSHI_GAP) / 2;
    m_matrixLeftBottomY = (winSize.height - SushiSprite::getContentWidth() * m_height - (m_height - 1) * SUSHI_GAP) / 2;

    //��ʼ������
    int arraySize = sizeof(SushiSprite*) * m_width * m_height;//����ߴ��С
    m_matrix = (SushiSprite**)malloc(arraySize);//Ϊm_matrix�����ڴ棬����m_matrix��ָ��ָ�����͵�ָ��
    //�䶨��Ϊ��SushiSprite **m_matrix�����Կ������Ϊm_matrix��һ��ָ������
    memset((void*)m_matrix, 0, arraySize);//��ʼ������m_matrix
   
    //��ͣ
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
    
    //��ʼ����˾����
    for (int row = 0; row < m_height; row++) 
        for (int col = 0; col < m_width; col++) 
            createAndDropSushi(row, col);
        
    scheduleUpdate();
    

    // create���󶨴����¼�
    //1.����һ���¼�������
    auto touchListener = EventListenerTouchOneByOne::create();//OneByOne���㴥���¼�
    // 2 �󶨴����¼�����������
    touchListener->onTouchBegan = CC_CALLBACK_2(PlayLayer::onTouchBegan, this);// ������ʼʱ����
    touchListener->onTouchMoved = CC_CALLBACK_2(PlayLayer::onTouchMoved, this);// �����ƶ�ʱ����
    // 3 ��Ӽ�����
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
    return true;
}

void PlayLayer::menuPauseCallback(Object* pSender)
{
    //�õ����ڵĴ�С
    Size visibleSize = Director::sharedDirector()->getVisibleSize();
    RenderTexture* renderTexture = RenderTexture::create(visibleSize.width, visibleSize.height);

    //������ǰ��������ӽڵ���Ϣ������renderTexture�С�
    //�������ƽ�ͼ��
    renderTexture->begin();
    this->getParent()->visit();
    renderTexture->end();

    //����Ϸ������ͣ��ѹ�볡����ջ�����л���GamePause����
    Director::sharedDirector()->pushScene(Gamepause::scene(renderTexture));
}

//���������ھ��εķ�Χ(�봥���йأ�
//ֻ��onTouchBegan�б�����
SushiSprite* PlayLayer::sushiOfPoint(Point* point)
{
    SushiSprite* sushi = NULL;
    Rect rect = Rect(0, 0, 0, 0);

    for (int i = 0; i < m_height * m_width; i++) {
        sushi = m_matrix[i];
        if (sushi) 
        {
            rect.origin.x = sushi->getPositionX() - (sushi->getContentSize().width / 2);//����x/y��rect���½�����ֵ
            rect.origin.y = sushi->getPositionY() - (sushi->getContentSize().height / 2);//ͨ��getPosition���ê�����꣨Ĭ�����ĵ㣩
            rect.size = sushi->getContentSize();
            if (rect.containsPoint(*point)) //containsPoint���point���ڲ���rect��
            {
                return sushi;
            }
        }
    }
    return NULL;
}

bool PlayLayer::onTouchBegan(Touch* touch, Event* unused)
{
    //��ʼ��
    m_srcSushi = NULL;
    m_destSushi = NULL;

    if (m_isTouchEnable) 
    {
        auto location = touch->getLocation();;//��ȡ������Ļʱ������
        m_srcSushi = sushiOfPoint(&location);//�Ǹ����������ĵ����ھ���
    }
    return m_isTouchEnable;
}

void PlayLayer::onTouchMoved(Touch* touch, Event* unused)
{
    //��Դ����Ҫ������
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
        swapSushi();//�ɽ����Ż�
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
//����λ��
void PlayLayer::swapSushi()
{
    m_isAnimationing = true;
    m_isTouchEnable = false;
    if (!m_srcSushi || !m_destSushi) 
    {
        m_movingVertical = true;//Ĭ��
        return;
    }

    Point SrcPos = m_srcSushi->getPosition();
    Point DestPos = m_destSushi->getPosition();
    float time = 0.2;

    // 1.��������Ա�����
    m_matrix[m_srcSushi->getRow() * m_width + m_srcSushi->getCol()] = m_destSushi;
    m_matrix[m_destSushi->getRow() * m_width + m_destSushi->getCol()] = m_srcSushi;
    int tmpRow = m_srcSushi->getRow();
    int tmpCol = m_srcSushi->getCol();
    m_srcSushi->setRow(m_destSushi->getRow());
    m_srcSushi->setCol(m_destSushi->getCol());
    m_destSushi->setRow(tmpRow);
    m_destSushi->setCol(tmpCol);

    //�������� 
    std::list<SushiSprite*> colChainList1;
    getColChain(m_srcSushi, colChainList1);

    std::list<SushiSprite*> rowChainList1;
    getRowChain(m_srcSushi, rowChainList1);

    std::list<SushiSprite*> colChainList2;
    getColChain(m_destSushi, colChainList2);

    std::list<SushiSprite*> rowChainList2;
    getRowChain(m_destSushi, rowChainList2);
    // ���н�����Ȼ�����
    if (colChainList1.size() >= 3|| rowChainList1.size() >= 3 || colChainList2.size() >= 3|| rowChainList2.size() >= 3) 
    {       
        m_srcSushi->runAction(MoveTo::create(time, DestPos));
        m_destSushi->runAction(MoveTo::create(time, SrcPos));
        return;
    }

    //2. ���������������ֽ�������
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
        m_isAnimationing = false;//�ȿ�����Ȼ��ִ��update
        for (int i = 0; i < m_height * m_width; i++) 
        {
            SushiSprite* sushi = m_matrix[i];
            if (sushi && sushi->getNumberOfRunningActions() > 0) //��ȡsushi����ִ���еĶ�������
            {
                m_isAnimationing = true;//���ж�������
                break;
            }
        }
    }

    unschedule(schedule_selector(PlayLayer::tickProgress));
    m_isTouchEnable = !m_isAnimationing;
    // ���ж������������ܴ���
    if (!m_isAnimationing) 
    {
        if (m_isNeedFillV) 
        {
            //��ըЧ��������ŵ�������˾����ǿ�����
            fillV();//���
            m_isNeedFillV = false;
        }
        else {
            checkAndRemoveChain();//��⣬����
        }
    }
}

//����chain���
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
//���ȱ
void PlayLayer::fillV()
{
    // �����ƶ������־
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

//ֻ��checkAndRemoveChain()������
void PlayLayer::removeSushi()//����
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
            m_isNeedFillV = true;//��Ҫ����
            // ������ͣ���������ʧ����
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

    auto colorSpriteRight = Sprite::create("hdisappear.png");//������Ч
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

//ֻ��emoveSushi()Ӧ��
void PlayLayer::explodeSushi(SushiSprite* sushi)
{
    float time = 0.3;

    // 1.�𽥱�С����ʧ����˾
    auto scale_to = ScaleTo::create(time, 0.0);
    auto call_func = CallFuncN::create(CC_CALLBACK_1(PlayLayer::actionEndCallback, this));
    auto seq = Sequence::create(scale_to, call_func, NULL);
    sushi->runAction(seq);

    // 2. ��������ʧ��Ч��
    auto circleSprite = Sprite::create("disappear.png");
    addChild(circleSprite, 10);
    circleSprite->setPosition(sushi->getPosition());
    circleSprite->setScale(0);// start size
    circleSprite->runAction(Sequence::create(ScaleTo::create(time, 1.0),
        CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, circleSprite)),
        NULL));

    // 3. �����������Ч
    auto particleStars = ParticleSystemQuad::create("stars.plist");
    particleStars->setAutoRemoveOnFinish(true);
    particleStars->setBlendAdditive(false);
    particleStars->setPosition(sushi->getPosition());
    particleStars->setScale(0.3);
    addChild(particleStars, 20);
}

//����Ҫ
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
            continue;// �ѱ�ǹ����������
        }
        if (sushi->getIgnoreCheck()) {
            continue;// �±仯��������˾��������
        }

        // start count chain
        std::list<SushiSprite*> colChainList;
        getColChain(sushi, colChainList);

        std::list<SushiSprite*> rowChainList;
        getRowChain(sushi, rowChainList);

        std::list<SushiSprite*>& longerList = colChainList.size() > rowChainList.size() ? colChainList : rowChainList;
        if (longerList.size() < 3) {
            continue;// С��3��������
        }
        
        /*����*/
        //�ڲ�������������н����滻����ͼƬ�����������ȥ
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
                // 4������������˾
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

        // ��������ɵ��������4��, ȡ���һ���仯Ϊ������˾
        if (!isSetedIgnoreCheck && longerList.size() > 3) {
            sushi->setIgnoreCheck(true);
            sushi->setIsNeedRemove(false);
            sushi->setDisplayMode(m_movingVertical ? DISPLAY_MODE_VERTICAL : DISPLAY_MODE_HORIZONTAL);
        }
    }

    // 3.��������˵���˾
    removeSushi();
}

//�������е�����
void PlayLayer::markRemove(SushiSprite* sushi)
{
    if (sushi->getIsNeedRemove()|| sushi->getIgnoreCheck() || !sushi)
    {
        return;
    }

    // ����Լ�
    sushi->setIsNeedRemove(true);
    // ���neighbor���ͺͱ�־��������
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
//����
//�������� ���䵽ָ��λ��
void PlayLayer::createAndDropSushi(int row, int col)
{
    Size const size = Director::getInstance()->getWinSize();

    SushiSprite* sushi = SushiSprite::create(row, col);

    // ������ִ�����䶯��
    Point endPosition = positionOfItem(row, col);
    Point startPosition = Point(endPosition.x, endPosition.y + size.height / 2);
    sushi->setPosition(startPosition);
    float const speed = startPosition.y / (1.5 * size.height);//��һ���ٶ�
    sushi->runAction(MoveTo::create(speed, endPosition));
    //����˾��ӵ�������ע�⣬���û����ӵ�������
    //������ӵ�����Ļ����Ͳ���õ����ܵ��Ż���
    spriteSheet->addChild(sushi);
    //��ָ��λ�õ����鸳ֵ
    m_matrix[row * m_width + col] = sushi;//��row�е�col�е����꣨��ά���飩
}

//�õ���Ӧ���о��������ֵ
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

    // �ӷֵ�λ�ˣ�ֹͣ��ʱ��
    if (_animation_score == _score)
    {
        unschedule(schedule_selector(PlayLayer::addScoreCallback));
    }
    if (_score == task_score1)
    {
        //������һ��
        auto label1 = Label::createWithSystemFont("Next round", "fonts/arial.ttf", 25);//����һ��Setting��ǩ
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
    // ��üǷ��ƣ����·����ͽ�����
    _score += delta_score;
    _progress_timer->setPercentage(_progress_timer->getPercentage() + 3.0);
    if (_progress_timer->getPercentage() > 100.0)
        _progress_timer->setPercentage(100.0);

    // ����Ʒּӷֶ���
    schedule(schedule_selector(PlayLayer::addScoreCallback), 0.03);
}

void PlayLayer::tickProgress(float dt)
{
    // ����ʱ��˥����������0
    if (_progress_timer->getPercentage() > 0.0)
        _progress_timer->setPercentage(_progress_timer->getPercentage() - 1.0);
    else
    {
        unschedule(schedule_selector(PlayLayer::tickProgress));
    }

}
