// �s�N�Z���V�F�[�_�[�̃��C���֐��@���@�|���S����`�悷��̂ɕK�v�ȃs�N�Z���������Ăяo�����
// �e�s�N�Z���̐F���w�肷��̂����
// �߂�l�F�@���̃s�N�Z�������F�ɂ��������ARGBA�Ŏw�肷��
// ����inputPos�F�@�s�N�Z���̍��W�B�E�C���h�E�̍�������_�Ƃ���s�N�Z�����W�B

// �e�N�X�`�����󂯎��O���[�o���ϐ�
Texture2D gTexture : register(t0);

// �T���v���[�̃O���[�o���ϐ�
SamplerState gSampler : register(s0);


float4 ps_main(float4 inputPos : SV_POSITION,
	float2 uv : TEXCOORD, float4 col : COLOR) : SV_Target
{
	// �e�N�X�`���̃e�N�Z���F���o�̓s�N�Z���F�ɂ���
	float4 pixelColor = gTexture.Sample(gSampler, uv);

	// ���_�̐F��������
	pixelColor *= col;

	return pixelColor;
}
