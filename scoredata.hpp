//記録管理用構造体(ブロック崩し・ランキングCGI共通)
typedef struct {
  int score; //スコア
  char name[6]; //名前
} ScoreData;

//初期化用(cstring必須)
void ScoreDataInit(ScoreData* data, int n) {
  memset((void*)data,0,sizeof(ScoreData) * n); //数字ごと0で初期化
  for(int i = 0; i < n; i++) {
    memset((void*)data[i].name,'-',5); //「-----」
  }
}
