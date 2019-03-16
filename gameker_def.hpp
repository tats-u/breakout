//////////////////ゲームの核クラス/////////////////////
class gamekernel{
  //キーコード
  int keycode;
  //現在のゲームモード
  gamemode mode;
  //ボールのポインタ
  Ball *ball;
  //バーのポインタ
  Bar *bar;
  //ブロックの双方向リスト
  list<Block*> blocks;
  //画面切り替え用カウンタ
  ULONG scrclock;
  //残り残機
  int life;
  //ステージ数(レベル)
  int stage;
  //ハイスコア
  ScoreData hiscr[5];
  //ハイスコア書き込み用
  int rank;
  //現在のスコア
  int score;
  //ステージが始まってからの時間
  int stagetime;
  //次の秒の時間(ゲームを起動してから)
  ULONG nextsec;
  static string datafilename;
  //Miss!
  Letter *letter;
  //Press SPC
  Letter *msgrb;
  //to Shoot
  Letter *msgrb2;
  //スコア登録用の名前一時保管用
  string *hisnamebuf;
  //キーを押し続けなくてもバーが動くか(ホールドモード)
  bool holdmode;
  //ホールドモードの方向
  direction holddir;
  //次にバーが動く時刻(ホールドモード)
  ULONG barmoveclk;

  //各オブジェクトを始末する関数
  void deleteparts();
  //各画面を表示する関数
  void gamestart();
  void gameover();
  void gameclear();
  //タイトル画面を表示する関数
  void backtostart();
  //画面の大きさが小さすぎると警告を出す
  void ScrSizeTooSmall();
  //キー入力を受け付ける関数
  bool KeyAcc();
  //ボールとブロックの当たりを判定する関数(ボール移動処理後)
  bool CheckHit();
  //ボールを動かす処理を呼び出す関数
  bool MoveBalls();
  //ゲーム画面描画
  void DrawGameWin();
  //モードを切り替えてそれに応じた処理をする関数
  void SetMode(gamemode);
  //バーの真上中央からボールを再発射する関数
  void NewBall();
  //次のレベルに移る処理をする関数
  void nextstage();
  //データを読み込む関数
  bool LoadData();
  //データを保存する関数
  void SaveData();
  //ボールを再発射してゲームを再開する関数
  void newballresume();
  //それぞれの表示を更新する関数
  void refreshScore();
  void refreshLife();
  void refreshHiScore();
  void refreshTime();
  void refreshLevel();
  //タイトル画面などのロゴを描画する関数
  void writedots(const char *[], int);
  //Miss!の表示を消す関数
  void deletemissdisp();
  //ゲーム中かどうか返す関数
  bool isPlayingGame();
  //スコアの順位を返す関数
  int getRank();
  //ネットランキングを閲覧
  void ShowNetRanking();
  //ローカルランキングを閲覧
  void ShowLocalRanking();
  //ランキングを表示する
  void DispRanking(ScoreData*, int, const char*);
  //ランキングがエラーで表示できないことを表示する
  void DispRankingError(const char*,const char*);
  //オンラインランキングCGIとHTTP通信する関数
  bool HTTPCommunicate(string&, string);
  //スコアを送信するか確認する表示を出す関数
  void ConfirmSubmit();
  //スコアを送信する処理を行う関数
  void SubmitScore();
  //与えられた文字列を画面中央に表示する関数
  void PrintStrCenter(int, string, int = 0);
  //バーを自動的に動かす関数
  void BarAutoMove();
  //ロゴの下の指示テキストの一番上のY座標を返す関数
  int GetGuidY();
public:
  //ボールを移動させるループ間隔
  static ULONG ballmoveint;
  //チートモードフラグ
  static bool cheatmode;
  //コンストラクタ/デストラクタ
  gamekernel(bool);
  ~gamekernel();
  //メインループ
  void mainloop();
  //オブジェクトの表示を消去(スペースを詰める)
  static void clearObjDisp(int,int,int);
  //オブジェクトを描画
  static void drawObj(int, int, char);
  static void drawObj(int, int, string);
  static void drawObj(int, int, char, int);
  static void drawObj(int, int, string,int);
};
ULONG gamekernel::ballmoveint = BLMINIT;
string gamekernel::datafilename = DATAFILE;
bool gamekernel::cheatmode = false;
