/**************************************/
//  プログラミングプロジェクト
//  2014/7/18 081330324 内野 達貴
//  block.cppほか
//  入力: なし
//  機能: ブロック崩し
/**************************************/

#include <unistd.h>
#include <clocale>
#include <cstdlib>
#include <cstring>
#include <ncurses.h>
#include <ctime>
#include <string>
#include <list>
#include <fstream>
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/param.h>
#include <sys/uio.h>
#define _HTTPLIB_CLIENT_ //下のライブラリのサーバ用の機能は使わない
#include "httplib.hpp" //URLデコード/エンコード用ライブラリ
#include "mt64.h" //メルセンヌ・ツイスタ(64bit用)

using namespace std;

//スコア等表示部を除いた高さ・幅
int SCRWIDTH = 0, SCRHEIGHT = 0;
//壁を表す文字
#define WALL_C '*'
#define WALL_H '='
#define WALL_V '|'
//最初のバーを表す文字
#define DEFAULT_BAR "==========="
//ブロックを表す文字
#define BLOCK_STR "#####"
//半壊ブロックを表す文字(BLOCK_STRと同じ文字数にすること)
#define BROKEN_BLOCK_STR "XXXXX"
//ボールを表す文字
#define BALL_C '@'
//ボールを動かす間隔
#define BLMINIT 120ul * CLOCKS_PER_SEC / 1000ul
//次のステージに移行したときどれだけボールのスピードを速めるか(インターバル時間を減算)
#define BLMSUB 20ul * CLOCKS_PER_SEC / 1000ul
//〃どれだけバーの大きさを小さくするか
#define BARSHR 2
//それぞれの色
#define BLOCKCOLOR 68
#define BALLCOLOR 4
#define BARCOLOR 69
#define SOLIDBLOCKCOLOR 64 //SOLIDBLOCK: 当たっていない状態
#define BROKENBLOCKCOLOR 1 //SOLODBLOCK: 半壊状態
//残機の初期値
#define INITLIFE 2
//ゲームオーバー・ゲームクリア画面の表示時間
#define SCRCHTIME 2ul * CLOCKS_PER_SEC
//ボールが落ちた時の一時停止時間
#define PAUSETIME CLOCKS_PER_SEC / 2
//データフィル名
#define DATAFILE "breakout.dat"
//ランキングCGIがあるサーバ
#define RANKINGCGIHOST "localhost"
//ランキングCGIがあるディレクトリ
#define RANKINGCGIPATH "/cgi-bin/blockranking.cgi"
//ランキングCGIサーバに接続するためのポート(標準は80)
#define RANKINGCGIPORT 11111
//ステージ(レベル)の数
#define STAGEMAX 3
//ホールドモードの時にバーが動く間隔
#define BARMOVEINT 45ul * CLOCKS_PER_SEC / 1000ul

//64bit符号なし型の短縮表示
typedef unsigned long ULONG;
typedef unsigned long long ULLONG;
//バイト型の定義(charと呼ばせると誤解を招くため)
typedef char BYTE;
//領域解放マクロ(関数にすると色々面倒)
#define DeletePt(pt); delete pt;pt=NULL;
//RefreshScrSize()関数のプロトタイプ
void RefreshScrSize();
//ロゴ
const char *gameovertext[] = {\
    " ###    #   #   # #####  ###  #   # ##### #### ",\
    "#   #  # #  ## ## #     #   # #   # #     #   #",\
    "#     #   # # # # #     #   # #   # #     #   #",\
    "#  ## ##### #   # ##### #   #  # #  ##### #### ",\
    "#   # #   # #   # #     #   #  # #  #     #   #",\
    "#   # #   # #   # #     #   #  # #  #     #   #",\
    " ###  #   # #   # #####  ###    #   ##### #   #",\
  },*gamecleartext[] = {\
    " ###    #   #   # #####  ###  #     #####   #   #### ",\
    "#   #  # #  ## ## #     #   # #     #      # #  #   #",\
    "#     #   # # # # #     #     #     #     #   # #   #",\
    "#  ## ##### #   # ##### #     #     ##### ##### #### ",\
    "#   # #   # #   # #     #     #     #     #   # #   #",\
    "#   # #   # #   # #     #   # #     #     #   # #   #",\
    " ###  #   # #   # #####  ###  ##### ##### #   # #   #",},\
  *titletext[] = {\
    "####                    #                     #",\
    "#   #                   #                 #   #",\
    "#   # # ###  ###  ###   #  #  ###  #  #  ###  #",\
    "####  ##  # #   #    #  # #  #   # #  #   #   #",\
    "#   # #     #####  ###  ##   #   # #  #   #   #",\
    "#   # #     #     #  #  # #  #   # #  #   # #  ",\
    "####  #      ####  ## # #  #  ###   ## #   #  #"};
