class Bar; //バークラスの前方宣言

///////////////////ボールクラス//////////////////
class Ball {
  //座標
  int x;
  int y;
  //どれぐらいの間隔で動くか
  ULONG intv;
  //各座標軸ごとの方向(±1)
  int dirx;
  int diry;
  //前回動いたのは何フレーム目か
  ULONG clockmv;
  //相対的にボールを動かして再描画する関数
  void moveRelDrw(int, int);
public:
  static const char ch = BALL_C; //ボールを表す文字
  bool recalled; //ボールロストフラグ
  //コンストラクタ/デストラクタ
  Ball(int initx,int inity);
  ~Ball();
  //座標取得
  int getX();
  int getY();
  //座標設定
  void setloc(int setx, int sety);
  //ボールを動かす
  void move(Bar*);
  //壁などで反射する
  void reflect(bool invx, bool invy);
  //方向(速度)取得
  int getdirX();
  int getdirY();
  //次に動くかどうかのフラグの取得
  bool nextmove();
  //チートモード用: ボールの速度再設定関数
  void ReflectSpeed();
};
