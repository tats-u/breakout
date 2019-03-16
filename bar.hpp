/*****************バー*****************/
//コンストラクタ
Bar::Bar() {
  y = SCRHEIGHT - 2;
  txt = DEFAULT_BAR;
  length = txt.length();
  x = (SCRWIDTH - length) >> 1; //真ん中に持ってくる
  reDraw();
}
//デストラクタ
Bar::~Bar() {
  gamekernel::clearObjDisp(x, y, length);
}
// 000 当たっていない
// 001 角以外
// 100 左端(角)
// 110 右端(角)
//当たり判定
//原理的にはブロックの当たり判定と同じ
BYTE Bar::isHit(Ball *bl) {
  int nxtX = bl->getX() + bl->getdirX() , nxtY = bl->getY() + bl->getdirY();
  if(nxtY == y) { //y座標が一致することが当たっていることの必要条件
    if(nxtX > x) { //少なくとも左端・左側ではない
      if(nxtX + 1 == x + length && bl->getdirX() < 0) return 6; //右端・左向き→右側角
      else if(nxtX + 1 <= x + length) return 1; //真ん中
    } else if(nxtX == x ) return bl->getdirX() > 0 ? 4 : 1; //左端: 右向き→角 左向き→角でない
  }
  return 0;
}
//カーソルキーで動かす関数(描画処理呼び出しも含む)
void Bar::move(direction dir) {
  switch(dir) {
  case RIGHT:
    if(x < SCRWIDTH - length - 1) moveRelDrw(1,0);
    break;
  case LEFT:
    if(x > 1) moveRelDrw(-1,0);
    break;
  case UP:
    if(gamekernel::cheatmode && y >= SCRHEIGHT - 12 || !gamekernel::cheatmode && y >= SCRHEIGHT - 8) { //チートモードの時は動かせる範囲が広い
      moveRelDrw(0,-1);
    }
    break;
  case DOWN:
    if(y <= SCRHEIGHT - 2) {
      moveRelDrw(0,1);
    }
    break;
  }
  refresh();
}
//座標取得関数
int Bar::getX() {
  return x;
}
int Bar::getY() {
  return y;
}
//長さ取得関数
int Bar::getLen() {
  return length;
}
//再描画関数
void Bar::reDraw() {
  gamekernel::drawObj(x, y, txt, BARCOLOR);
}
//バーを大きくする関数(チートモード限定)
void Bar::enlarge(int diff, Ball *ball) {
  if(diff <= 0 || length + diff + 2 > SCRWIDTH) return; //伸ばす気がない・長すぎの場合は何もしない
  bool isleft;
  if(ball != NULL) { //ボールを弾く準備
    isleft = ball->getX() < x;
  }
  gamekernel::clearObjDisp(x, y, length); //一旦消去
  txt.append(diff, '=');
  length += diff; //長くする
  //壁にくっつかないように位置の調節
  if(x + length + 1 >= SCRWIDTH) x = SCRWIDTH - length - 1;
  else if(x  <= (diff >> 1)) x = 1;
  else x -= (diff >> 1);
  if(ball != NULL && y == ball->getY()) { //ボールに重なるなら弾く
    if(isleft) ballexcludetoleft(ball);
    else ballexcludetoright(ball);
  }
  reDraw(); //再描画
}
//バーを小さくする関数
void Bar::shrink(int diff) {
  if(diff <= 0 || length < diff + 3) return; //短くする気がない/短すぎ
  gamekernel::clearObjDisp(x, y, length); //一旦消去
  //短くする
  txt.erase(length - diff);
  length -= diff;
  //位置調節
  x += (diff >> 1);
  //ボールは考えなくて良い
  reDraw(); //再描画
}
//バーの長さをMAXにする関数(チートモード限定)
void Bar::fulllength(Ball *ball) {
  //この辺りはenlarge()関数と同じ
  if(length + 2 >= SCRWIDTH) return;
  bool isleft;
  if(ball != NULL) {
    isleft = ball->getX() < x;
  }
  int diff = SCRWIDTH -length - 2; //diffを自動計算
  //あとはenlarge()関数と同じ
  gamekernel::clearObjDisp(x, y, length);
  txt.append(diff, '=');
  length += diff;
  if(x + length + 1 >= SCRWIDTH) x = SCRWIDTH - length - 1;
  else if(x  <= (diff >> 1)) x = 1;
  else x -= (diff >> 1);
  if(ball != NULL && y == ball->getY()) {
    if(isleft) ballexcludetoleft(ball);
    else ballexcludetoright(ball);
  }
  reDraw();
}
//中央の座標を取得する関数(ボール発射用)
int Bar::getcenterX() {
  return x + ((length - 1) >> 1);
}
//相対的に動かす関数
void Bar::moveRelDrw(int dx, int dy) {
  gamekernel::clearObjDisp(x, y, length);
  x += dx;
  y += dy;
  reDraw();
}
//ボールを上側に弾く関数
bool Bar::ballexcludetotop(Ball *ball) {
  //ボールがバー上にある
  if(y == ball->getY() && x <= ball->getX()  && ball->getX() + 1 <= x + length) {
    if(ball->getdirY() > 0) { //ボールが下向きならはね返す
      ball->reflect(false, true);
    }
    ball->setloc(ball->getX(), y - 1); //ボールをバーの真上に
  }
  reDraw();
}
//左側
void Bar::ballexcludetoleft(Ball *ball) {
  //ボールがバー上にある
  if(y == ball->getY() && x <= ball->getX()  && ball->getX() + 1 <= x + length) {
    if(x == 1) { //ボールが左の壁のすぐ右(バーと壁にはさまれる)
      ball->reflect(ball->getdirX() < 0, ball->getdirY() > 0);//左向き・下向きならはね返す 
      ball->setloc(1, y - 1); //ボールをバーの真上に
    } else {
      if(ball->getdirX() > 0) { //ボールがバーの
	ball->reflect(true, false);
      }
      ball->setloc(x - 1, y);
      reDraw();
    }
  }
}
//右側
void Bar::ballexcludetoright(Ball *ball) {
  //ボールがバー上にある
  if(y == ball->getY() && x <= ball->getX()  && ball->getX() + 1 <= x + length) {
    if(x + length + 1 == SCRWIDTH) {
      ball->reflect(ball->getdirX() > 0, ball->getdirY() > 0);
      ball->setloc(SCRWIDTH - 2, y - 1);
    }else {
      if(ball->getdirX() < 0) {
	ball->reflect(true, false);
      }
    ball->setloc(x + length, y);
    }
    reDraw();
  }
}
//下側
void Bar::ballexcludetobottom(Ball *ball) {
  if(y == ball->getY() && x <= ball->getX()  && ball->getX() + 1 <= x + length) {
      ball->setloc(ball->getX(), y + 1);
  }
  reDraw();
}
