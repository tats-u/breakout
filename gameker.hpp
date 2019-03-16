/**********ゲームの核**************/

//画面の大きさに応じてどのように表示するかを分けている
void gamekernel::writedots(const char *array[], int len) {
  int textcolor = (cheatmode) ? 66 : 64;
  int i,j;
  int hlen = strlen(*array);
  switch(winsize) {
  case LARGE: //画面サイズが十分な場合はスペース2つで１つのドットを形成
    for(i = 0; i < len; i++) {
      move(2 + i, (COLS >> 1) - hlen); //ロゴが中央揃えになるように移動
      for(j = 0;array[i][j] != 0; j++) {
        if(array[i][j] != ' ') {
          attrset(COLOR_PAIR(textcolor));
	  addstr("  ");
	  attrset(0);
	} else {
	  addstr("  ");
	}
      }
    }
    break;
  case MID: //そこまでもない場合は1つのスペースで形成

    for(i = 0; i < len; i++) {
      move(2 + i, COLS - hlen >> 1); //ロゴが中央揃えになるように移動
      for(j = 0;array[i][j] != 0; j++) {
        if(array[i][j] != ' ') {
          attrset(COLOR_PAIR(textcolor));
	  addch(' ');
	  attrset(0);
	} else {
	  addch(' ');
	}
      }
    }
    break;
  case SMALL: //それでもだめなら文字をそのまま表示
    attrset(COLOR_PAIR(textcolor - 63));
    switch(mode) {
    case TITLE:
      PrintStrCenter(SCRHEIGHT >> 2,"Breakout!");
      break;
    case CLEAR:
    case INPNAME:
      PrintStrCenter(SCRHEIGHT >> 2,"GAMECLEAR");
      break;
    case OVER:
      PrintStrCenter(SCRHEIGHT >> 2,"GAMEOVER");
      break;
    }
    attrset(0);
    break;
  }
}

