/************************************/
// ランキングCGI(サーバサイド)
/***********************************/
// 仕様
//   プログラム外からのアクセスの場合は順位をHTML形式で出力
//   プログラム内からで
//     読み込みモードなら現在のランキングのバイナリをURLエンコーディングして出力
//     書き込みモーどならその前に順位を示すint型の数値を出力(バイナリ、URLエンコーディング)
//       首位が0、ランク外が-1
//   データを初期化したい場合はブラウザでURLの末尾に「?dataclean=yes」をつけてアクセス
//   ファイルロック機能はなし(衝突するとデータリセットがかかる)
/************************************/
//   プログラム内からのアクセスと判定する条件は
//   ・POSTメソッド
//   ・from=ブロック崩しプログラム
//   (・mode=書き込み)
/************************************/

#include <iostream>
#include <fstream>
#include <map>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include "httplib.hpp"
#include "scoredata.hpp"


#define DATAFILE "onlineranking.dat"
//↑データファイル名
#define MAX 10
//↑登録人数

using namespace std;
//データを読み込む関数
//引数: 読み込み先のScoreData構造体(ランキングデータ)の配列/要素数
//戻り値: 読み込めたらtrue
bool LoadData(ScoreData *records, int number) {
  ifstream st;
  st.open(DATAFILE, ios::binary);
  if(!st) return false;
  st.read((char *)records, sizeof(ScoreData) * number);
  return !st.fail();
}
//データを書き込む関数
//引数: LoadDataと同じ
void SaveData(ScoreData *records, int number) {
  ofstream st;
  st.open(DATAFILE, ios::binary);
  if(st.bad()) return;
  st.write((char *)records, sizeof(ScoreData) * number);
}


typedef std::map<std::string, std::string> strpair;
int main() {
  strpair args; //ブラウザ・ブロック崩しプログラムから渡されるデータ格納用
  ScoreData records[MAX]; //スコアデータの配列
  bool canload = LoadData(records,MAX); //データファイル読み込み
  HTTPLib::LoadQuery(args); //ブラウザ等からのデータ読み込み
  bool datacleaning = HTTPLib::GetMethod() == HTTPLib::GET && args.find("dataclean") != args.end();
  //「?dataclean=yes」がURL末尾についてアクセスされていないか
  if(!canload || datacleaning) ScoreDataInit(records,MAX);
  if(HTTPLib::GetMethod() != HTTPLib::POST || args.find("from") == args.end()  || args["from"] != "ブロック崩しプログラム") { //ブロック崩し以外のプログラムからアクセス
    //↓HTML出力
    cout << "Content-Type: text/html; charset=UTF-8\nContent-Language: ja\n\n";
    cout <<"<!DOCTYPE html>\n<html lang=\"ja\">\n  <head>\n    <title>ブロック崩し　オンラインランキング</title>\n  </head>\n  <body>\n    <h1>ブロック崩し　オンラインランキング</h1>\n    <section>\n";
    cout <<"      <table border>\n	<tr>\n	  <th>順位</th>\n	  <th>名前</th>\n	  <th>得点</th>\n	</tr>\n";    for(int i = 0; i < MAX; i++) { //レコード出力
      if(records[i].score <= 0) continue;
      cout << "	<tr>\n	  <td>" << i + 1 << "位</td>\n	  <td>"<< records[i].name <<"</td>\n	  <td>" << records[i].score <<"点</td>\n	</tr>\n";
    }
    cout << "      </table>\n";
    cout << "    </section>\n";
    if(datacleaning) cout << "  <p>データを初期化しました</p>\n";
    cout <<"  </body>\n</html>\n";
    if(!canload || datacleaning) SaveData(records, MAX);
  } else { //ブロック崩しプログラムから読み込まれた
    
    std::stringstream bufstream; //一旦こっちに吐き出させる
    if(args.find("mode") != args.end() && args["mode"] == "書き込み" && args.find("data") != args.end()) { //書き込みモード
      //ブロック崩しから渡されたスコア・名前
      ScoreData inprcd = *((ScoreData *)args["data"].data());
      int rank = -1;
      string rankbin,escapedrankbin; //バイナリ変換用
      for(int i = 0; i < MAX; i++) //ランクを計算
	if(inprcd.score > records[i].score) {
	  rank = i;
	  break;
	}
      if(rank != -1) { //ランクイン
	for(int i = MAX - 1; i > rank;i--) { //既存データをどける
	  records[i] = records[i - 1];
	}
	//割り込ませる
	records[rank].score = inprcd.score;
	strcpy(records[rank].name,inprcd.name);
	SaveData(records, MAX);

      } else if(!canload) SaveData(records, MAX); //ランク外でも最初に作るときは書き込み処理を行う
      rankbin.assign((char *)&rank,sizeof(rank)); //順位のバイナリデータを移す
      HTTPLib::URLEnc(escapedrankbin, rankbin); //URLンコード
      string data((char *)records, sizeof(records)), percentaged; //dataに現在の記録のバイナリデータを移す
      HTTPLib::URLEnc(percentaged, data); //URLエンコードしてpercentageに格納
      bufstream << escapedrankbin << percentaged;
      
    } else { //読み込みモード
      string data((char *)records, sizeof(records)), percentaged; //dataに現在の記録のバイナリデータを移す
      HTTPLib::URLEnc(percentaged, data); //URLエンコードしてpercentageに格納
      bufstream << percentaged;
      if(!canload) SaveData(records, MAX);
    }
    std::string outstr = bufstream.str();
    cout << "Content-Length: " << outstr.length() << "\r\n"; //チャンク通信(実装が面倒)しないようにするおまじない
    cout << "Content-Type: application/octet-stream\r\n\r\n";
    cout << outstr;
  }
  return 0;
}
  
