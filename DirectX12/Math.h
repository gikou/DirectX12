#pragma once
#include<DirectXMath.h>
#include<math.h>
float Length(const DirectX::XMFLOAT3& vec) {
	return sqrtf(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
}

DirectX::XMFLOAT3 Normalize(const DirectX::XMFLOAT3& vec) {
	float value = Length(vec);
	return DirectX::XMFLOAT3(vec.x / value, vec.y / value, vec.z / value);
}

float Dot(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b) {
	return a.x*b.x + a.y*b.y + a.x*b.y;
}

DirectX::XMFLOAT3 Cross(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b) {
	return { a.y*b.z - a.z*b.y, a.z*b.x - a.z*b.z, a.x*b.y - a.y*b.x };
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