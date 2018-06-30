#include <QMessageBox>
#include <QDebug>
#include <QPainter>
#include <QKeyEvent>
#include <QIntValidator>
#include "widget.h"
#include "ui_widget.h"

int AREA_ROW=20; //场景行数
int AREA_COL=12; //场景列数
const int BLOCK_SIZE=25; // 块大小
const int MARGIN=10; //场景边距

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    QIntValidator *v = new QIntValidator(0, 1000, this);
    ui->col->setValidator(v);
    ui->row->setValidator(v);
    ui->col->setText(QString::number(AREA_COL));
    ui->row->setText(QString::number(AREA_ROW));
    isPause = false;
    // 单击获取焦点
    setFocusPolicy(Qt::ClickFocus);
    setWindowTitle("俄罗斯方块");

    connect(ui->start,&QPushButton::clicked, this, &Widget::InitGame);
    connect(ui->pause,&QPushButton::clicked, this, &Widget::PauseGame);
    //初始化游戏
    InitGame();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(129, 134, 140));
    painter.drawRect(rect());
    //画游戏场景边框
    QPen pen;
    pen.setColor(QColor(Qt::gray));
    pen.setWidth(2);
    painter.setPen(pen);
    painter.drawRect(MARGIN-2,MARGIN-2,AREA_COL*BLOCK_SIZE + 4,AREA_ROW*BLOCK_SIZE + 4);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(Qt::black,Qt::SolidPattern));
    painter.drawRect(MARGIN,MARGIN,AREA_COL*BLOCK_SIZE,AREA_ROW*BLOCK_SIZE);

    int rightBlockX = MARGIN*5+AREA_COL*BLOCK_SIZE;

    ui->widget->setGeometry(rightBlockX - 15 , BLOCK_SIZE * 11, BLOCK_SIZE*5, BLOCK_SIZE);
    ui->start->setGeometry(rightBlockX + BLOCK_SIZE*2/3, BLOCK_SIZE * 12 + BLOCK_SIZE * 3/2, BLOCK_SIZE*3, BLOCK_SIZE);
    ui->pause->setGeometry(rightBlockX + BLOCK_SIZE*2/3, BLOCK_SIZE * 12+ BLOCK_SIZE * 3, BLOCK_SIZE*3, BLOCK_SIZE);
    //绘制得分
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial",15));
    painter.drawText(QRect(rightBlockX, MARGIN, BLOCK_SIZE * 4,BLOCK_SIZE * 2),Qt::AlignCenter,"Score:  " + QString::number(tetris.GetScore()));

    painter.drawText(QRect(rightBlockX, MARGIN + BLOCK_SIZE * 2,BLOCK_SIZE*4,BLOCK_SIZE*2),Qt::AlignCenter,"Next");
    //画方块预告
    painter.setPen(pen);
    painter.drawRect(rightBlockX-2, MARGIN + BLOCK_SIZE * 4 - 2, 4*BLOCK_SIZE + 4,4*BLOCK_SIZE + 4);
    painter.setPen(Qt::NoPen);
    painter.drawRect(rightBlockX,MARGIN + BLOCK_SIZE * 4,4*BLOCK_SIZE,4*BLOCK_SIZE);
    pen.setWidth(1);
    pen.setColor(Qt::black);
    painter.setPen(pen);
    painter.setBrush(QBrush(Qt::red,Qt::SolidPattern));
    for(int i=0;i<4;i++) {
        for(int j=0;j<4;j++) {
            if(tetris.next_block[i][j]==1) {
                painter.drawRect(rightBlockX+j*BLOCK_SIZE,MARGIN + BLOCK_SIZE * 4+i*BLOCK_SIZE,BLOCK_SIZE,BLOCK_SIZE);
            }
        }
    }

    //绘制下落方块和稳定方块,注意方块边线的颜色是根据setPen来的，默认黑色
    AreaSize size = tetris.GetAreaSize();
    int ** game_area = tetris.GetGameArea();
    for(int i=0;i<size.row;i++)
        for(int j=0;j<size.col;j++)
        {
            //绘制活动方块
            if(game_area[i][j]==1)
            {
                painter.setBrush(QBrush(Qt::red,Qt::SolidPattern));
                painter.drawRect(j*BLOCK_SIZE+MARGIN,i*BLOCK_SIZE+MARGIN,BLOCK_SIZE,BLOCK_SIZE);
            }
            //绘制稳定方块
            else if(game_area[i][j]==2)
            {
                painter.setBrush(QBrush(Qt::green,Qt::SolidPattern));
                painter.drawRect(j*BLOCK_SIZE+MARGIN,i*BLOCK_SIZE+MARGIN,BLOCK_SIZE,BLOCK_SIZE);
            }
        }
}

void Widget::timerEvent(QTimerEvent *event)
{
    if (!isPause) {
        //方块下落
        if(event->timerId()==game_timer)
            if(tetris.BlockMove(DOWN)) {
                GameOver();
            }
        //刷新画面
        if(event->timerId()==paint_timer)
            update();
    }
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    bool isOver = false;
    switch(event->key())
    {
    case Qt::Key_Up:
        isOver = tetris.BlockMove(UP);
        break;
    case Qt::Key_Down:
        isOver = tetris.BlockMove(DOWN);
        break;
    case Qt::Key_Left:
        isOver = tetris.BlockMove(LEFT);
        break;
    case Qt::Key_Right:
        isOver = tetris.BlockMove(RIGHT);
        break;
    case Qt::Key_Space:
        isOver = tetris.BlockMove(SPACE);
        break;
    default:
        break;
    }
    if (isOver) {
        GameOver();
    }
}


void Widget::PauseGame() {
    isPause = !isPause;
    ui->pause->setText(isPause ? "继续游戏" : "暂停游戏");
}
void Widget::InitGame()
{
    setFocus();
    if (ui->row->text() != "" && ui->col->text() != "") {
        const int row = ui->row->text().toInt();
        const int col = ui->col->text().toInt();
        if (row >= 20) {
            AREA_ROW = row;
        }
        if (col >= 10) {
            AREA_COL = col;
        }
    }
    //调整窗口尺寸布局
    setFixedSize(AREA_COL*BLOCK_SIZE+MARGIN*10+4*BLOCK_SIZE,AREA_ROW*BLOCK_SIZE+MARGIN*2);
    tetris.SetGameAreaSize(AREA_ROW, AREA_COL);

    tetris.InitGame();
    isPause = false;
    speed_ms=800;
    refresh_ms=30;
    //初始化随机数种子
    srand(time(0));
    StartGame();
}

void Widget::StartGame()
{
    game_timer=startTimer(speed_ms); //开启游戏timer
    paint_timer=startTimer(refresh_ms); //开启界面刷新timer
    int block_id=rand()%7;
    tetris.CreateBlock(block_id);
    tetris.ResetBlock(); //产生方块
}

void Widget::GameOver()
{
    //游戏结束停止计时器
    killTimer(game_timer);
    killTimer(paint_timer);
    QMessageBox::information(this,"failed","game over");

}

