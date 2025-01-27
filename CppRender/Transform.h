#pragma once
#include <array>
class Transform
{
private:
	 double scale ;
	 std::array<double, 3> rotate;
	 std::array<double, 3> translation;
	 double angle ;
public:
	Transform();
	Transform(const double scale, const double angle, const std::array<double, 3>& rotate, const std::array<double, 3>& translation);
	~Transform();
	Transform(const Transform& other);
	const double& GetScale() const;
	const std::array<double, 3>& GetRotate() const;
	const std::array<double, 3>& GetTranslation() const;
	const double& GetAngle() const;
	const void SetRotateState(const std::array<double, 3>& rotate,const double angle  );


};

