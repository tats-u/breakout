/*************ブロック************/
//コンストラクタ
Block::Block(int initx,int inity) {
  x = initx;
  y = inity;
  gamekernel::drawObj(x, y, txt, BLOCKCOLOR);
}
//当たり判定関数
//返り値:
// 000 当たっていない
// 001 真ん中
// 100 左端(角)
// 110 右端(角)
BYTE Block::isHit(Ball *bl) {
  if(bl->getY() == y) { //y座標が一致しないなら0
    if(bl->getX() > x) { //左端・左側でない
      if(bl->getX() + 1 == x + length && bl->getdirX() < 0) return 6; //右端で右側から
      else if(bl->getX() + 1 <= x + length) return 1; //端でない
    } else if(bl->getX() == x) return bl->getdirX() > 0 ? 4 : 1; //左端; 左側からだったら角とみなす
  }
  return 0;
}
//デストラクタ
Block::~Block() {
  gamekernel::clearObjDisp(x, y, length);
}
