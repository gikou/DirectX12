#pragma once
#include<DirectXMath.h>
#include<math.h>
inline float Length(const DirectX::XMFLOAT3& vec) {
	return sqrtf(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
}

inline DirectX::XMFLOAT3 Normalize(const DirectX::XMFLOAT3& vec) {
	float value = Length(vec);
	return DirectX::XMFLOAT3(vec.x / value, vec.y / value, vec.z / value);
}

inline float Dot(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b) {
	return a.x*b.x + a.y*b.y + a.x*b.y;
}

inline DirectX::XMFLOAT3 Cross(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b) {
	return { a.y*b.z - a.z*b.y, a.z*b.x - a.z*b.z, a.x*b.y - a.y*b.x };
}


inline DirectX::XMMATRIX LookAtMatrix(DirectX::XMFLOAT3& lookat, DirectX::XMFLOAT3& right) {
	using namespace DirectX;
	DirectX::XMVECTOR vz = XMVector3Normalize(XMLoadFloat3(&lookat));
	DirectX::XMVECTOR vx = XMVector3Normalize(XMLoadFloat3(&right));
	DirectX::XMVECTOR vy = XMVector3Normalize(XMVector3Cross(vx, vz));
	vx = XMVector3Normalize(XMVector3Cross(vz, vy));

	DirectX::XMMATRIX ret = DirectX::XMMatrixIdentity();
	DirectX::XMFLOAT3 fvx, fvy, fvz;
	XMStoreFloat3(&fvx, vx);
	XMStoreFloat3(&fvy, vy);
	XMStoreFloat3(&fvz, vz);

	ret.r[0].m128_f32[0] = fvx.x; ret.r[0].m128_f32[1] = fvx.y; ret.r[0].m128_f32[2] = fvx.z;
	ret.r[1].m128_f32[0] = fvy.x; ret.r[1].m128_f32[1] = fvy.y; ret.r[1].m128_f32[2] = fvy.z;
	ret.r[2].m128_f32[0] = fvz.x; ret.r[2].m128_f32[1] = fvz.y; ret.r[2].m128_f32[2] = fvz.z;
	return ret;
}

//DirectXMathOperator
inline bool operator== (const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b) {

	return a.x == b.x && a.y == b.y && a.z == b.z;
};

inline DirectX::XMFLOAT3 operator - (const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b) {
	return DirectX::XMFLOAT3(a.x - b.x, a.y - b.y, a.z - b.z);
};

inline DirectX::XMFLOAT3 operator + (const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b) {
	return DirectX::XMFLOAT3(a.x + b.x, a.y + b.y, a.z + b.z);
};

inline DirectX::XMFLOAT3 operator + (const DirectX::XMFLOAT3& a) {
	return DirectX::XMFLOAT3(a.x, a.y, a.z);
};

inline DirectX::XMFLOAT3 operator - (const DirectX::XMFLOAT3& a) {
	return DirectX::XMFLOAT3(-a.x, -a.y, -a.z);
};

inline DirectX::XMFLOAT3 operator * (const DirectX::XMFLOAT3& a, const DirectX::XMMATRIX& mat) {
	return DirectX::XMFLOAT3(
		(((a.x * mat.r[0].m128_f32[0]) + (a.y * mat.r[1].m128_f32[0])) + (a.z * mat.r[2].m128_f32[0])) + mat.r[3].m128_f32[0],
		(((a.x * mat.r[0].m128_f32[1]) + (a.y * mat.r[1].m128_f32[1])) + (a.z * mat.r[2].m128_f32[1])) + mat.r[3].m128_f32[1],
		(((a.x * mat.r[0].m128_f32[2]) + (a.y * mat.r[1].m128_f32[2])) + (a.z * mat.r[2].m128_f32[2])) + mat.r[3].m128_f32[2]);
};