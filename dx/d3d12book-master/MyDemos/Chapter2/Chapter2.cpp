// Chapter2.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <iostream>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <vector>
using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace std;

vector<vector<float>> Transpose(const vector<vector<float>> m);
float Det(const vector<vector<float>>& m);
vector<vector<float>> Inverse(const vector<vector<float>>& m);
float Cofactor(const vector<vector<float>>& m, int row, int col);
vector<vector<float>> Adjugate(const vector<vector<float>>& m);

ostream& XM_CALLCONV operator << (ostream& os, FXMVECTOR v) {
    XMFLOAT4 dest;
    XMStoreFloat4(&dest, v);
    os << "(" << dest.x << " , " << dest.y << ", " << dest.z << ", " << dest.w << ")";
    return os;
}

ostream& XM_CALLCONV operator <<(ostream& os, FXMMATRIX m) {
    for (size_t i = 0; i < 4; i++)
    {
        os << XMVectorGetX(m.r[i]) << "\t";
        os << XMVectorGetY(m.r[i]) << "\t";
        os << XMVectorGetZ(m.r[i]) << "\t";
        os << XMVectorGetW(m.r[i]);
        os << endl;
    }
    return os;
}

int main()
{
    if (!XMVerifyCPUSupport) {
        return 0;
    }
    XMMATRIX A(1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 4.0f, 0.0f,
        1.0f, 2.0f, 3.0f, 1.0f);

    XMMATRIX B = XMMatrixIdentity();
    XMMATRIX C = A * B;
    XMMATRIX D = XMMatrixTranspose(A);
    XMVECTOR det = XMMatrixDeterminant(A);
    XMMATRIX E = XMMatrixInverse(&det, A);
    XMMATRIX F = A * E;
    cout << "F = A*E" << endl << F << endl;

    Transpose({ {1,2,3} , {4,5,6} });
    cout << Det({ {21,-4} , {10,7} });
    Inverse({ {21,-4} , {10,7} });
}

vector<vector<float>> Transpose(const vector<vector<float>> m) {
    auto rowC = m.size();
    auto colC = m[0].size();

    vector<vector<float>> result(colC,vector<float>(rowC));
    for (size_t c = 0; c < colC; c++)
    {
            for (size_t r = 0; r < rowC; r++)
            {
                result[c][r] = m[r][c];
            }
    }
    return result;
}

float Det(const std::vector<std::vector<float>>& m) {
    auto size = m.size();
    if (size == 1) {
        return m[0][0];
    }
    if (size == 2) {
        return m[0][0] * m[1][1] - m[0][1] * m[1][0];
    }

    float result = 0.0;

    for (size_t c = 0; c < size; c++) {
        std::vector<std::vector<float>> subMatrix(size - 1, std::vector<float>(size - 1));

        for (size_t subRow = 1; subRow < size; subRow++) {
            for (size_t subCol = 0; subCol < size; subCol++) {
                if (subCol < c) {
                    subMatrix[subRow - 1][subCol] = m[subRow][subCol];
                }
                else if (subCol > c) {
                    subMatrix[subRow - 1][subCol - 1] = m[subRow][subCol];
                }
            }
        }


        result += m[0][c] * Det(subMatrix) * (c % 2 == 0 ? 1 : -1);
    }

    return result;
}
// 计算余子式
float Cofactor(const vector<vector<float>>& m, int row, int col) {
    int n = m.size();
    vector<vector<float>> subMatrix(n - 1, vector<float>(n - 1));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i != row && j != col) {
                subMatrix[i < row ? i : i - 1][j < col ? j : j - 1] = m[i][j];
            }
        }
    }
    return Det(subMatrix) * ((row + col) % 2 == 0 ? 1 : -1); 
}

// 计算伴随矩阵
vector<vector<float>> Adjugate(const vector<vector<float>>& m) {
    int n = m.size();
    vector<vector<float>> adjugate(n, vector<float>(n));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            adjugate[j][i] = Cofactor(m, i, j); // 转置
        }
    }
    return adjugate;
}

// 计算逆矩阵
vector<vector<float>> Inverse(const vector<vector<float>>& m) {
    float determinant = Det(m);
    if (determinant == 0) {
        throw runtime_error("矩阵不可逆"); // 如果行列式为0则不可逆
    }

    vector<vector<float>> adjugate = Adjugate(m);
    int n = m.size();
    vector<vector<float>> inverse(n, vector<float>(n));

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            inverse[i][j] = adjugate[i][j] / determinant; 
        }
    }

    return inverse;
}


// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
