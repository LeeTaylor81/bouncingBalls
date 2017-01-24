#pragma once

#include <vector>

class Ball
{
private:
	double mX, mY, mRadius, mXVelocity, mYVelocity;
	std::vector<double> mColor;

public:
	Ball();
	Ball(double x, double y, double radius, double xVelocity, double yVelocity, double r, double g, double b);
	double getRadius();
	double getX();
	void setX(double newX);
	double getNextX();
	double getY();
	void setY(double newY);
	double getNextY();
	double getXVelocity();
	void setXVelocity(double newXVelocity);
	double getYVelocity();
	void setYVelocity(double newXVelocity);
	std::vector<double> getColor();
};

