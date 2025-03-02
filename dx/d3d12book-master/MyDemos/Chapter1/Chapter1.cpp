// Chapter1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace std;
int Test167();

int main()
{
	Test167();
	return 0;
}

ostream& XM_CALLCONV operator<<(ostream& os,FXMVECTOR v)
{
	XMFLOAT3 dest;
	XMStoreFloat3(&dest, v);
	os << "(" << dest.x << " , " << dest.y << ", " << dest.z << ")";
	return os;
}

int Test167()
{
	cout.setf(ios_base::boolalpha);

	if (!XMVerifyCPUSupport)
	{
		cout << "directx math not supported" << endl;
		return 0;

	}

	XMVECTOR p = XMVectorZero();
	XMVECTOR q = XMVectorSplatOne();
	XMVECTOR u = XMVectorSet(1.0f, 2.0f, 3.0f, 0.0f);
	XMVECTOR v = XMVectorReplicate(-2.0f);
	XMVECTOR w = XMVectorSplatZ(u);
	auto a = v * w;
	cout << "p=" << p << endl;
	cout << "q=" << q << endl;
	cout << "u=" << u << endl;
	cout << "v=" << v << endl;
	cout << "w=" << w << endl;
	cout << "w=" << v*w << endl;
	return 0;	
}
