# ue5-cpp-bp-foundation-Day14
UE5初心者によるC++とBlueprintを組み合わせた14日間の開発記録用リポジトリです。
本プロジェクトでは、C++で基盤となるロジック（計算・管理）を実装し、Blueprintで外見やエフェクトを制御する設計を行っています。
本リポジトリはコードレビュー向けに構成されており、.uasset等のバイナリファイルを除いたソースコード（.cpp, .h）のみを公開しています。

## プロジェクト構成（Sourceファイル解説）

### 📂 Actors
- **MyPlayerCharacter**: プレイヤーの移動、入力制御、攻撃トリガーの実装。
- **MyEnemy / MyBullet / MyExpOrb**: ゲーム内の各動的エンティティのベースクラス。

### 📂 Components
- **BulletManagerComponent**: **オブジェクトプーリング**による弾丸の生成・再利用管理。
- **MyStatusComponent**: HP、攻撃力、経験値などのパラメータ管理とイベント通知。

### 📂 Managers
- **MyEnemySpawner**: **オブジェクトプーリング**による敵の管理、敵の出現タイミングと位置の制御。
- **MyOrbManager**: **オブジェクトプーリング**による経験値オーブの管理、経験値オーブのライフサイクル管理。
- **MyPopUpManager**: ダメージ通知などのUIエフェクトの生成管理。
- **MyGameModeBase**: ゲームのメインルール、ステート（進行状況）の管理。

### 📂 UI
- **MyUpgradeMenuWidget**: レベルアップ時のアップグレード選択ロジック。
- **MyGameHUD**: プレイ中のHPバーやUI全体を表示するコントローラー。

### 📂 Data & Maps
- **MyGameTypes.h**: プロジェクト全体で使用する構造体や列挙型の定義。
- **MyTerrainPanel**: 無限生成される地形のタイル管理ロジック。

---
## クレジット
**本プロジェクトのグラフィックには、Kenney氏（www.kenney.nl）制作の「Tiny Dungeon」を使用させていただきました。**
Assets: [Tiny Dungeon by Kenney](https://www.kenney.nl)
