# D3DVP
Direct3D 11 Video Processing Avisynth/AviUtl Filter

Direct3D 11 の Video API を使ったインタレ解除フィルタです。
お持ちのGPUのドライバがちゃんと実装されていれば、GPUでインタレ解除できます。

## [ダウンロードはこちらから](https://github.com/nekopanda/D3DVP/releases)

# Avisynth版

## 動作環境

- **Windows 8以降**
- Avisynth 2.6以降？（AvisynthPlus CUDAでしか試してないから不明）

## インストール

D3DVP.dllをプラグインフォルダ（plugins+/plugins64+)にコピーしてください。

※[Visual Studio 2015ランタイム](https://www.microsoft.com/ja-jp/download/details.aspx?id=48145)に依存しています。インストールしていない場合は、インストールしてください。

## 関数

D3DVP(clip, int "mode", int "order", int "width", int "height", int "quality", bool "autop",
		int "nr", int "edge", string "device", int "deviceIndex", int "cache", int "reset", string "border", int "adjust", int "debug")

	mode:
		インタレ解除モード
		- 0: 同じFPSで出力(half rate)
		- 1: 2倍FPSで出力(normal rate)
		デフォルト: 1

	order:
		フィールドオーダー
		- -1: Avisynthのparityを使用
		-  0: bottom field first (bff)
		-  1: top field first (tff)
		デフォルト: -1

	width:
		出力画像の幅
		0の場合はリサイズしない
		デフォルト: 0

	height:
		出力画像の高さ
		0の場合はリサイズしない
		デフォルト: 0

	quality:
		品質（ドライバによっては効果がないこともあります）
		- 0: 速度重視
		- 1: ふつう
		- 2: 品質重視
		デフォルト: 2

	autop:
		ドライバの自動画質補正を有効にするか
		- False: 無効
		- True: 有効
		デフォルト: False

	nr:
		ノイズリダクションの強度(0-100)
		-1でノイズリダクションを無効
		デフォルト: -1

	edge:
		エッジ強調の強度（0-100）
		-1でエッジ強調を無効
		デフォルト: -1

	device:
		使用するGPUのデバイス名。前方一致で比較されます。
		GPUのデバイス名はデバイスマネージャー等で確認してください。
		例) "Intel", "NVIDIA", "Radeon"
		デフォルト: ""（指定なし）

	deviceIndex:
		デバイス名にマッチするGPUが複数ある場合に、使用するGPUを指定します。
		最初にマッチしたGPUが0、2番目にマッチしたGPUが1、・・・です。
		デフォルト: 0

	cache:
		シーク時に処理してキャッシュする前方フレームの数です。
		小さくするとシークが高速になる反面、
		時間軸逆方向へフレームを進めるのが遅くなります。
		大きくするとこの逆になります。
		デフォルト: 15

	reset:
		初期化時やシーク時に捨てるフレーム数です。
		特に問題がなければデフォルト値でOK
		ドライバによってステートを持っていることがあるので、
		シーク直後は正しい結果が出てこないことがあるので
		処理結果を数フレームスキップします。
		その枚数の指定です。
		デフォルト: 4

	border:
		先頭のフレームより前と、最後のフレームより後ろの
		存在しないフレームをどう与えるかを制御
		- "copy": 先頭より前は先頭フレーム、最後より後ろは最後のフレームとします。
				  先頭と最後のフレームがインタレ解除できない可能性があります。
		- "blank": 存在しないフレームは黒１色のフレームにします。
		デフォルト: "copy"

	adjust:
		フレーム番号を指定した値だけ補正します。
		一部のドライバで出力フレームが遅れてたりするので、その補正用。
		だいたい以下のようにすればOK？（設定やバージョンによって変わるかも）
		Intel, NVIDIA(mode=1): 1（1フレーム遅れ）
		Radeon, NVIDIA(mode=0): 0（ずれなし）
		デフォルト: 0

	debug:
		デバッグ用です。
		1にすると処理をバイパスしてフレームをコピーします。

※nr,edgeはドライバによっては実装されていないこともあります。

## 制限

フォーマットは8bitYUV420のみ対応

処理は完全にドライバ依存なので、
PCのグラフィックス設定や、GPUに種類によって
画質が変わる可能性があります。

## サンプルスクリプト

```
# 普通にインタレ解除
srcfile="..."
LWLibavVideoSource(srcfile)
D3DVP()
```

```
# IntelとNVIDIAとRadeonのインタレ解除を比較
srcfile="..."
LWLibavVideoSource(srcfile)
intel = D3DVP(device="Intel",border="blank",adjust=1)
nvidia = D3DVP(device="NVIDIA",border="blank",adjust=1)
radeon = D3DVP(device="Radeon",border="blank")
Interleave(intel.subtitle("Intel"),nvidia.subtitle("NVIDIA"),radeon.subtitle("radeon"))
```

```
# インタレ解除して1280x720にリサイズ
srcfile="..."
LWLibavVideoSource(srcfile)
D3DVP(width=1280,height=720)
```

# AviUtl版

## 動作環境

- **Windows 8以降**

## インストール

D3DVP.aufをコピーしてください。「Direct3D 11インタレ解除」フィルタが追加されます。

※D3DVP.aufはD3DVP.dllをリネームしただけで中身は同じです。

※[Visual Studio 2015ランタイム](https://www.microsoft.com/ja-jp/download/details.aspx?id=48145)に依存しています。インストールしていない場合は、インストールしてください。

## パラメータ

*  品質（ドライバによっては効果がないこともあります）
   * 0: 速度重視
   * 1: ふつう
   * 2: 品質重視

* 幅、高さ
   * 「リサイズ」にチェックした場合のみ有効

* NR
   * ノイズ除去の強度。「ノイズ除去」にチェックした場合のみ有効

* EDGE
   * エッジ強調の強度。「エッジ強調」にチェックした場合のみ有効

* 調整
   * フレーム番号を指定した値だけ補正する。
		一部のドライバで出力フレームが遅れてたりするので、その補正用。

* 2倍fps化
   * bobでインタレ解除します。
   * AviUtlはフィルタがfpsを変更することはできないため、動画は2倍のFPSで読み込ませておいてください。ソースが29.97fpsの場合は、59.94fpsで読み込んでください。「60fps読み込み」や「60fps」ではありません。60fpsで読み込むとインタレ縞が残ることがあります。
   
<img src="https://i.imgur.com/GHafaMe.png" alt="59.94fpsを選択" title="2倍FPS読み込み" width=313>

* YUV420で処理
   * 通常はYUY2で処理しますが、一部YUY2での処理に対応していないドライバがあるため、その場合はこれにチェックしてYUV420で処理してください。

* 使用GPU
   * 使用するGPUの指定です。指定がない場合は一番上のGPUを使います。

## AviUtl版の制限

- 処理速度はAviSynth版より遅くなります。

- 内部である程度フレームを持っている関係で、上流フィルタの設定を変えても反映されないことがあります。パラメータをいじれば、フレームが再処理されて更新されると思います。
   - 保存（エンコード）時は、最初にリセットするので、出力はちゃんと上流フィルタの設定が反映されるはずです。

# ライセンス

D3DVPのソースコードはMITライセンスとします。