//パーツの削除
void gamekernel::deleteparts() {
  list<Block*>::iterator it;
  //ボール・バーを削除
  DeletePt(ball);
 DeletePt(bar);
  life = INITLIFE; //ライフを初期化
  //ブロックを削除
  if(!blocks.empty()) {
    for(it = blocks.begin(); !blocks.empty(); it = blocks.begin()) {
      delete *it;
      blocks.pop_front();
    }
  }
}
//ゲームスタート
void gamekernel::gamestart() {
  int i,j, len, newlen, initx, blocknum;
  RefreshScrSize();
  //画面が小さい
  if(SCRWIDTH < 2 * Block::length + 5 || SCRHEIGHT < 17) {
    ScrSizeTooSmall();
    return;
  }
  SetMode(BALLWAIT); //ボール発射待機
  life = INITLIFE; //ライフ初期化
  stage = 1; //レベル初期化
  stagetime = -1; //DrawGameWin内で加算する別関数を呼び出すので0にはしていない
  score = 0; //得点初期化
  ballmoveint = BLMINIT; //ボールスピード初期化
  clear(); //画面リセット
  holdmode = false; //ホールドモードリセット
  //以下バー・ボール・ブロックの表示(コンストラクタを使用)
  bar = new Bar(); //バー初期化・描画
  //ブロック初期化・描画
  for(len = Block::length, blocknum = 0; (newlen = len + Block::length + 1) + 4 <= SCRWIDTH; len++, blocknum++);
  //1パス目: ブロックを1段仮配置(実際にはしていない)して左端を決める
  initx = (SCRWIDTH - len) >> 1;
  for(i = 2; i + 15 <= SCRHEIGHT; i += 2) { //2パス目: 実際に配置
    for(j = initx; j + Block::length + 2 < SCRWIDTH; j += Block::length + 1) {
      blocks.push_back(new Block(j, i));
    }
  }
  //枠・各種情報表示
  DrawGameWin();
}
//次のレベルに移る関数
void gamekernel::nextstage() {
  int i,j, len, newlen, initx, blocknum;
  stage++; //レベルを進める
  ballmoveint = BLMINIT - BLMSUB * (stage - 1); //ボールスピードUP
  DeletePt(ball); //ボールを消去
  life = INITLIFE; //ライフ回復
  SetMode(BALLWAIT);
  clear();
  if(cheatmode) bar->reDraw(); //チートモードでなければバーを縮めて再描画
  else bar->shrink(BARSHR);
  //乱数初期化
  init_genrand64((ULLONG)time(NULL));
  ULLONG threshold = 0xFFFFFFFFFFFFFFFF / (ULLONG)((stage << 1) + 1) * (ULLONG)stage; //しきい値
  //ブロック復活
  for(len = Block::length, blocknum = 0; (newlen = len + Block::length + 1) + 4 <= SCRWIDTH; len++, blocknum++);
   //1パス目
  initx = (SCRWIDTH - len) >> 1;
  for(i = 2; i + 15 <= SCRHEIGHT; i += 2) { //2パス目
    for(j = initx; j + Block::length + 2 < SCRWIDTH; j += Block::length + 1) {
      if((stage << 1) <= STAGEMAX || genrand64_int64() > threshold) blocks.push_back(new Block(j, i));
      else blocks.push_back(new SolidBlock(j, i)); //後半のレベルでレベル/7の確率でSolidBlock(標準で赤色)
    }
  }
  DrawGameWin();
}
//ゲームオーバー
void gamekernel::gameover() {
  mode = OVER;
  scrclock = clock() + SCRCHTIME; //一定時間たったら自動でタイトルに戻るようにする
  if(ball != NULL) deleteparts(); //全部のオブジェクトを解放
  RefreshScrSize();
  clear();
  writedots(gameovertext, 7); //「GAME OVER」を表示
  PrintStrCenter(GetGuidY(), "S→スタート", 17); //タイトルに戻るに合わせる
  PrintStrCenter(GetGuidY() + 1, "Q→終了", 17); //タイトルに戻るに合わせる
  PrintStrCenter(GetGuidY() + 2, "B→タイトルに戻る",17);
}
//出したスコアの順位を求める関数
int gamekernel::getRank() {
  for(int i = 0; i < 5; i++)
    if(score > hiscr[i].score) return i; //1位 = 0
  return -1; //ランク外
}
//ゲームクリア
void gamekernel::gameclear() {
  if(ball != NULL) deleteparts();
  RefreshScrSize();
  SetMode(CLEAR);
  clear();
  writedots(gamecleartext, 7);
  if((rank = getRank()) != -1) { //ハイスコア更新
    SetMode(INPNAME);
    stringstream rankinmes;
    rankinmes << rank + 1 << "位にランクインしました！";
    PrintStrCenter(GetGuidY(),rankinmes.str(),26); //一番下に合わせる
    PrintStrCenter(GetGuidY() + 1, "名前を入力してください" , 26); //これも
    PrintStrCenter(GetGuidY() + 2, "Enter→登録 F3→キャンセル", 26);
    hisnamebuf = new string();
  } else {
    scrclock = clock() + SCRCHTIME; //一定時間後に自動でタイトル画面に切り替わるように
    PrintStrCenter(GetGuidY(), "S→スタート",17);
    PrintStrCenter(GetGuidY() + 1, "Q→終了", 17);
    PrintStrCenter(GetGuidY() + 2, "B→タイトルに戻る", 17);
  }
}
//タイトルに戻る
void gamekernel::backtostart() {
  SetMode(TITLE);
  if(ball != NULL || bar != NULL) deleteparts();
  RefreshScrSize();
  clear();
  writedots(titletext, 7); //タイトルロゴを表示
  PrintStrCenter(GetGuidY(), "S→スタート Q→終了",27);
  PrintStrCenter(GetGuidY() + 1, "R→インターネットランキング",27);
  PrintStrCenter(GetGuidY() + 2, "L→ローカルランキング",27);
}
void gamekernel::ScrSizeTooSmall() {
  SetMode(SCRSIZETOOSMALL);
  clear();
  mvaddstr(0,0,"画面が小さすぎます。\nサイズを変更して\nもう一度Sキーを押してください。\n(B→タイトル/Q→終了)");
}
//キー入力受け付け
bool gamekernel::KeyAcc() {
  if(mode == INPNAME) {
    switch(keycode = getch()) {    
    case '\n': //スコア登録
      //何も入力されていない場合は抜ける
      if(hisnamebuf->length() == 0) break;
      for(int i = 4; i > rank;i--) { //下位のデータを下にずらす
	hiscr[i] = hiscr[i - 1];
      }
    hiscr[rank].score = score; //記録挿入
    strcpy(hiscr[rank].name,hisnamebuf->c_str());
    DeletePt(hisnamebuf); //名前用の領域を解放
    ConfirmSubmit(); //インターネットランキングに登録するか確認する
    break;
    case KEY_BACKSPACE: //1字消す
      if(!hisnamebuf->empty()) {
	hisnamebuf->erase(hisnamebuf->end() - 1); //空でなかったら末尾を消去
	mvaddch(GetGuidY() + 3, (COLS - 5 >> 1) +  hisnamebuf->length(), ' '); //末尾の文字の表示をスペースで置き換え
      }
      break;
    case KEY_F(3): //キャンセル
      DeletePt(hisnamebuf); //名前用の領域を解放
      backtostart(); //タイトルに戻る
      break;
    default:
      if(keycode >= '0' && keycode <= '9' || keycode >= 'A' && keycode <= 'z' || keycode >= 'A' && keycode <= 'Z' || keycode == '-' || keycode <= '.' && keycode >= ',') {
	if(hisnamebuf->size() < 5) { //入力できるなら
	  *hisnamebuf += keycode; //文字を追加
	  mvaddstr(GetGuidY() + 3,COLS - 5 >> 1, hisnamebuf->c_str()); //表示
	}
      }
    }
  } else if(mode == SUBMITCONFIRM) { //送信するか確認する画面
    switch(keycode = getchar()) {
    case 'y': //スコア送信
      SubmitScore();
      break;
    case 'n': //タイトルに戻る
      backtostart();
      break;
    }
  } else { //その他
    switch(keycode = getch()) {
    case 'q': //終了
      return false;
    case 's': //ゲームスタート
      if(!isPlayingGame() && mode != RANKING) gamestart();
      break;
    case 'b': //タイトルに戻る
      if(mode == OVER || mode == CLEAR || mode == RANKING || mode == SCRSIZETOOSMALL || isPlayingGame()) backtostart();
      break;
    case 'g': //チートモード限定: バーを大きくする
      if(isPlayingGame() && cheatmode) bar->enlarge(2, ball);
      break;
    case 'a': //チートモード限定: バーを小さくする
      if(isPlayingGame() && cheatmode) bar->shrink(2);
      break;
    case 'f': //チートモード限定: バーの長さをMAXにする
      if(isPlayingGame() && cheatmode) bar->fulllength(ball);
      break;
    case 'e': //チートモード限定: ボールのスピードを上げる
      if(isPlayingGame() && cheatmode && ballmoveint > BLMSUB) {
	ballmoveint -= BLMSUB;
	if(ball != NULL) ball->ReflectSpeed();
      }
      break;
    case 'w': //チートモード限定: ボールのスピードを下げる
      if(isPlayingGame() && cheatmode && ballmoveint + BLMSUB <= BLMINIT * 4) {
	ballmoveint += BLMSUB;
	if(ball != NULL) ball->ReflectSpeed();
      }
      break;
    case 'r': 
      if(mode == TITLE) ShowNetRanking(); //インターネットランキングを見る
      break;
    case 'l': //ローカルランキングを見る
      if(mode == TITLE) ShowLocalRanking();
      break;
    case 'x':
    case KEY_LEFT: //左
      if(isPlayingGame() && (!holdmode || bar->getX() > 1 && holddir != LEFT)) { //ホールドモードでない・バーが動けて左向きでない
	bar->move(LEFT);
	if(mode == PLAYING) bar->ballexcludetoleft(ball); //ボールを右から叩きつけて上向きに打ち返すチート防止
	if(holdmode) { //ホールドモードの時
	  holddir = LEFT;
	  barmoveclk = clock() + BARMOVEINT;
	}
      }
      break;
    case 'v':
    case KEY_RIGHT: //右
      if(isPlayingGame() && (!holdmode || bar->getX() + bar->getLen() + 1 < SCRWIDTH && holddir != RIGHT)) {
	bar->move(RIGHT);
	if(mode == PLAYING) bar->ballexcludetoright(ball); //ボールを左から叩きつけて上向きに打ち返すチート防止
	if(holdmode) {
	  holddir = RIGHT;
	  barmoveclk = clock() + BARMOVEINT;
	}
      }
      break;
    case'd':
    case KEY_UP: //上
      if(isPlayingGame() && (!holdmode || (cheatmode && bar->getY() >= SCRHEIGHT - 12 || !cheatmode && bar->getY() >= SCRHEIGHT - 8) && holddir != UP)) {
	bar->move(UP);
	if(mode == PLAYING) bar->ballexcludetotop(ball); //ボール貫通防止
	if(holdmode) {
	  holddir = UP;
	  barmoveclk = clock() + BARMOVEINT;
	}
      }
      break;
    case 'c':
    case KEY_DOWN: //下
      if(isPlayingGame() && (!holdmode || bar->getY() <= SCRHEIGHT - 2 && holddir != DOWN)) {
	bar->move(DOWN);
	if(mode == PLAYING) bar->ballexcludetobottom(ball); //ボールを上から叩きつけて上向きに打ち返すチート防止
	if(holdmode) {
	  holddir = DOWN;
	  barmoveclk = clock() + BARMOVEINT;
	}
      }
      break;
    case 'z': //ホールドモード
      if(isPlayingGame()) {
	if(holdmode) { //ホールドモードを解除する
	  holdmode = false;
	} else { //ホールドモードにする
	  holddir = STOP;
	  holdmode = true;
	}
      }
      break;
    case ' ': //ボール発射
      if(mode == BALLWAIT) newballresume();
      break;
    }
  }
  return true;}
