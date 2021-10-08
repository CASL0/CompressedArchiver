# CompressedArchiver
アーカイブパッケージマネージャーです。

## 機能
* アーカイブ機能
  * 対応アルゴリズム
    * ZIP
    * XPRESS
    * XPRESS + ハフマン
    * LZMS
    * LZ4
  * xxHashによるチェックサム
* アーカイブ内のリスト表示

## 開発環境
Visual Studio 2019 Community
* C++によるデスクトップ開発
  * MFC

依存ライブラリ
* xxHash
* LZ4
```
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
bootstrap-vcpkg.bat
vcpkg integrate install
vcpkg install xxhash:x64-windows-static
vcpkg install lz4:x64-windows-static
```
