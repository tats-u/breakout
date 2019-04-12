# ブロック崩し

本プログラムは2014年に受講した講義、「プログラミング及び演習」のプログラミングプロジェクト課題にて制作した、CUI上で動くブロック崩しである。講義にて提示されたプログラムを流用せず、独自エンジンを使用したこと(=著作権問題がない)、及び翌々年から制作対象となるゲームが変わったこと(=後輩にとってのカンニングペーパーとならない)が幸いし、公開できた。

## 遊び方

### 標準モード

1. `block` とコマンドラインで入力して起動する。あまりにも端末画面が小さいとエラーが出る。その場合は再起動が必要である。
2. Sキーでゲームを開始できる。L・Rキーでランキングが見れる。終了はQキーで可能である。
3. Sキーを押してもすぐにはボールが発射されない。発射するにはスペースキーを押す必要がある。矢印キー・X・C・D・Vキーでラケットを移動できる。C・D・上下キーで上下にも移動できる。Zキーを押すとホールドモードになり、その後カーソルキーを押すとお下方向に移動し続ける。ホールドモードを解除するには、もう一度Zキーを押す。ゲーム中にBキーを押すとタイトルに戻り、Qキーを押すと終了する。
4. すべてのブロックにボールを当てて消去すると次のレベルに移る。後半のレベルは通常の紫色のブロックに混じって赤色のブロックが登場する。これは2回当てないと消えないブロックである。1回当てると表示が変わるので、もう1回当てる必要がある。また、レベルが上がると、
・ボールが速くなる
・ラケットが小さくなる
・赤ブロックの数が増える
・得点が増えやすくなる
このような変化が起きる。そのため、難しい後半のミスをなるべく少なくすれば、高得点が望める。
5. クリアするとスコア登録を求められる場合がある。BackSpaceキーで1文字消すことができる。オンラインランキングにも登録することができる。


### 本ゲームの癖

このゲームはいくつかの仕様上の癖があり、慣れないうちは特にその癖によりミスをしやすくなっている。

1. ボールが角に当たった時の挙動  
  ボールがブロックに斜め横から当たった場合、横方向にも反射する。そのため、ある意味不意打ちになり、以下の要因と合わさって非常にミスをしやすくなっている。
2. ラケットの高速移動の遅延  
  このゲームではCUIであるため、ある程度方向キーを押し続けないとラケットが高速移動をしない。そのため、ボールの動きにラケットが追いつかず、ミスしやすくなっている。また、高速移動中に他のキーを押すとストップしてしまう。そのため、ミスが起きてしまうこともある。この仕様はホールドモードである程度解消できる。
3. ラケットの上下移動  
  このゲームではラケットを上下移動できる。そのため、あまりにも調子に乗ってラケットを上に持ってくると①の「不意打ち」に対処できなくなる。

以上の要因が複雑に絡み合ってこのゲームの難易度が高くなっている。そのため、どうしてもクリアできないときは `block.cpp` のオプションを変更したり、以下のチートモードを利用したりするのも1つの手段である。

### チートモード

このゲームにはチートモードという隠しモードが用意されている。チートモードで起動する場合は「`block cheat`」と `cheat` オプションを付ける。すると次のような変化が起きる。

1. タイトルロゴなどの色の変化  
  通常モードでは赤色であるが、チートモードでは青色になる。
2. ラケットの手動伸縮  
  通常モードでは、ラケットを手動で伸縮することはできないが、チートモードでは以下のような操作で伸縮が可能である。
    - Aキー: 半角2文字分縮める
    - Gキー:     〃     伸ばす
    - Fキー: 長さをMAXにする
3. ボールの手動速度変更  
  通常モードではボールの速度は手動で変更することはできないが、チートモードでは速くすることも遅くすることも可能である。Eキーで速く、Wキーで遅くなる。
4. 上下移動の幅拡大  
  チートモードでは通常モードよりも上にラケットを動かせる。
5. ラケット幅の自動収縮なし  
  通常モードではレベルアップ時にラケット幅が自動で小さくなるが、チートモードでは2.の関係で自動でサイズを変更しないようになっている。

## セーブデータの編集

### データエディタ

セーブデータを修正するにはデータエディタ(dataeditor)を使う。起動すると現在のデータが表示される。n位のデータ(n∈ℕ)を変更したい場合はnを入力する。そして得点→名前の順に入力する。別のデータも変更したい場合はこれを繰り返し、終了したい場合は他の文字を入力してから、もしくは何も入力せずにEnterキーを押して終了する。

### データクリーナー

セーブデータを初期化したい場合にはデータクリーナー(`datacleaner`)を使う。起動するとデータを初期化するか聞かれるので、初期化する場合には「y」を入力する。

## ビルド方法

まず、必要なライブラリを `apt` でインストールする。

```bash
# Ubuntu
sudo apt install g++ make libncurses5-dev libncursesw5-dev

# CentOS
sudo yum install gcc-c++ make ncurses-devel
```

その後、このディレクトリで `make` する。 `make` ターゲットの一覧は以下の通り。

| ターゲット名    | ビルド対象                | デバッグ可 |
| --------------- | ------------------------- | ---------- |
| (なし)          | ブロック崩し本体(`block`) | ❌          |
| `debug`         | 〃                        | ⭕          |
| `clean`         | (`block` 削除)            | -          |
| `dedit`         | データエディタ            | ❌          |
| `deditdebug`    | 〃                        | ⭕          |
| `dcleaner`      | データクリーナー          | ❌          |
| `dcleanerdebug` | 〃                        | ⭕          |
| `cgi`           | オンラインランキングCGI   | ❌          |
| `cgidebug`      | 〃                        | ⭕          |

## 把握しているバグ

- **ラケットを上下に動かすとラケットの表示がおかしくなる。**
  - WSL Ubunut 18.04 + Windows Console / Hyperで再現
  - リモートCentOS6 + OpenSSH for Win32 + Hyperでも再現
  - リモートCentOS6 + PuTTYごった煮版では再現ならず
- ごくまれに左の壁の一部が消えることがある。(Levelの左側)
- リモート環境(Puttyなど)ではF3キーが効かないことがある。

## 公開にあたっての修正点・`readme.txt` について

`readme.txt` は、課題提出用に用意した説明書である。以下の理由で、初期コミット時点で4. インターネットランキングの項を修正した。

課題提出時、インターネットランキングはあるスペースを利用してホスティングしていた。しかし、以下の事項により、Dockerを用いて各自ローカルでホスティングする形式に変更した。

- スペースの公開をしたくない
- 一般公開に耐えないランキングの仕様
  - ランキングの消去が誰にでも可能
  - サーバサイドでスコアの検証をしない
  - XSS耐性なし
  - その他未知のバグを突かれることに対する懸念
