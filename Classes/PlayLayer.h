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
    //�����¼�
    virtual bool onTouchBegan(Touch* touch, Event* unused) override;
    virtual void onTouchMoved(Touch* touch, Event* unused) override;
private:
    SpriteBatchNode* spriteSheet;
    SushiSprite** m_matrix;// 2D array which store (SushiSprite *).
    int m_width;
    int m_height;
    //��˾�������½ǵĵ�����
    float m_matrixLeftBottomX;
    float m_matrixLeftBottomY;
    
    bool m_isTouchEnable;// �Ƿ���Ҫ�������¼�
    SushiSprite* m_srcSushi;// �û��ƶ�����˾(Դ��˾)
    SushiSprite* m_destSushi;// �ƶ����ĸ���˾(Ŀ����˾)
    bool m_isAnimationing;//������
    bool m_isNeedFillV;//�Ƿ��п�ȱ��Ҫ�
    bool m_movingVertical;// true: 4������������ը��.  false: ��������ը��.

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
    void markRemove(SushiSprite* sushi);//�������е�����

    int _score; // ��Ϸ����
    int task_score1 = 400, task_score2 = 600;
    cocos2d::Label* _score_label; // ��������
    cocos2d::Label* round_label; // �ؿ�����
    

    int _animation_score; // �Ʒֶ������м����
    cocos2d::ProgressTimer* _progress_timer; // ������

    void addScore(int delta_score); // ��ӷ���
    void addScoreCallback(float dt); // �����ı�Ķ�ʱ���ص�
    void tickProgress(float dt); // ���½�����
    void menuPauseCallback(CCObject* pSender);
};

#endif /* defined(__PlayLayer_H__) */