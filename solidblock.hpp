//コンストラクタ
SolidBlock::SolidBlock(int initx, int inity) {
  x = initx;
  y = inity;
  life = initlife;
  txt.assign(BLOCK_STR);
  gamekernel::drawObj(x, y, txt.c_str(), SOLIDBLOCKCOLOR);
}
//衝突処理関数(ボールが当たった時に呼び出される)
//返り値: 消えるならtrue
bool SolidBlock::Hit() {
  if(life <= 0) return true; //壊れる
  if(life == initlife) {
    txt.assign(BROKEN_BLOCK_STR);
    gamekernel::drawObj(x, y, txt.c_str(), BROKENBLOCKCOLOR); //半壊形にして再描画
  }
  life--;
  return false; //まだ壊れない
}