//ホールドモードの時バーを自動的に動かす関数
void gamekernel::BarAutoMove() {
  if(isPlayingGame() && holdmode && holddir != STOP && barmoveclk <= clock()) {
    barmoveclk = clock() + BARMOVEINT; //次の時刻を設定
    int pos = (holddir == UP || holddir == DOWN) ? bar->getY() : bar->getX();
    bar->move(holddir);
    if(mode == PLAYING) switch(holddir) { //ボールがあればバーから押しのける
      case UP:
	bar->ballexcludetotop(ball);
	break;
      case DOWN:
	bar->ballexcludetobottom(ball);
	break;
      case RIGHT:
	bar->ballexcludetoright(ball);
	break;
      case LEFT:
	bar->ballexcludetoleft(ball);
	break;
      }
    if(pos == ((holddir == UP || holddir == DOWN) ? bar->getY() : bar->getX())) holddir = STOP; //もう動けない
  }
}
//ボールとブロックの当たりを判定する関数(ボール移動処理後)
bool gamekernel::CheckHit() {
  list<Block*>::iterator it;
  BYTE hitflag;
  for(it = blocks.begin(); it != blocks.end(); it++) {
    switch(hitflag = (*it)->isHit(ball)) { //ブロックにボールが当たったか
    case 4:
    case 6: //X・Y両方向の反射(角に当たる)
	ball->reflect(true, true);
	ball->setloc(ball->getX() + ball->getdirX(), ball->getY() + ball->getdirY());
	break;
    case 1: //Y方向の反射
	ball->reflect(false, true);
	ball->setloc(ball->getX(), ball->getY() + ball->getdirY());
	break;
    case 0:
      continue; //当たってない
    }
    //以下ブロック当たり判定
    beep();
    if((*it)->Hit()) {    //ブロック消去
      delete *it;
      it = blocks.erase(it);
      //点数加算
      score += stage * (life * 5 + 5);
      refreshScore();
      //もう残りのブロックがない
      if(blocks.empty()) {
	if(stage == STAGEMAX) {
	  gameclear();
	} else {
	  nextstage();
	  return true;
	}
      }
    } else { //ブロックにまだライフがある(SolidBlock)
      score += 5;
      refreshScore();  
    }
  }
  return false;
}
//ボールを動かす処理を呼び出す関数・兼ミス判定
//返り値: 次にブロックとの当たり判定を行うか
bool gamekernel::MoveBalls() {
  if(mode != PLAYING && mode != PAUSE) return false;
  if(ball->nextmove()) {
    ball->move(bar); //ボールを動かす
    //ボールが落ちた
    if(ball->recalled) {
      if(life > 0) {
	//Miss!の文字の座標(仮)
	int ltrx = ball->getX() - 2;
	int ltry = SCRHEIGHT - 1;
	life--; //ライフ減
	refreshLife(); //表示更新
	DeletePt(ball); //ボール消去
	//Miss!の表示が壁に重なるとき
	if(ltrx + 5 >= SCRWIDTH) ltrx = SCRWIDTH - 6;
	if(ltrx <= 0) ltrx = 1;
	//Miss!の表示がバーに重なるとき
	if(bar->getY() == SCRHEIGHT - 1 && ltrx + 4 >= bar->getX() && ltrx < bar->getX() + bar->getLen()) ltry--;
	//Miss!の表示を消す時間を設定
	scrclock = clock() + PAUSETIME;
	SetMode(PAUSE);
	//Miss!を表示
	letter = new Letter(ltrx,ltry,"Miss!",1);
      }
      else { //もうやめて！プレーヤーのライフはゼロよ！
	gameover();
      }
      return false;
    }
  }
  return true; //ブロックとの当たり判定を行う
}
//CGIとHTTP通信をする関数
//getdata: サーバから受け取ったバイナリデータを格納する変数
//query:   サーバに送信するデータ(POST; エンコード済み)
//返り値: エラーが起きたらfalse
bool gamekernel::HTTPCommunicate(string &getdata, string query) {
  int s;                               /* ソケットのためのファイルディスクリプタ */
  struct hostent *servhost;            /* ホスト名と IP アドレスを扱うための構造体 */
  struct sockaddr_in server;           /* ソケットを扱うための構造体 */
  struct servent *service;             /* サービス (http など) を扱うための構造体 */
  int BUF_LEN = 256; //バッファサイズ
  
  string send_buf;              /* サーバに送る HTTP プロトコル用バッファ */
  string host(RANKINGCGIHOST);    /* 接続するホスト名 */
  string path(RANKINGCGIPATH);            /* 要求するパス */
  /* ホストの情報(IPアドレスなど)を取得 */
  servhost = gethostbyname(host.c_str());
  if ( servhost == NULL ){
    DispRankingError("インターネットランキング","通信エラー(サーバのIPアドレスがわかりませんでした)");
    return false;
  }

  bzero(&server, sizeof(server));            /* 構造体をゼロクリア */

  server.sin_family = AF_INET;

  /* IPアドレスを示す構造体をコピー */
  bcopy(servhost->h_addr, &server.sin_addr, servhost->h_length);

  server.sin_port = htons(RANKINGCGIPORT); //ポート番号指定
  /* ソケット生成 */
  if ( ( s = socket(AF_INET, SOCK_STREAM, 0) ) < 0 ){
    DispRankingError("インターネットランキング","ソケットの生成ができませんでした");
    return false;

  }

  /* サーバに接続 */
  if ( connect(s, (struct sockaddr *)&server, sizeof(server)) == -1 ){
    DispRankingError("インターネットランキング","サーバに接続できませんでした");
    return false;
  }

  stringstream sendstream;
  /* HTTP プロトコル生成 & サーバに送信 */
  sendstream << "POST "<< path << " HTTP/1.1\r\n";
  sendstream << "User-Agent: Breakout\r\n";
  sendstream << "Accept: application/octet-stream\r\n";
  sendstream << "Content-Length: " << query.length() << "\r\n";
  sendstream << "Host: " << host << ":" << RANKINGCGIPORT << "\r\n\r\n";
  //ヘッダ出力ここまで

  //サーバに送信するデータを付加
  sendstream << query;

  send_buf = sendstream.str();
  
  //送信
  write(s, send_buf.c_str(), send_buf.length());

  string receivedtxt; //ここに受信テキストを溜める
  //受信する部分
  while (1){
    char buf[BUF_LEN];
    int read_size;
    read_size = read(s, buf, BUF_LEN);
    if ( read_size > 0 ){ //受信できたら
      receivedtxt.append( buf, read_size); //適宜追加
    } else {
      break;
    }
  }
  /* 後始末 */
  close(s);

  
  stringstream receivestream(receivedtxt);
  string linebuf;
  int pos;
  getline(receivestream, linebuf);
  //最初の行は「HTTP/1.1 200 OK」のはず(200が入ってる)
  if((pos = linebuf.find("200")) == -1) { //200でない
    string err("通信エラー(");
    err.append(linebuf.substr((pos = linebuf.find("HTTP/")) + 9,linebuf.length() - (linebuf.find('\r') == linebuf.length() - 1 ? 10 : 9)));
    //(エラー番号) (メッセージ)　を切り取って追加
    err.append(")");
    DispRankingError("インターネットランキング", err.c_str());
    return false;
  }
  //Content-Type: application/octet-streamがある行を探す
  while(1) {
    if(receivestream.eof()) { //ない場合
      DispRankingError("インターネットランキング","通信エラー(受信テキストのMIMEタイプエラー)");
      return false;
    }
    getline(receivestream, linebuf);
    if((pos = linebuf.find("Content-Type: application/octet-stream")) != -1) {
      break;
    }
  }
  string structstr;
  //空行を探す(その直後が構造体のデータ)
  while(1) {
    if(receivestream.eof()) { //ない場合
      DispRankingError("インターネットランキング","通信エラー(受信テキストにデータなし)");
      return false;
    }
    getline(receivestream, linebuf);
    if(linebuf.length() == 0 || linebuf.compare("\r") == 0) 
      break;
  }
  //あとは構造体(+「\r」)が残るはず(1行長いのがくるはず)
  while(1) {
    getline(receivestream, linebuf);
    if(linebuf.length() > sizeof(ScoreData) * 2) break;
    if(receivestream.eof()) {
      DispRankingError("インターネットランキング","通信エラー(受信テキストにデータなし)");
      return false;
    }
  }

  structstr = linebuf.substr(0, linebuf.find("\r"));
  //URLエンコードされたデータを抜き出し
  HTTPLib::URLDec(getdata, structstr);
  //生データに変換
  return true;
}
//スコアを送信するか確認する関数
void gamekernel::ConfirmSubmit() {
  SetMode(SUBMITCONFIRM);
  clear();

  PrintStrCenter(1,"オンラインランキングに登録しますか？",36);
  PrintStrCenter(2,"y→登録する　n→登録しない",36);
  refresh();
}
//スコアをサーバに送信する関数
void gamekernel::SubmitScore() {
  string query("from="), structraw, submitscore((char*)&hiscr[rank], sizeof(ScoreData));
  query += HTTPLib::URLEnc("ブロック崩しプログラム")
    + "&mode=" + HTTPLib::URLEnc("書き込み")
    + "&data=" + HTTPLib::URLEnc(submitscore); //サーバに送信するデータを生成
  //通信中を表示
  clear();
  mvaddstr(1,1,"通信中・・・");
  refresh();
  if(HTTPCommunicate(structraw, query)) {
    //インターネットランキングの順位
    int netrank;
    memcpy((void *)&netrank, (const void*)structraw.data(), sizeof(int));
    //インターネットランキングの現在の記録
    int n = (structraw.length() - sizeof(int)) / sizeof(ScoreData); //何位まで表示するか
    ScoreData netrecords[n]; //ランキング用の配列確保
    //生データを構造体領域にコピー(これで読み出せる形になる)
    memcpy((void *)netrecords, (const void*)(structraw.data() + sizeof(int)), structraw.length() - sizeof(int));
    string mes;
    if(netrank <= -1) { //ランク外の時
      mes.assign("残念ながらランク外でした");
    } else { //ランクインした時
      stringstream messtream;
      messtream << netrank + 1 << "位に入りました。おめでとう！";
      mes.assign(messtream.str());
    }
    DispRanking(netrecords, n, mes.c_str()); //メッセージ・ランキングを表示
  }
}
//ネットランキングを表示する関数
void gamekernel::ShowNetRanking() {
  string query("from="), structraw;
  query += HTTPLib::URLEnc("ブロック崩しプログラム");
  //通信中を表示
  clear();
  mvaddstr(1,1,"通信中・・・");
  refresh();
  if(HTTPCommunicate(structraw, query)) {
    int n = structraw.length() / sizeof(ScoreData); //何位まで表示するか
    ScoreData netrecords[n];
    //生データを構造体領域にコピー(これで読み出せる形になる)
    memcpy((void *)netrecords, (const void*)structraw.data(), structraw.length());
  
    DispRanking(netrecords, n, "インターネットランキング"); //ランキング表示
  }

}
//ローカルランキング
void gamekernel::ShowLocalRanking() {
  DispRanking(hiscr, 5, "ローカルランキング"); //ランキング表示
}
//ランキング表示関数
void gamekernel::DispRanking(ScoreData *records, int number,const char* title) {
  clear();
  mvaddstr(1,2,title); //タイトル表示
  int i = 0;
  SetMode(RANKING);
  if(records != NULL && number > 0) { //データがあるとき
    for(i = 0; i < number; i++) {
      mvprintw(3 + i, 2, "%2d位: %5s(%d点)",i + 1,records[i].name, records[i].score); //出力
    }
  }
  mvaddstr(4 + i, 2, "B→タイトル Q→終了");
}
//ランキング読み込みエラーの表示関数
void gamekernel::DispRankingError(const char *title, const char *error) {
  clear();
  mvaddstr(1,2,title); //タイトル表示
  SetMode(RANKING);
  mvaddstr(3, 2, error); //エラー文表示
  mvaddstr(5, 2, "B→タイトル Q→終了");
}
//Miss!の表示を消す関数
void gamekernel::deletemissdisp() {
  DeletePt(letter); //これでMiss!の表示が消える
  SetMode(BALLWAIT); //ボール発射待機
  bar->reDraw();
  msgrb = new Letter(SCRWIDTH + 1, SCRHEIGHT - 2, "Press SPC",0); //発射ガイド
  msgrb2 = new Letter(SCRWIDTH + 1, SCRHEIGHT - 1, "to Shoot", 0);
}
//枠・各種情報表示関数
void gamekernel::DrawGameWin() {
  int i;
  list<Block*>::iterator it, en;
  //壁の描画
  mvaddch(0,0,WALL_C); //左上隅
  //上
  for(i = 1;i < SCRWIDTH - 1; i++) {
    addch(WALL_H);
  }
  addch(WALL_C); //右上隅
  for(i = 1;i < SCRHEIGHT; i++) {
    mvaddch(i,0,WALL_V); //左
    mvaddch(i,SCRWIDTH - 1, WALL_V); //右
  }
  //各種データ表示
  mvaddstr(2, SCRWIDTH,"Time");
  mvaddstr(3, SCRWIDTH,"Level");
  mvaddstr(4, SCRWIDTH,"Score");
  mvaddstr(5, SCRWIDTH, "Ball");
  mvaddstr(6, SCRWIDTH, "HighS");
  mvaddstr(7, SCRWIDTH, "Name");
  refreshTime(); //ここで経過時間を+1
  refreshScore();
  refreshLife();
  refreshLevel();
  refreshHiScore();
  msgrb = new Letter(SCRWIDTH + 1, SCRHEIGHT - 2, "Press SPC", 0); //発射ガイド
  msgrb2 = new Letter(SCRWIDTH + 1, SCRHEIGHT - 1, "to Shoot", 0);
}

