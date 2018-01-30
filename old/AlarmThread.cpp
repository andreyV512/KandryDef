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
	// контролируем аварийные ситуации
	NameThreadForDebugging("AlarmThread");
	bool cycle=true;
	is_alarm=false;
	// бесконечно проверяем, есть ли необходимы для работы сигналы
	while(cycle)
	{
		if(digital->SignalExists("ГП включен"))
		{
			if(!digital->CheckInBouncelessSignal("гп включен",false))
			{
				cycle=false;
				reason="Не включен модуль группы прочности!";
				TPr::SendToProtocol("AlarmThread: "+reason);
			}
			TThread::CurrentThread->Sleep(40);
		}
		// -------------------------------------------------------------------------
		if(!digital->CheckInBouncelessSignal("цепи управления",false))
		{
			cycle=false;
			reason="Нет сигнала цепи управления!";
			TPr::SendToProtocol("AlarmThread: "+reason);
			TThread::CurrentThread->Sleep(40);
		}
		if(LinearCycle)
			if(!digital->CheckInBouncelessSignal("Прод цикл",false))
			{
				cycle=false;
				reason="Пропал сигнал Продольный Цикл!";
				TPr::SendToProtocol("AlarmThread: "+reason);
				TThread::CurrentThread->Sleep(40);
			}

		if(CrossCycle)
			if(!digital->CheckInBouncelessSignal("Попер цикл",false))
			{
				cycle=false;
				reason="Пропал сигнал Поперечный цикл!";
				TPr::SendToProtocol("AlarmThread: "+reason);
				TThread::CurrentThread->Sleep(40);
			}
		if(InvA)
		{
			if(!digital->CheckInSignal("Прод ПЧ А"))
			{
				cycle=false;
				reason="Пропал сигнал Прод ПЧ А!";
				TPr::SendToProtocol("AlarmThread: "+reason);
			}
		}

		if(Terminated)
			break;
	}
	// проверям, как вышли - по аварии или штатно, терминатнули из главного треда
	if(!cycle)
	{
		// Synchronize(&UpdateCaption);
		is_alarm=true;
	}

}
// ---------------------------------------------------------------------------

void __fastcall AlarmThreadClass::UpdateCaption()
{
	MainForm->Caption="Все ебнулось";
}
// -------сигнал Цикл поперечного---------------------------------------------
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
// -------сигнал Цикл продольного---------------------------------------------
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
