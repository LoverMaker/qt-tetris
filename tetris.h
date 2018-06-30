#ifndef TETRIS_H
#define TETRIS_H

//方向
enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    SPACE
};

//定义边界信息
struct Border
{
    int ubound;
    int dbound;
    int lbound;
    int rbound;
};

//坐标
struct BlockPoint
{
    int pos_x;
    int pos_y;
    //    block_point(int x,int y):pos_x(x),pos_y(y){}
};

struct AreaSize
{
   int row;
   int col;
};


class Tetris
{
public:
    Tetris();
    ~Tetris();
    void InitGame(); // 初始游戏，重置分数，清空方块
    void SetGameAreaSize(int row, int col); // 设置游戏大小
    void ResetBlock(); //重置方块
    bool BlockMove(Direction dir); //方块变动
    void BlockRotate(int block[4][4]); //方块旋转
    void CreateBlock(int block_id); //产生方块
    void GetBorder(int block[4][4],Border &border); //计算边界
    void ChangeStable(int x,int y); //转换为稳定方块
    bool IsCollide(int x,int y,Direction dir); //判断是否会碰撞

    // getters
    int GetScore();
    BlockPoint GetCurrentBlockPosition();
    AreaSize GetAreaSize();
    int** GetGameArea();
    int next_block[4][4]; //下一个方块形状

private:
    void checkFullBlock();
    AreaSize area_size;
    int **game_area; //场景区域，1表示活动的方块，2表示稳定的方块，0表示空
    BlockPoint block_pos; //当前方块坐标
    int current_block[4][4]; //当前方块形状
    Border current_border; //当前方块边界
    int score;  //游戏分数
};

#endif // TETRIS_H
