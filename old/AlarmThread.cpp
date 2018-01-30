// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "AlarmThread.h"
#include "Main.h"

#pragma package(smart_init)
// ---------------------------------------------------------------------------

// Important: Methods and properties of objects in VCL can only be
// used in a method called using Synchronize, for example:
//
// Synchronize(&UpdateCaption);
//
// where UpdateCaption could look like:
//
// void __fastcall AlarmThreadClass::UpdateCaption()
// {
// Form1->Caption = "Updated in a thread";
// }
// ---------------------------------------------------------------------------

__fastcall AlarmThreadClass::AlarmThreadClass(bool CreateSuspended)
	: TThread(CreateSuspended)
{
	cycleM=new TMutex(false);
}
// ---------------------------------------------------------------------------
void __fastcall AlarmThreadClass::Execute()
{
	// ������������ ��������� ��������
	NameThreadForDebugging("AlarmThread");
	bool cycle=true;
	is_alarm=false;
	// ���������� ���������, ���� �� ���������� ��� ������ �������
	while(cycle)
	{
		if(digital->SignalExists("�� �������"))
		{
			if(!digital->CheckInBouncelessSignal("�� �������",false))
			{
				cycle=false;
				reason="�� ������� ������ ������ ���������!";
				TPr::SendToProtocol("AlarmThread: "+reason);
			}
			TThread::CurrentThread->Sleep(40);
		}
		// -------------------------------------------------------------------------
		if(!digital->CheckInBouncelessSignal("���� ����������",false))
		{
			cycle=false;
			reason="��� ������� ���� ����������!";
			TPr::SendToProtocol("AlarmThread: "+reason);
			TThread::CurrentThread->Sleep(40);
		}
		if(LinearCycle)
			if(!digital->CheckInBouncelessSignal("���� ����",false))
			{
				cycle=false;
				reason="������ ������ ���������� ����!";
				TPr::SendToProtocol("AlarmThread: "+reason);
				TThread::CurrentThread->Sleep(40);
			}

		if(CrossCycle)
			if(!digital->CheckInBouncelessSignal("����� ����",false))
			{
				cycle=false;
				reason="������ ������ ���������� ����!";
				TPr::SendToProtocol("AlarmThread: "+reason);
				TThread::CurrentThread->Sleep(40);
			}
		if(InvA)
		{
			if(!digital->CheckInSignal("���� �� �"))
			{
				cycle=false;
				reason="������ ������ ���� �� �!";
				TPr::SendToProtocol("AlarmThread: "+reason);
			}
		}

		if(Terminated)
			break;
	}
	// ��������, ��� ����� - �� ������ ��� ������, ������������ �� �������� �����
	if(!cycle)
	{
		// Synchronize(&UpdateCaption);
		is_alarm=true;
	}

}
// ---------------------------------------------------------------------------

void __fastcall AlarmThreadClass::UpdateCaption()
{
	MainForm->Caption="��� ��������";
}
// -------������ ���� �����������---------------------------------------------
void AlarmThreadClass::SetCrossCycle()
{
	cycleM->Acquire();
	CrossCycle=true;
	cycleM->Release();
}
void AlarmThreadClass::ResetCrossCycle()
{
	cycleM->Acquire();
	CrossCycle=false;
	cycleM->Release();
}
// -------������ ���� �����������---------------------------------------------
void AlarmThreadClass::SetLinearCycle()
{
	cycleM->Acquire();
	LinearCycle=true;
	cycleM->Release();
}
void AlarmThreadClass::ResetLinearCycle()
{
	cycleM->Acquire();
	LinearCycle=false;
	cycleM->Release();
}
// ---------------------------------------------------------------------------
void AlarmThreadClass::SetA()
{
	cycleM->Acquire();
	InvA=true;
	cycleM->Release();
}
// ---------------------------------------------------------------------------
void AlarmThreadClass::ResetA()
{
	cycleM->Acquire();
	InvA=false;
	cycleM->Release();
}
// ---------------------------------------------------------------------------
