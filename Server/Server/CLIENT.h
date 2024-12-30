#pragma once
#include "pch.h"

class CLIENT
{
public:
	int _s_id; //�÷��̾� ���� id
	char name[MAX_NAME_SIZE]; //�÷��̾� nick
	char _pw[MAX_NAME_SIZE];  // pw
	//�α��� ����
	bool bLogin = false;
	// ��ġ
	float	x;
	float	y;
	float	z;
	// ȸ����
	// ������ ȹ�� ī��Ʈ
	int myItemCount;
	bool connected = false;
	bool selectweapon = false;
	float	Yaw;
	float	Pitch;
	float	Roll;
	float   Speed;
	// ���� ������
	float AO_YAW, AO_PITCH;
	// �ӵ�
	float VX;
	float VY;
	float VZ;
	float Max_Speed;

	float _hp; // ü��
	int damage;
	bool is_bone = false;
	bool bGetWeapon = false;
	bool bCancel;
	bool bEndGame = false;
	// ����
	bool bRecharge = false;
	// bAlive
	bool bAlive = true;
	// ����Ʈ ��/����
	bool bLightOn = false;
	//	Reload
	bool bReload = false;
	// hitAnim
	bool bHitAnim = false;
	WeaponType w_type;
	PlayerType p_type;
	float s_x, s_y, s_z;
	float e_x, e_y, e_z;
	int wtype;
	//�������� �ִϸ��̼� ����
	int bojoanimtype;
	//--------------------
	//�״� �ִϸ��̼� Ÿ��
	int deadtype;
	// ������ Ÿ��
	int dissolve;
	// ������ ���ӷ� ��ȣ
	int currentRoom = -1;
	int jumpType = -1;
	unordered_set   <int>  viewlist; // �þ� �� ������Ʈ

	mutex state_lock;
	CL_STATE _state;
	atomic_bool  _is_active = false;
	int num;
	int itemAnimNum;
	//-------------
	float pitch0, yaw0, roll0;
	float pitch1, yaw1, roll1;
	float pitch2, yaw2, roll2;
	float pitch3, yaw3, roll3;
	float pitch4, yaw4, roll4;
	//-------------
	Overlap		_recv_over;
	SOCKET		_socket;
	int      _prev_size;
	int      last_move_time;

public:
	CLIENT() : _state(ST_FREE), _prev_size(0)
	{
		_hp = 100.f;
		myItemCount = 0;
	}

	~CLIENT()
	{
		closesocket(_socket);
	}
	void do_recv();
	void do_send(int num_bytes, void* mess);
};


