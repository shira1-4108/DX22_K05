
// 頂点シェーダーのメイン関数
// 役割：各頂点の対して行いたい移動・回転・縮小などを実行する
// 戻り値：このシェーダーの処理の結果を、頂点のデータとして戻す
// 引数 inputPos：VRAMに転送された頂点データ（のうち１つ）が渡される
//      C言語で設定した頂点座標と同じ座標系


// C++から定数バッファで転送されてきたデータ
cbuffer ConstantBufferMatrix : register(b1)
{
	matrix gWorld;
	matrix gView;
	matrix gProjection;
}

// return用の構造体
struct VS_OUTPUT
{
	float4 pos : SV_Position;
	float2 uv  : TEXCOORD;
	float4 col : COLOR;
};

// 頂点シェーダー　メイン関数（entrypoint）
// 戻り値：座標変換済みの頂点データ
VS_OUTPUT vs_main(float4 inputPos : POSITION, // xy
	float2 uv : TEX, float4 col : COL) // uv
{
	VS_OUTPUT output;

	output.pos = inputPos;  // 渡されたxyとuvを変数に移す
	output.uv = uv;
	output.col = col;

	// 画面比率で横長になってしまうのを調整する
	//output.pos.x *= 480.0f / 640.0f;

	output.pos = mul(output.pos, gWorld); // ワールド変換行列を頂点座標にかける
	output.pos = mul(output.pos, gView);
	output.pos = mul(output.pos, gProjection);

	return output;
}