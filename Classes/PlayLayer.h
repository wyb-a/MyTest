#ifndef __PlayLayer_H__
#define __PlayLayer_H__

#include "cocos2d.h"

USING_NS_CC;

class SushiSprite;

class PlayLayer : public Layer
{
public:
    PlayLayer();
    ~PlayLayer();
    static Scene* createScene();
    CREATE_FUNC(PlayLayer);
    
    virtual bool init() override;
    virtual void update(float dt) override;
    //触摸事件
    virtual bool onTouchBegan(Touch* touch, Event* unused) override;
    virtual void onTouchMoved(Touch* touch, Event* unused) override;
private:
    SpriteBatchNode* spriteSheet;
    SushiSprite** m_matrix;// 2D array which store (SushiSprite *).
    int m_width;
    int m_height;
    //寿司矩阵左下角的点坐标
    float m_matrixLeftBottomX;
    float m_matrixLeftBottomY;
    
    bool m_isTouchEnable;// 是否需要处理触摸事件
    SushiSprite* m_srcSushi;// 用户移动的寿司(源寿司)
    SushiSprite* m_destSushi;// 移动到哪个寿司(目标寿司)
    bool m_isAnimationing;//程序锁
    bool m_isNeedFillV;//是否有空缺需要填补
    bool m_movingVertical;// true: 4消除产生纵向炸弹.  false: 产生横向炸弹.

    void restart(CCObject* psender);
    void createAndDropSushi(int row, int col);
    Point positionOfItem(int row, int col);
    void checkAndRemoveChain();
    void getColChain(SushiSprite* sushi, std::list<SushiSprite*>& chainList);
    void getRowChain(SushiSprite* sushi, std::list<SushiSprite*>& chainList);
    void removeSushi();
    void actionEndCallback(Node* node);
    void explodeSushi(SushiSprite* sushi);

    void explodeSpecialH(Point point);
    void explodeSpecialV(Point point);

    void fillV();
    SushiSprite* sushiOfPoint(Point* point);
    void swapSushi();
    void markRemove(SushiSprite* sushi);//整行整列的消除

    int _score; // 游戏分数
    int task_score1 = 400, task_score2 = 600;
    cocos2d::Label* _score_label; // 分数文字
    cocos2d::Label* round_label; // 关卡文字
    

    int _animation_score; // 计分动画的中间分数
    cocos2d::ProgressTimer* _progress_timer; // 进度条

    void addScore(int delta_score); // 添加分数
    void addScoreCallback(float dt); // 分数改变的定时器回调
    void tickProgress(float dt); // 更新进度条
    void menuPauseCallback(CCObject* pSender);
};

#endif /* defined(__PlayLayer_H__) */