#include <time.h>
#include <stdlib.h>
#include "tetris.h"

//定义图案代码和边界
//田字
int item1[4][4]=
{
    {0,0,0,0},
    {0,1,1,0},
    {0,1,1,0},
    {0,0,0,0}
};
//右L
int item2[4][4]=
{
    {0,1,0,0},
    {0,1,0,0},
    {0,1,1,0},
    {0,0,0,0}
};
//左L
int item3[4][4]=
{
    {0,0,1,0},
    {0,0,1,0},
    {0,1,1,0},
    {0,0,0,0}
};
//右S
int item4[4][4]=
{
    {0,1,0,0},
    {0,1,1,0},
    {0,0,1,0},
    {0,0,0,0}
};
//左S
int item5[4][4]=
{
    {0,0,1,0},
    {0,1,1,0},
    {0,1,0,0},
    {0,0,0,0}
};
//山形
int item6[4][4]=
{
    {0,0,0,0},
    {0,0,1,0},
    {0,1,1,1},
    {0,0,0,0}
};
//长条
int item7[4][4]=
{
    {0,0,1,0},
    {0,0,1,0},
    {0,0,1,0},
    {0,0,1,0}
};

//拷贝方块图案
inline void block_cpy(int dist[4][4],int source[4][4])
{
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            dist[i][j]=source[i][j];
}

inline int ** NewArray(int row, int col) {
    int ** array = new int *[row];
    for( int i=0; i<row; i++ ){
        array[i] = new int [col]  ;
    }
    return array;
}

void swap(int &a, int &b){
    int t = a;
    a = b;
    b = t;
}

Tetris::Tetris() {
  game_area = nullptr;
}

Tetris::~Tetris() {
    delete []game_area;
}

void Tetris::InitGame() {
    for(int i = 0;i < area_size.row;i++) {
        for(int j = 0;j < area_size.col;j++) {
           game_area[i][j]=0;
        }
    }
    score = 0;
}

void Tetris::SetGameAreaSize(int row, int col) {
    area_size.row = row;
    area_size.col = col;
    //假定数组第一维长度为m， 第二维长度为n
    //动态分配空间
    if (game_area != nullptr) {
        delete []game_area;
    }
    game_area = NewArray(row, col);

}

int** Tetris::GetGameArea() {
    return game_area;
}

int Tetris::GetScore() {
    return score;
}
BlockPoint Tetris::GetCurrentBlockPosition() {
    return block_pos;
}

AreaSize Tetris::GetAreaSize() {
    return area_size;
}

void Tetris::CreateBlock(int block_id)
{
    switch (block_id)
    {
    case 0:
        block_cpy(next_block,item1);
        break;
    case 1:
        block_cpy(next_block,item2);
        break;
    case 2:
        block_cpy(next_block,item3);
        break;
    case 3:
        block_cpy(next_block,item4);
        break;
    case 4:
        block_cpy(next_block,item5);
        break;
    case 5:
        block_cpy(next_block,item6);
        break;
    case 6:
        block_cpy(next_block,item7);
        break;
    default:
        break;
    }
}

void Tetris::GetBorder(int block[4][4], Border &border)
{
    //计算上下左右边界
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            if(block[i][j]==1)
            {
                border.dbound=i;
                break; //直到计算到最后一行有1
            }
    for(int i=3;i>=0;i--)
        for(int j=0;j<4;j++)
            if(block[i][j]==1)
            {
                border.ubound=i;
                break;
            }
    for(int j=0;j<4;j++)
        for(int i=0;i<4;i++)
            if(block[i][j]==1)
            {
                border.rbound=j;
                break;
            }
    for(int j=3;j>=0;j--)
        for(int i=0;i<4;i++)
            if(block[i][j]==1)
            {
                border.lbound=j;
                break;
            }
//    qDebug()<<current_border.ubound<<current_border.dbound<<current_border.lbound<<current_border.rbound;
}

void Tetris::ResetBlock()
{

    //产生当前方块
    block_cpy(current_block,next_block);
    GetBorder(current_block,current_border);

    //产生下一个方块
    int block_id=rand()%7;
    CreateBlock(block_id);

    //设置初始方块坐标,以方块左上角为锚点
    BlockPoint start_point;
    start_point.pos_x=area_size.col/2-2;
    start_point.pos_y=0;
    block_pos=start_point;
}

void Tetris::BlockRotate(int block[4][4])
{
    // 顺时针旋转，按矩阵对称轴交换
    int n = 4;
    for(int i = 0; i < n; ++i) {
       for(int j = 0; j < n-i; ++j) {
         swap(block[i][j], block[n-1-j][n-1-i]);
       }
    }
    for(int i = 0; i < n / 2; ++i) {
      for(int j = 0; j < n; ++j) {
         swap(block[i][j], block[n-1-i][j]);
      }
    }

}

void Tetris::ChangeStable(int x,int y)
{
    for(int i=current_border.ubound;i<=current_border.dbound;i++) {
        for(int j=current_border.lbound;j<=current_border.rbound;j++) {
            if(current_block[i][j]==1) {
                game_area[y+i][x+j]=2; //x和y别搞反
            }
        }
    }
}

