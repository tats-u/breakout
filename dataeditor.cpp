#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cctype>
#include <cstring>
#include "scoredata.hpp"
using namespace std;
//データファイル名
static string datafilename("breakout.dat");
//ファイルを開く関数
//返り値: 開けたらtrue
bool LoadData(ScoreData *hiscr) {
  ifstream st;
  st.open(datafilename.c_str(), ios::binary);
  if(!st) return false; //開けない
  st.read((char *)hiscr, sizeof(ScoreData) * 5);
  return !st.fail();
}
//ファイルを閉じる関数
void SaveData(ScoreData *hiscr) {
  ofstream st;
  st.open(datafilename.c_str(), ios::binary);
  if(st.bad()) return; //開けない
  st.write((char *)hiscr, sizeof(ScoreData) * 5);
}

int main() {
  string buf, tmp;
  ScoreData hiscr[5];
  char ch;
  if(!LoadData(hiscr)) { //開けない
    cerr << "データファイル「" << datafilename << "」が読み込めませんでした\n";
    return 1;
  }
  
  cout << "現在の設定:\n";
  for(int i = 0; i < 5; i++) {
    cerr <<i + 1 << "位\t" << hiscr[i].score << "点\t" <<  hiscr[i].name << endl;
  }
 rewrite:
  cerr << "変更しますか？(1～5 / other) :";
  getline(cin,buf); //入力受付
  ch = buf[0];
  if(ch >= '1' && ch <= '5') {
    ch = ch - '1'; //配列アドレス参照用
    cerr << "得点を入力してください(q/b/c: 中断): ";
  reinp:
    getline(cin,buf); //入力受付
    if(!::all_of(buf.begin(), buf.end(), ::isdigit)) { //数字以外が入ってる
      transform(buf.begin(), buf.end(), buf.begin(), ::tolower); //q,b,cを想定してすべて小文字化
      if(buf.compare("q") && buf.compare("b") && buf.compare("c")) {
	cerr << "数字を入力してください。\n再入力(q/b/c: 中断): ";
	goto reinp;
      } else { //q,b,c
	return 0;
      }
    }
    hiscr[ch].score = std::stoi(buf);
    cerr << "最高得点者の名前を入力してください: ";
    getline(cin,buf); //入力受付(一時的にバッファに溜める)
    strcpy(hiscr[ch].name,buf.substr(0,5).c_str()); //切り出して格納
    SaveData(hiscr); //保存
    cerr << "変更完了しました\n他の箇所も";
    goto rewrite;
  }
  return 0;
}
