////////////////////////文字を表す構造体/////////////////////////////
class Letter {
  string txt; //表示する文字
public:
  //座標
  int x;
  int y;
  int color; //色
  //コンストラクタ
  Letter(int,int,string,int = 0);
  //デストラクタ
  ~Letter();
  //(再)表示関数
  void Refresh();
};
