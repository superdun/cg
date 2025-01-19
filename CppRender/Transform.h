#pragma once
#include <array>
class Transform
{
private:
	const double scale;
	const std::array<double, 3> rotate;
	const std::array<double, 3> translation;
public:
	Transform(const double scale, const std::array<double, 3>& rotate, const std::array<double, 3>& translation);
	~Transform();
	const double& GetScale() const;
	const std::array<double, 3>& GetRotate() const;
	const std::array<double, 3>& GetTranslation() const;

};

