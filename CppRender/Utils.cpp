#include "Utils.h"


std::string Utils::ArrayToString(const std::array<double, 3>& arr) {
    std::ostringstream oss;
    oss << std::fixed; // 保持固定小数位
    oss.precision(6);  // 设置小数精度
    oss << arr[0] << "," << arr[1] << "," << arr[2]; // 将数组元素连接成字符串
    return oss.str();
}   