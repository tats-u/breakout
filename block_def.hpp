/////////////////////ブロックのクラス//////////////////////
class Block {
  const static char* txt;
protected:
  //座標取得関数
  int x;
  int y;
  Block() {}
public:
  const static int length;
  //コンストラクタ
  Block(int initx,int inity);
  virtual ~Block();
  //当たり判定関数
  BYTE isHit(Ball *bl);
  //座標取得関数
  int getX() { return x; }
  int getY() { return y; }
  //ボールが当たった時に呼ばれる; 壊れるならtrue
  virtual bool Hit() { return true; }
};
const char* Block::txt = BLOCK_STR;
const int Block::length = strlen(txt);
