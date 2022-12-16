// ピクセルシェーダーのメイン関数　→　ポリゴンを描画するのに必要なピクセル数だけ呼び出される
// 各ピクセルの色を指定するのが役目
// 戻り値：　このピクセルを何色にしたいか、RGBAで指定する
// 引数inputPos：　ピクセルの座標。ウインドウの左上を原点とするピクセル座標。

// テクスチャを受け取るグローバル変数
Texture2D gTexture : register(t0);

// サンプラーのグローバル変数
SamplerState gSampler : register(s0);


float4 ps_main(float4 inputPos : SV_POSITION,
	float2 uv : TEXCOORD, float4 col : COLOR) : SV_Target
{
	// テクスチャのテクセル色を出力ピクセル色にする
	float4 pixelColor = gTexture.Sample(gSampler, uv);

	// 頂点の色を混ぜる
	pixelColor *= col;

	return pixelColor;
}
