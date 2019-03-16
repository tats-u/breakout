/*******************複数回当てないと消えないブロック*********************/
class SolidBlock : public Block {
  int life; //何回までなら当たっても壊れないか
  const static int initlife; //最初のlife
public:
  string txt; //ブロックを表す文字(普通のブロックとは異なり、途中で変わるのでstaticにはしない)
  const static int length; //ブロックの幅
  SolidBlock(int, int); //コンストラクタ
  bool Hit(); //衝突処理関数
};
const int SolidBlock::initlife = 1;
