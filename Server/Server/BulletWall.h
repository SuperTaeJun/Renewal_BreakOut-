#pragma once
#include "pch.h"
class BulletWall
{
public:
	float x, y, z;
	float pitch, yaw, roll;
	int ob_id;
	int currentGameRoom;
public:
	BulletWall() {}
	~BulletWall() {}

	void setPosition(float x_val, float y_val, float z_val) {
		x = x_val;
		y = y_val;
		z = z_val;
	}

	void removeOBJ() {
		//�����Ǿ��� �� �迭�� �������� �������� ������ ������ ������ ���鿡�� ����ȭ
	}
};
