#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "tetris.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    void InitGame(); //初始化
    void StartGame(); //开始游戏
    void PauseGame(); //开始游戏
    void GameOver(); //游戏结束

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    virtual void paintEvent(QPaintEvent *event); //场景刷新
    virtual void timerEvent(QTimerEvent *event); //定时器事件
    virtual void keyPressEvent(QKeyEvent *event); //键盘响应

private:
    Ui::Widget *ui;

private:
    int game_timer; //方块下落计时器
    int paint_timer; //渲染刷新计时器
    int speed_ms; //下落时间间隔
    int refresh_ms; //刷新时间间隔
    bool isPause;
    Tetris tetris;

};

#endif // WIDGET_H