//コンストラクタ
gamekernel::gamekernel(bool ctmd) {
  if(!LoadData()) { //読み込めなかったとき
    ScoreDataInit(hiscr, 5); //初期化
  }
  cheatmode = ctmd; //チートモード設定
  backtostart(); //タイトル表示
}
//デストラクタ
gamekernel::~gamekernel() {
  deleteparts();
  SaveData();
}
//メインループ
void gamekernel::mainloop() {
  while (1) {
    if(!KeyAcc()) break;
    BarAutoMove();
    //ボール関連の処理
    if(mode == PLAYING && ball->nextmove()) {
      if(MoveBalls()) CheckHit();
      //画面の自動切換
    } else if((mode == CLEAR || mode == OVER) && clock() >= scrclock) {
      backtostart();
      //ミス表示を消してボール再発射待機
    } else if(mode == PAUSE && clock() >= scrclock) {
      deletemissdisp();
    }
    //経過時間を進める
    if(mode == PLAYING) {
      if(clock() >= nextsec) {
	refreshTime();
      }
    }
  }
}
//ボール再発射(バーの真上中央から)
void gamekernel::NewBall() {
  ball = new Ball(bar->getcenterX(),bar->getY() - 1);
}
//ボール再発射してゲーム再開
 void gamekernel::newballresume() {
   NewBall();
   SetMode(PLAYING);
   DeletePt(msgrb);
   DeletePt(msgrb2);
 }
