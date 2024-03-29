# file system

ストレージデバイスは大きな配列のようなものであるため，
ストレージデバイスにデータの読み書きをするためには
アドレスなどを管理する必要がある．
ストレージデバイスにアクセスるための煩雑な処理を
管理する仕組みがファイルシステムである．
Linux で使用可能なファイルシステムには様々な種類があるが，
どのファイルシステムでも統一されたシステムコールを介して
ファイルを扱うことができる．

ファイルシステム内ではファイルをカテゴリ別に管理するために
ディレクトリというファイルが存在する．
ディレクトリは配下のファイルのメタ情報をかくのうしており，
ディレクトリ内にディレクトリを格納することも可能である．
これにより，ファイルシステムは木構造になる．

ファイルシステムに格納されるファイルには，ファイルの大きさや格納場所，
権限や時刻情報などのメタデータが格納されている．
ファイルのデータサイズに対してメタデータは小さなものではあるが，
小さなファイルを多く作るようなシステムでは注意する必要がある．
```stat``` コマンドを使用してファイルのメタデータを確認することができる．

ファイルしシステムには様々な種類が存在するが，
ファイルシステムの違いを吸収し，統一されたインターフェースを提供する
Virtual File System (VFS) という仕組みが存在する．

## ファイルシステムの不整合を防ぐ仕組み
ファイル処理の途中でシステムがダウンした際などに，
ファイルシステムの内容に不整合が生じることがある．
例えば，ファイルを移動する際には移動対象のファイルとその親ディレクトリ
の2つのファイルに書き込みが発生する．
個々の書き込み処理は不可分であるため，２つの処理を順番に行う必要がある．
ファイル移動の途中でシステムがダウンするとファイルシステムに不整合が生じる．

### ジャーナリング
ファイルシステム内にユーザには認知できない特殊な領域をジャーナル領域として確保する．
ファイルシステム更新時には更新内容の個々の処理をジャーナル領域に書き込む．
この内容をジャーナルログと呼ぶ．
ジャーナルログを保存してからファイルシステムの更新を行う．
ジャーナルログ作成の段階でシステムに不具合が生じた場合はファイルシステムの更新が行われず，
更新の段階で不具合が生じた場合は帆残されたジャーナルログに従って再度更新が行われる．
これにより，ファイルシステムの整合性が保たれる．

### コピーオンライト
ファイルシステム更新時に更新内容を全て別のファイルに書き込んでから，更新前のファイルを削除する．
途中でシステムがダウンしても更新しかけのファイルを削除すればよいだけで不整合は生じない．

### ファイルシステムの復旧
上記のような対策がなされていてもファイルシステムに不整合が生じてしまうことがある．
このような時のためにバックアップをこまめに取っておくべきであるが，
ファイルシステムに用意されている復旧用コマンドを使用することができる．
```fsck``` などが代表例であるが，このコマンドを使用したとしても完全に復旧できないこともあるため，
結局のところはこまめにバックアップを取っておくべきである．

## ファイルの種類
通常ファイルとディレクトリ以外に，デバイスファイルというファイルが存在する．
デバイスファイルにも同様にシステムコールでアクセスすることができる．
デバイスファイルにはキャラクタデバイスとブロックデバイスの２種類がある．
ファイルの種類は ```ls -l``` で確認することができる．

デバイスの名前はメジャー番号とマイナー番号で示されており，
メジャー番号でデバイスドライバが決まり，マイナー番号でデバイスドライバ管理下のデバイスの個体が決まる．

### キャラクタデバイス
読み書きはできるが，シーク（ランダムアクセス，特定の位置に対する読み書き）できないファイル．
端末やキーボード，マウスがキャラクタデバイスの代表例である．
例えば別でターミナルを開いて対応する ```/dev/pts/[#terminal]``` に入出力を行うと，
内容がそのターミナルに流れる．

### ブロックデバイス
ブロックデバイスは読み書き以外にランダムアクセスが可能である．
HDD や SSD が代表的なブロックデバイスである．
ブロックデバイスにファイルシステムから直接アクセスすることもできるが，危険なのでやめたほうがいい．

## 様々なファイルシステム
* tmpfs: メモリ上に作成されるファイルシステム．アクセスが高速
* nfs: ネットワークを経由してファイルシステムの内容を見せるもの

以下は仮想ファイルシステム
* procfs: プロセスに関する情報を格納するためのファイルシステム
* sysfs: procfs が乱用されることを防ぐために作られた
* cgroupfs: プロセスグループに対してリソース制限をかけるための ```cgroup``` という機能のために使用される

Btrfs  
ext4 や XFS よりも豊富な機能を持ったファイルシステム．
LVM のようなボリュームマネジメント機能，作成したサブボリュームを使用してのスナップショット取得，
ファイルシステム上での RAID の構築などが可能である．
加えて，ファイルデータの破損検知，修復（ RAID 使用時）も可能である．