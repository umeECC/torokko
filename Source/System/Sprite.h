#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include "RenderContext.h"

// スプライト
class Sprite
{
public:
	Sprite();
	Sprite(const char* filename);

	// 頂点データ
	struct Vertex
	{
		DirectX::XMFLOAT3	position;
		DirectX::XMFLOAT4	color;
		DirectX::XMFLOAT2	texcoord;
	};

	// 描画実行
	void Render(const RenderContext& rc,
		float dx, float dy,					// 左上位置
		float dz,							// 奥行
		float dw, float dh,					// 幅、高さ
		float sx, float sy,					// 画像切り抜き位置
		float sw, float sh,					// 画像切り抜きサイズ
		float angle,						// 角度
		float r, float g, float b, float a	// 色
	) const;

	// 描画実行（テクスチャ切り抜き指定なし）
	void Render(const RenderContext& rc,
		float dx, float dy,					// 左上位置
		float dz,							// 奥行
		float dw, float dh,					// 幅、高さ
		float angle,						// 角度
		float r, float g, float b, float a	// 色
	) const;
	//シェーダーのframeworkクラス
	struct scroll_constants
	{
		DirectX::XMFLOAT2 scroll_direction;
		DirectX::XMFLOAT2 scroll_dummy;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> scroll_constant_buffer;
	DirectX::XMFLOAT2 scroll_direction;
	D3D11_TEXTURE2D_DESC mask_desc;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mask_texture;

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>			vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>			inputLayout;

	Microsoft::WRL::ComPtr<ID3D11Buffer>				vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shaderResourceView;

	float textureWidth = 0;
	float textureHeight = 0;
};
