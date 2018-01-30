//---------------------------------------------------------------------------

#ifndef AlarmThreadH
#define AlarmThreadH
//---------------------------------------------------------------------------
#include <Classes.hpp>
//---------------------------------------------------------------------------
#include "AdvantechSignals.h"


//! ����� ����������� �������� ��������� �������� �� ����� ������
class AlarmThreadClass : public TThread
{
private:
	bool CrossCycle;
	bool LinearCycle;
	bool InvA;

protected:
	//! ������ ����� �� �������� ������ �� ����� ������
	void __fastcall Execute();
	//! ������� �������� ������� ��������
	TMutex *cycleM;
	//! �������� ������� ��������
	bool control;

public:
	//! ����������� ������
	__fastcall AlarmThreadClass(bool CreateSuspended);
	//! ��������� ������� �����
	void __fastcall UpdateCaption();
	//! ���� �� ������
	bool is_alarm;
	//! ������� ������
	String reason;
	//! �������� ��������� ������ ����� ����
	void SetCrossCycle();
	//! ��������� ��������� ������ ���� ����
	void ResetCrossCycle();
	//! �������� ��������� ������ ����� ����
	void SetLinearCycle();
	//! ��������� ��������� ������ ���� ����
	void ResetLinearCycle();
	void SetA();
	void ResetA();
	//! ����� ������� ����� ���������
	void SetStand(String st1, String st2 = "");
	//! ������ �� ��������� ������� �����
	void ResetStand();

};
//---------------------------------------------------------------------------
#endif
