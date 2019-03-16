/********************コンストラクタ・デストラクタで表示を管理する文字************************/
//デストラクタ(表示消去)
Letter::~Letter() {
  gamekernel::clearObjDisp(x,y,txt.length());
}
//コンストラクタ(表示)
Letter::Letter(int newx,int newy,string newtxt, int newcolor) {
  x = newx;
  y = newy;
  txt = newtxt;
  color = newcolor;
  Refresh(); //表示する
}
//(再)表示関数
void Letter::Refresh() {
  gamekernel::drawObj(x,y,txt,color);
}
