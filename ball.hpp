/*****************ボール******************/
//コンストラクタ
Ball::Ball(int initx,int inity) {
  //初期座標設定
  x = initx;
  y = inity;
  intv = gamekernel::ballmoveint; //間隔をgamekernelクラスから読み込み
  //方向(速度)設定
  dirx = 1;
  diry = -1;
  clockmv =  clock() + intv; //次に動く時間を設定
  recalled = false;
  gamekernel::drawObj(x, y, ch, BALLCOLOR); //表示
}
//デストラクタ
Ball::~Ball() {
  gamekernel::clearObjDisp(x, y, 1);
}
//座標取得関数
int Ball::getX() {
  return x;
}
int Ball::getY() {
  return y;
}
//位置変更関数(絶対座標)
void Ball::setloc(int setx, int sety) {
  gamekernel::clearObjDisp(x, y, 1); //一旦消去
  x = setx;
  y = sety;
  gamekernel::drawObj(x, y, ch, BALLCOLOR); //再表示
  
}
//ボールを現在の状態(方向)に従って動かす関数
void Ball::move(Bar *bar) {
  BYTE hitflag;
  //壁との衝突を考える
  reflect(x + dirx <= 0 || x + dirx + 1 >= SCRWIDTH,
	  y + diry <= 0);
  //バーとの衝突を考える
  if(hitflag = bar->isHit(this)) {
    reflect(hitflag >= 4, diry > 0);
    //もう一度壁との衝突を考える
    reflect(x + dirx <= 0 || x + dirx + 1 >= SCRWIDTH,
	    false); //y方向の衝突は考えなくてよい
  }
  //ボールを動かす
  moveRelDrw(dirx, diry);
  //次に動かす時間を設定
  clockmv =  clock() + intv;
  if(y > SCRHEIGHT) { //1ミス
    recalled = true;
  }
}
//ボールを反射させる関数
void Ball::reflect(bool invx, bool invy) {
  if(invx) dirx = -dirx;
  if(invy) diry = -diry;
}
//方向取得関数
int Ball::getdirX() {
  return dirx;
}
int Ball::getdirY() {
  return diry;
}
//ボールが動くかどうか取得する関数
bool Ball::nextmove() {
  return (clock() >= clockmv);
}
//ボールの位置をセットする関数(相対座標)
void Ball::moveRelDrw(int dx, int dy) {
  gamekernel::clearObjDisp(x, y, 1);  
  x += dx;
  y += dy;
  gamekernel::drawObj(x, y, ch ,BALLCOLOR);
}
//ボールの速度を再設定する関数
void Ball::ReflectSpeed() {
  intv = gamekernel::ballmoveint;
}