//ゲームモード列挙体
typedef enum {TITLE, PLAYING, CLEAR, OVER, PAUSE, BALLWAIT, INPNAME, RANKING, SUBMITCONFIRM, SCRSIZETOOSMALL} gamemode;
//方向を示す列挙体
typedef enum {RIGHT, UP, LEFT, DOWN, STOP = -1} direction;
//ウインドウサイズ(タイトル等を表示できるスペースがどれだけあるかの区分)
enum WINSIZE {SMALL, MID, LARGE} winsize;

/*********************************/
//以下ハイスコア構造体の定義
/*********************************/
#include "scoredata.hpp"
/*********************************/
//以下クラスの定義
/*********************************/
#include "ball_def.hpp"
#include "bar_def.hpp"
#include "block_def.hpp"
#include "solidblock_def.hpp"
#include "letter_def.hpp"
#include "gameker_def.hpp"
/******************************************/
//以下メンバ関数定義
/******************************************/
#include "ball.hpp"
#include "bar.hpp"
#include "block.hpp"
#include "solidblock.hpp"
#include "letter.hpp"
#include "gameker.hpp"
/**********************************/
//メイン関数
/**********************************/
int main(int argc, char *argv[]) {
  setlocale(LC_ALL,"");
  initscr();
  
  start_color();
  //色定義
  init_pair(1, COLOR_RED, COLOR_BLACK);
  init_pair(2, COLOR_GREEN, COLOR_BLACK);
  init_pair(3, COLOR_BLUE, COLOR_BLACK);
  init_pair(4, COLOR_CYAN, COLOR_BLACK);
  init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(6, COLOR_YELLOW, COLOR_BLACK);
  init_pair(64,COLOR_RED, COLOR_RED);
  init_pair(65,COLOR_GREEN, COLOR_GREEN);
  init_pair(66,COLOR_BLUE, COLOR_BLUE);
  init_pair(67,COLOR_CYAN, COLOR_CYAN);
  init_pair(68,COLOR_MAGENTA, COLOR_MAGENTA);
  init_pair(69,COLOR_YELLOW, COLOR_YELLOW);
  init_pair(70,COLOR_WHITE, COLOR_WHITE);
  //各処理
  curs_set(0);
  noecho();
  cbreak();
  keypad(stdscr, TRUE);
  timeout(0);

  //ループ突入
  gamekernel *kernel = new gamekernel(argc >= 2 && strcmp(argv[argc - 1], "cheat") == 0);
  kernel->mainloop();
  delete kernel;
  endwin();
  return 0;
}
/**********************************/
//スクリーンサイズ確認・変更関数
/**********************************/
void RefreshScrSize() {
  //一旦リフレッシュ
  refresh();
  if(SCRWIDTH + 11 != COLS || SCRHEIGHT != LINES) { //画面サイズが変わった
    //画面幅・高さ取得
    SCRWIDTH = COLS - 11;
    SCRHEIGHT = LINES;
    //winsizeを変更(LARGE/MID/SMALL)
    winsize = (COLS >> 1) >= strlen(gamecleartext[0]) + 8 && 17 < LINES ? LARGE : COLS >= strlen(gamecleartext[0]) + 4 && 17 < LINES ? MID : SMALL;
  }
}
