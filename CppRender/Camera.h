#pragma once
#include <array>
class Camera
{
private:
	std::array<double, 3> position;
	std::array<double, 3> direction;
	double angelY = 0;
public:
	Camera( std::array<double, 3> position,  std::array<double, 3> direction);
	~Camera();
	std::array<double, 3> GetPosition() const;
	std::array<double, 3> GetDirection() const;
	double GetAngleY() const;
	void Move();
	void Forward() ;
	void TurnAround();
};