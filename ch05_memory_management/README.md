# memory management

## 仮想記憶
### ナイーブなメモリ確保の問題点
C言語では malloc などでメモリを触ることができると言われているが，
実は， Linux 上で動いているプロセスが物理メモリを直接触ることはできない．
事実， ```segv``` を実行すると Segmentation Fault になり，プログラムが終了する．

もしもプロセスが物理メモリを直接操作することができる場合，
以下のような問題点が発生する．

* メモリの断片化，フラグメンテーション  
    メモリの獲得，解放を繰り返すことによって空いてるメモリ領域が散り散りになってしまうこと．
* 別プロセスのメモリ領域にアクセスできること  
    メモリの番地を直接指定するだけで任意のメモリ領域にアクセスできる場合，
    カーネルや他のプロセスが使用している領域に簡単にアクセスできてしまう．
    これにより，別プロセスのメモリを書き換えてプロセスを壊したり，
    カーネルの重要な領域を書き換えることができてしまう．
* マルチプロセスの扱いが大変

これらの問題を解決する仕組みが仮想記憶（仮想メモリ）である．


### 仮想記憶
それぞれのプロセスに仮想的なメモリを割り当て，
各プロセスが自身の仮想メモリを占有しているかのように使用する仕組み．
物理メモリと仮想メモリをそれぞれページごとに分割し，
互いの対応関係をページテーブルに記録することで仮想記憶は実現される．

```mmap``` を実行することでメモリマップの新規割り当てを確認することができる．

上述の問題点は以下のように解決される．
* メモリの断片化，フラグメンテーション  
    断片化した物理メモリをひとまとめにして仮想メモリを作成すると，プロセスからはひとまとまりのメモリ領域として認識される．
* 別プロセスのメモリ領域にアクセスできること  
    プロセスは自身に割り当てられた仮想メモリのアドレス以外にアクセスすることができなくなる．
* マルチプロセスの扱いが大変
    プロセスごとに仮想メモリが割り当てられるのでプロセスは自身の仮想メモリを自由に扱うことが可能となる．


## 仮想記憶の応用例
### ファイルマップ
mmap を利用してストレージ上のファイルの内容をメモリに読み出し，仮想メモリ上にマップをすることができる．
これにより，メモリアクセスと同様の方法でファイル内容にアクセスすることが可能となる．
ファイルの変更した領域は所定のタイミングでストレージに書き戻される．

```filemap``` を実行することでファイルマップが行われていることを確認することができる．

### デマンドページング
稼働中のプロセスの仮想メモリに対応する物理メモリが使用されていなくても割り当てられている場合，メモリを無駄に消費するので非効率である．
プロセスに割り当てられた仮想メモリや，マシン全体の物理メモリを使い切ると，メモリが枯渇しプロセスが kill されてしまう (Out Of Memory)．
この問題を解決る仕組みがデマンドページングである．
仮想メモリの状態としてプロセスに割り当てられているが物理メモリは割り当てられていない状態を定義し，
プロセスからアクセスされていない仮想メモリの物理メモリを未割当にすることで物理メモリの効率的な使用を可能にする．

```demand-paging``` を実行している裏で ```sar -r 1``` や ```sar -B 1``` を実行するとページフォールトの発生や物理メモリの確保が行われていることがわかる．

### コピーオンライト
fork() の際，親プロセスのメモリが子プロセスにコピーされるが，
新たに物理メモリを確保するのではなくページテーブルをコピーすることで物理メモリの節約と高速化が実現される．
この時，ページテーブルにおける書き込み権限欄を親子ともに無効化することでどちらかにメモリ内容が書き換えられることを防ぐ．
どちらかがメモリ内容を書き換えようとした時にはページフォールトが発生し，
アクセスされるメモリ領域を別の場所に書き込み可能な状態でコピーする．
このような仕組みを Copy on Write (CoW) と呼ぶ．

```cow``` を実行することで fork した後，子プロセスがメモリにアクセスするまではメモリ使用量があまり増えていないことを確認できる．

### スワップ
OOM を防ぐための仕組みとしてスワップがある．
物理メモリが枯渇しそうなときに，物理メモリのしばらく使われないであろう領域をストレージ上のスワップ領域に退避させ，
別のプロセスに空いた領域を割り当てる．
スワップイン，スワップアウトはページ単位で行われることから，ページイン，ページアウトとも呼ばれる．

一般にメモリへのアクセスよりもストレージへのアクセスのほうが遥かに遅いため，スワッピングが頻繁に発生するとシステムの性能が低下する．
この状態のことをスラッシングという．

スワップ領域の確認は ```swapon --show``` で確認できる．
スワッピングの状態は ```sar -W 1``` で確認できる．

### 階層型ページテーブル
x86_64 アーキテクチャでは 1 プロセスあたり 128TB までの仮想記憶に対応しているが，
このサイズのページテーブルをフラットな状態で物理メモリに格納するためには 256GB の物理メモリが必要になる．
ページテーブルを小さくするために階層型のページテーブルが採用されている．
プロセスが使用する仮想メモリが大きくなると階層型ページテーブルのメモリ使用量はフラット型よりも多くなるが，
そのようなことは稀である．

### ヒュージページ
階層型ページテーブルにおいて仮想メモリを多く使うプロセスのページテーブルは巨大になる．
これに対応してページテーブルを小さくするためにページサイズを大きくする仕組みがヒュージページである．