bool Tetris::IsCollide(int x,int y,Direction dir) {
    //用拷贝的临时方块做判断
    int temp_block[4][4];
    block_cpy(temp_block,current_block);
    Border temp_border;
    GetBorder(temp_block,temp_border);
    //先尝试按照某方向走一格
    switch(dir) {
    case UP:
        BlockRotate(temp_block);
        GetBorder(temp_block,temp_border); //旋转后要重新计算边界
        break;
    case DOWN:
        y+=1;
        break;
    case LEFT:
        x-=1;
        break;
    case RIGHT:
        x+=1;
        break;
    default:
        break;
    }
    for(int i = temp_border.ubound;i <= temp_border.dbound;i++)
        for(int j = temp_border.lbound;j <= temp_border.rbound;j++)
            if((game_area[y+i][x+j] == 2 && temp_block[i][j] == 1) || x+temp_border.lbound < 0 || x + temp_border.rbound > area_size.col - 1)
                return true;
    return false;
}

bool Tetris::BlockMove(Direction dir) {
    switch (dir) {
    case UP:
        if(IsCollide(block_pos.pos_x,block_pos.pos_y,UP))
            break;
        //逆时针旋转90度
        BlockRotate(current_block);
        //防止旋转后bug,i和j从0到4重新设置方块
        for(int i=0;i<4;i++)
            for(int j=0;j<4;j++)
                game_area[block_pos.pos_y+i][block_pos.pos_x+j]=current_block[i][j];
        //重新计算边界
        GetBorder(current_block,current_border);
        break;
    case DOWN:
        //方块到达边界或者碰撞则不再移动
        if(block_pos.pos_y+current_border.dbound==area_size.row-1 || IsCollide(block_pos.pos_x,block_pos.pos_y,DOWN)) {
            ChangeStable(block_pos.pos_x,block_pos.pos_y);
            ResetBlock();
            break;
        }

        //恢复方块上场景,为了清除移动过程中的方块残留
        for(int j=current_border.lbound;j<=current_border.rbound;j++) {
            game_area[block_pos.pos_y][block_pos.pos_x+j]=0;
        }
        //没有碰撞则下落一格
        block_pos.pos_y+=1;
        //方块下降一格，拷贝到场景,注意左右边界
        for(int i=0;i<4;i++) {//必须是0到4而不是边界索引，考虑到旋转后边界重新计算
            for(int j=current_border.lbound;j<=current_border.rbound;j++) {
                if(block_pos.pos_y+i<=area_size.row-1&&game_area[block_pos.pos_y+i][block_pos.pos_x+j] != 2) {//注意场景数组不越界,而且不会擦出稳定的方块
                    game_area[block_pos.pos_y+i][block_pos.pos_x+j]=current_block[i][j];
                }
            }
        }
        break;
    case LEFT:
        //到左边界或者碰撞不再往左
        if(block_pos.pos_x+current_border.lbound == 0||IsCollide(block_pos.pos_x,block_pos.pos_y,LEFT))
            break;
        //恢复方块右场景,为了清除移动过程中的方块残留
        for(int i=current_border.ubound;i<=current_border.dbound;i++)
            if (game_area[block_pos.pos_y+i][block_pos.pos_x+3] != 2) {
               game_area[block_pos.pos_y+i][block_pos.pos_x+3]=0;
            }
        block_pos.pos_x-=1;
        //方块左移一格，拷贝到场景
        for(int i=current_border.ubound;i <= current_border.dbound;i++)
            for(int j=0;j<4;j++)
                if(block_pos.pos_x+j>=0 && game_area[block_pos.pos_y+i][block_pos.pos_x+j]!=2) //注意场景数组不越界
                    game_area[block_pos.pos_y+i][block_pos.pos_x+j]=current_block[i][j];
        break;
    case RIGHT:
        // //到右边界或者碰撞不再往右
        if(block_pos.pos_x+current_border.rbound==area_size.col-1||IsCollide(block_pos.pos_x,block_pos.pos_y,RIGHT))
            break;
        //恢复方块左场景,为了清除移动过程中的方块残留
        for(int i=current_border.ubound;i<=current_border.dbound;i++)
            if (game_area[block_pos.pos_y+i][block_pos.pos_x] !=2) {
               game_area[block_pos.pos_y+i][block_pos.pos_x]=0;
            }
        block_pos.pos_x+=1;
        //方块右移一格，拷贝到场景
        for(int i=current_border.ubound;i<=current_border.dbound;i++)
            for(int j=0;j<4;j++)
                if(block_pos.pos_x+j<=area_size.col-1&&game_area[block_pos.pos_y+i][block_pos.pos_x+j]!=2) //注意场景数组不越界
                    game_area[block_pos.pos_y+i][block_pos.pos_x+j]=current_block[i][j];
        break;
    case SPACE: //一次到底
        //一格一格下移，直到不能下移
        while(block_pos.pos_y+current_border.dbound<area_size.row-1&&!IsCollide(block_pos.pos_x,block_pos.pos_y,DOWN)) {
            BlockMove(DOWN);
        }
        break;
    default:
        break;
    }

    checkFullBlock();
    //判断游戏是否结束
    for(int j = 0;j < area_size.col;j++) {
        if(game_area[0][j] == 2) {//最顶端也有稳定方块
            return true;
        }
    }
    return false;
}

void Tetris::checkFullBlock() {
    //处理消行，整个场景上面的行依次下移
    int i = area_size.row-1;
    int line_count = 0; //记消行数
    while(i>=1) {
        bool is_line_full=true;
        for(int j=0;j<area_size.col;j++)
            if(game_area[i][j]==0)
            {
                is_line_full = false;
                i--;
                break;
            }
        if(is_line_full)
        {
            for(int k = i;k >= 1;k--)
                for(int j = 0;j < area_size.col;j++)
                    game_area[k][j] = game_area[k-1][j];
            line_count++;//每次增加消行的行数
        }
    }
    score += line_count*10; //得分
}

