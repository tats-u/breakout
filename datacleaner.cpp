#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include "scoredata.hpp"
using namespace std;
static string datafilename("breakout.dat");
int main() {
  string buf;
  cerr << "データファイル「" << datafilename << "」を初期化しますか？(y/other): ";
  getline(cin, buf); //入力受付
  if(buf == "y" || buf == "Y") { //初期化する
    ScoreData data[5];
    ScoreDataInit(data, 5); //初期化データを作成
    ofstream st;
    st.open(datafilename.c_str(), ios::binary);
    if(st.bad()) { //開けない
      cerr << "開けませんでした" << endl;
      return 1;
    }
    st.write((char *)data, sizeof(ScoreData) * 5); //書き込み
    cerr << "初期化しました" << endl;
  }
  return 0;
}