//モードを設定
void gamekernel::SetMode(gamemode newmode) {
  mode = newmode;
}
//以下オブジェクト消去
void gamekernel::clearObjDisp(int x, int y, int len) {
  move(y,x);
  for(int i = 0; i < len; i++) addch(' ');
}
//以下オブジェクト描画関数
void gamekernel::drawObj(int x, int y, char ch) {
  mvaddch(y, x, ch);
}
void gamekernel::drawObj(int x, int y, string str) {
  mvaddstr(y, x, str.c_str());
}
void gamekernel::drawObj(int x, int y, char ch, int col) {
  attrset(COLOR_PAIR(col));
  drawObj(x, y, ch);
  attrset(0);
}
void gamekernel::drawObj(int x, int y, string str, int col) {
  attrset(COLOR_PAIR(col));
  drawObj(x, y,str);
  attrset(0);
}
//スコア読み込み(ローカル)関数
bool gamekernel::LoadData() {
  ifstream st;
  st.open(datafilename.c_str(), ios::binary);
  if(!st) return false; //開けない
  st.read((char *)&hiscr, sizeof(hiscr)); //構造体配列を生で読み込み
  return !st.fail();
}
//スコア保存関数
void gamekernel::SaveData() {
  ofstream st;
  st.open(datafilename.c_str(), ios::binary);
  if(st.bad()) return;
  st.write((char *)&hiscr, sizeof(hiscr)); //構造体配列を書込
}
//各種情報の更新
void gamekernel::refreshScore() {
  mvprintw(4, SCRWIDTH + 6,"%05d", score);
}
void gamekernel::refreshLife() {
  mvprintw(5, SCRWIDTH + 6,"%05d", life);
  mvaddch(6, 0, WALL_V); //なぜか壁の一部が消えるので修復
}
void gamekernel::refreshHiScore() {
  if(hiscr[0].score >= 99999) //表示上はカンスト
    mvaddstr(6, SCRWIDTH + 6,"99999");
  else
    mvprintw(6, SCRWIDTH + 6,"%05d", hiscr[0].score);
  mvprintw(7, SCRWIDTH + 6,"%5s", hiscr[0].name);
}
//この関数だけ時間変数をいじっている
void gamekernel::refreshTime() {
  nextsec = clock() + CLOCKS_PER_SEC;
  if(stagetime < 99999)
    mvprintw(2, SCRWIDTH + 6,"%05d", ++stagetime);
  else {//表示上はカンスト
    mvaddstr(2, SCRWIDTH + 6, "99999");
    stagetime++;
  }
}
void gamekernel::refreshLevel() {
  mvprintw(3, SCRWIDTH + 6,"%05d", stage);
}
//ゲーム中かどうか返す関数
bool gamekernel::isPlayingGame() {
  if(mode == PLAYING || mode == PAUSE || mode == BALLWAIT) return true;
  return false;
}
//中央揃えで文字列を表示させる関数
void gamekernel::PrintStrCenter(int y, string str, int len) {
  if(len <= 0) len = str.length();
  int x = COLS - len >> 1;
  mvaddstr(y, x > 0 ? x : 0, str.c_str());
}
//ロゴの下の指示テキストの一番上のY座標を返す関数
int gamekernel::GetGuidY() {
  if(winsize == SMALL) {
    if(SCRHEIGHT >> 2 >= SCRHEIGHT - 7) return (SCRHEIGHT >> 2) + 1;
    return SCRHEIGHT - 7;
  } else {
    if(SCRHEIGHT <= 14) return 10;
    return SCRHEIGHT - 7; 
  }
}
