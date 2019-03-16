/////////////////バーのクラス////////////////////
class Bar {
  //座標の取得
  int x;
  int y;
  //バーの長さ
  int length;
  //相対的にバーを動かして再描画する関数
  void moveRelDrw(int, int);
public:
  //バーを表す文字列
  string txt;
  //コンストラクタ
  Bar();
  //デストラクタ
  ~Bar();
  //再描画
  void reDraw();
  //ボールとの当たり判定(ボールが動いた時)
  BYTE isHit(Ball*);
  //上に動かした時ボールに当たっていないかの判定(当たってたら動かす)
  bool ballexcludetotop(Ball*);
  //下・左右で同じことを考える
  void ballexcludetoleft(Ball*);
  void ballexcludetoright(Ball*);
  void ballexcludetobottom(Ball*);
  //カーソルキーで動かす関数
  void move(direction);
  //座標取得関数
  int getX();
  int getY();
  //長さを取得する関数
  int getLen();
  //バーを大きくする関数
  void enlarge(int,Ball* = NULL);
  //バーを小さくする関数
  void shrink(int);
  //バーの長さをMAXにする関数
  void fulllength(Ball* = NULL);
  //中央のx座標を取得する関数
  int getcenterX();
};
