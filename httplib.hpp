/******************************
 //
 // HTTP通信用ライブラリ for C++
 //
 // 必須ライブラリ: cstdlib(stdlib.h), map, string
 // ※_HTTPLIB_CLIENT_を定義すればstringだけでOK
 //
 *******************************/

namespace HTTPLib {
  //0-9,A-F,a-fのみ対応(g-z、G-Zなどにも反応してしまう仕様)
  char HexToByte(char dgt) {
    if('9' >= dgt) { //数字
      return dgt - '0';
    } else if('F' >= dgt) { //A-F
      return dgt - 'A' + 10;
    } else { //a-f
      return dgt - 'a' + 10;
    }
  }

  //0-9,A-Zかどうか調べる関数
  bool isHex(char ch) {
    return ('f' >= ch && (ch >= 'a' || 'F' >= ch && (ch >= 'A' || '9' >= ch && ch >= '0')));
  }

  //URLエンコードされた文字列(例: %E3%81%93%E3%82%93%E3%81%AB%E3%81%A1%E3%81%AF)を元の文字列(例: こんにちは)に変換する関数
  void URLDec(std::string &out, std::string inp) {
    if(!out.empty()) out.clear(); //中に入っている場合は消去
    for(int i = 0, len = inp.length(); i < len; i++) {
      switch(inp[i]) { //文字によって場合分け
      case '%':
	if(i + 1 != len && isHex(inp[i + 1]) && i + 2 != len && isHex(inp[i + 2])) { //%XXの場合
	  out.push_back(HexToByte(inp[i + 1]) << 4 | HexToByte(inp[i + 2]));
	  i += 2;
	}
	else { //ただの%
	  out.push_back('%');
	}
	break;
      case '+': //「+」はスペース
	out.push_back(' ');
	break;
      case ' ': //「 」は無視
	break;
      default: //普通の文字
	out.push_back(inp[i]);
	break;
      }
    }
  }
  //返り値で結果を返すタイプ
  std::string URLDec(std::string inp) {
    std::string ret;
    URLDec(ret, inp);
    return ret;
  }
#ifndef _HTTPLIB_CLIENT_
  //URLエンコードされた文字列(例: greeting=%E3%81%93%E3%82%93%E3%81%AB%E3%81%A1%E3%81%AF)をmap配列に格納する関数(例: ["greeting"] = "こんにちは")
  void URLDec(std::string inp, std::map<std::string, std::string> &arr) {
    std::string keybuf, varbuf, *pt = &keybuf;
    bool isval = false; //=の後か(値かどうか)
    for(int i = 0, len = inp.length(); i < len; i++) {
      switch(inp[i]) {
      case '%':
	if(i + 1 != len && isHex(inp[i + 1]) && i + 2 != len && isHex(inp[i + 2])) {
	  pt->push_back(HexToByte(inp[i + 1]) << 4 | HexToByte(inp[i + 2]));
	  i += 2;
	}
	else {
	  pt->push_back('%');
	}
	break;
      case '&': //&は値→キーの切り替えのサイン
	if(isval) {
	  if(!pt->empty()) { //&key=val&
	    arr[keybuf] = varbuf;
	  } else { //&key=&
	    arr[keybuf] = "";
	  }
	  keybuf.clear();
	  isval = false;
	  pt = &keybuf;
	} else if(!pt->empty()) { //&key&
	  if(arr.find(keybuf) == arr.end()) arr[keybuf] = "";
	  keybuf.clear();
	} //&&→無視
	break;
      case '=': //=はキー→値の切り替えのサイン
	if(arr.find(keybuf) == arr.end()) {
	  varbuf.clear();
	  pt = &varbuf;
	  isval = true;
	} else { //重複する場合は最初に登録したもの優先
	  for(i++;inp[i] != '&' && i < len; i++); //「&」まで飛ばす
	  keybuf.clear();
	}
	break;
      case '+': //スペースとみなす
	pt->push_back(' ');
	break;
      case ' ': //無視でおｋ？
	break;
      default: //他の文字
	pt->push_back(inp[i]);
	break;
      }
    }
    //事後処理(&が来たのと同等)
    if(isval) {
      if(!pt->empty()) { //&key=val&
	arr[keybuf] = varbuf;
      } else { //&key=&
	arr[keybuf] = "";
      }
      keybuf.clear();
    } else if(!pt->empty()) { //&key&
      if(arr.find(keybuf) == arr.end()) arr[keybuf] = "";
      keybuf.clear();
    } //&&→無視
  }

  //GET/POSTで渡された文字列(例: greeting=%E3%81%93%E3%82%93%E3%81%AB%E3%81%A1%E3%81%AF)をmap配列に格納する関数(例: ["greeting"] = "こんにちは")
  void LoadQuery(std::map<std::string, std::string> &args) {
    char *tmp;
    std::string buf;
    std::string request_method;
    if((tmp = getenv( "REQUEST_METHOD" )) != NULL) { 
      request_method = tmp;
      if(request_method == "GET") buf = getenv("QUERY_STRING"); //GETの場合はgetenv(cstdlib)で抜き出す
      else if(request_method == "POST") std::cin >> buf; //POSTの場合は標準入力
      else buf = ""; //その他のメソッド
    } else buf = ""; //CGIとして呼ばれていない
    URLDec(buf, args); //デコードしてargsに格納
  }

  //メソッドタイプの列挙体定義
  enum MethodType {GET, POST, OTHER = -1};

  //メソッドの種類(GET/POST/その他)を返す関数
  MethodType GetMethod() {
    char *buf = getenv("REQUEST_METHOD");
    if(buf != NULL) {
      std::string buf2(buf);
      if(buf2.compare("GET") == 0) return GET;
      if(buf2.compare("POST") == 0) return POST;
      return OTHER;
    } else return OTHER;
  }
#endif
  //URL形式にエンコードする関数
  //例:\n→%0A
  void URLEnc(std::string &ret,std::string inp) {
    static std::string dgttbl("0123456789ABCDEF");
    if(!ret.empty()) ret.clear(); //何か入っていたら消去
    for(std::string::iterator it = inp.begin(), ob = inp.end(); it != ob; it++) {
      //英数字、!'()*-.:_~はそのまま(Windows版Chromeが基準)
      if(*it == '~' || *it <= 'z' && (*it >= 'a' ||  *it == '_' || *it <= 'Z' && (*it >= 'A' || *it <= ':' && (*it == '!' || *it >= '\'' && *it != '+' && *it != ',' && *it != '/')))) {
	ret.push_back(*it);
      } else if(*it != ' ') { //「 」でない要変換記号
	
	ret.push_back('%');
	ret.push_back(dgttbl[(unsigned char)(*it) >> 4]);
	ret.push_back(dgttbl[(unsigned char)(*it) & 15]);
      } else ret.push_back('+'); //「 」
    }
  }
  //返り値で結果を返すタイプ
  std::string URLEnc(std::string inp) {
    std::string ret;
    URLEnc(ret, inp);
    return ret;
  }
}
