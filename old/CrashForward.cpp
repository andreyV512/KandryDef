//---------------------------------------------------------------------------


#pragma hdrstop

#include "CrashForward.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

//---------------------------------------------------------------------------
__fastcall CrashForwardThread::CrashForwardThread(bool CreateSuspended , TStatusBar *St , AlarmThreadClass &_alth)
				: TThread(CreateSuspended)
{
	StatusBar = St;
}
//---------------------------------------------------------------------------
void __fastcall CrashForwardThread::UpdateMainForm()
{
	StatusBar->Panels->Items[1]->Text = stext1;
	StatusBar->Panels->Items[2]->Text = stext2;
	StatusBar->Refresh();
}
//---------------------------------------------------------------------------
void __fastcall CrashForwardThread::Execute()
{
	NameThreadForDebugging("CrashForwardThread");
// ���� ������ ����� - �� �������
	if ( !digital->CheckInSignal("���� ����") &&
			!digital->CheckInSignal("���� �����") &&
			!digital->CheckInSignal("���� ����") &&
			!digital->CheckInSignal("���� �����") &&
			!digital->CheckInSignal("����� ����") &&
			!digital->CheckInSignal("����� �����") &&
			!digital->CheckInSignal("������ 1") &&
			!digital->CheckInSignal("������ 3") &&
			!digital->CheckInSignal("������ 4") )
	{
		stext1 = "����� \"�������\"";
		stext2 = "�������� ������";
		TPr::SendToProtocol("-----------------");
		TPr::SendToProtocol("����� �������");
		Synchronize( &UpdateMainForm );
		cool = Transit();
	}
// ���� ���-�� ����� �������� - �� �����
	else
	{
		stext1 = "����� \"�����\"";
		stext2 = "�������� ������";
		TPr::SendToProtocol("-----------------");
		TPr::SendToProtocol("����� �����");
		Synchronize( &UpdateMainForm );
		cool = CrashForwardMode();
	}

	Terminate();
	return;
}
//---------------------------------------------------------------------------

bool CrashForwardThread::CrashForwardMode()
{
	bool result=true;				// ��������� ������ (�������/�����)
	bool lin = false;				// ��������� �� ���������� � ������
	bool thick = false;				// ��������� �� ���������� � ������
	bool cross = false;				// ��������� �� ���������� � ������
	String ModuleResult;			// ��������� ����������� ������
	isTransit = false;
	while (result)
	{

// �������, ��� �������� �����
		thick = ( ( digital->CheckInSignal("���� ����") || digital->CheckInSignal("���� �����") ||
					digital->CheckInSignal("������ 3")  || digital->CheckInSignal("����� �����") )
				 && digital->CheckInSignal("���� ��") );
		lin = ( ( digital->CheckInSignal("���� ����") || digital->CheckInSignal("���� �����") ||
				  digital->CheckInSignal("������ 3")  || digital->CheckInSignal("������ 4") )
				&& digital->CheckInSignal("���� ��") );
		cross = ( ( digital->CheckInSignal("����� ����") || digital->CheckInSignal("����� �����") ||
					digital->CheckInSignal("������ 1")  || digital->CheckInSignal("���� ����") )
				 && digital->CheckInSignal("����� ��") );
// ����� �������� � ����������, ���������� ������
		if ( cross && !lin )
		{
			stext2 = "������� ���������� ������";
			Synchronize( &UpdateMainForm );
			if ( ModuleResult != "ok")
			{
				stext2 = ModuleResult;
				TPr::SendToProtocol("�����: " + stext2);
				result = false;
				break;
			}
		}
// ����� �������� � ����������, ������� ������
		if ( cross && !thick )
		{
			stext2 = "������� ������ �������������";
			Synchronize( &UpdateMainForm );
			if ( ModuleResult != "ok")
			{
				stext2 = ModuleResult;
				TPr::SendToProtocol("�����: " + stext2);
				result = false;
				break;
			}
		}
// ������������� ����� ������ ����������� ����� (����� 1 � 2)
		digital->SetOutSignal("������ �������");
		digital->SetOutSignal("������ ����");
		stext2 = "���� ���������� �����";
		Synchronize( &UpdateMainForm );
		if ( !( digital->WaitForInSignal("������ ������",5000 , TThread::CurrentThread)
				&& digital->CheckInSignal("������ ������") ) )
		{
			stext2 = "������ ����������� �����!";
			TPr::SendToProtocol("�����: " + stext2);
			result = false;
			break;
		}
		digital->ResetOutSignal("����� �������");
		digital->ResetOutSignal("����� ����");

/*
// ������������� ����� ������ ����������� ������� (����� 1 � 2)
		digital->SetOutSignal("����� �������");
		stext2 = "���� ���������� �������";
		Synchronize( &UpdateMainForm );
		if ( !( digital->WaitForInSignal("����� ������",3000 , TThread::CurrentThread) ) )
//		&& digital->CheckInSignal("������ ����2") ) )
		{
			stext2 = "������ ����������� �������!";
			result = false;
			break;
		}
*/
// ���� �������� �������

		if (thick)
		{
			stext2="�������� �������� �����������";
			Synchronize( &UpdateMainForm );
			if ( !digital->WaitForInSignal("��������",20000 , TThread::CurrentThread))
			{
				stext2 = "�� ��������� �������� �����������";
				TPr::SendToProtocol("�����: " + stext2);
				result = false;
				break;
			}
		}
		// ������ ��� �����������
		digital->SetOutSignal("����");

// ������� ����������, ���� �����
		if (lin)
		{
			stext2 = "������� ���������� ������";
			Synchronize( &UpdateMainForm );
			digital->SetOutSignal("���� ��� ��");
			digital->SetOutSignal("���� STF");
			digital->SetOutSignal("���� �������� 1");
		}

		stext2= "����� ���� ������";
		Synchronize( &UpdateMainForm );
		TPr::SendToProtocol("�����: " + stext2);
		if ( !digital->WaitForDropInSignal( "������ 4" , 80000 , TThread::CurrentThread) )
		{
			stext2 = "�� ��������� ����� ����� �� ������ 4!";
			TPr::SendToProtocol("�����: " + stext2);
			result = false;
			break;
		}
	   //	digital->ResetOutSignal("���� STF");

		stext2="���� ��������� ����� � ����� ����� (��2)";
		Synchronize( &UpdateMainForm );
		if ( !digital->WaitForInSignal("��� ��2", 120000 , TThread::CurrentThread) )
		{
			stext2 = "�� ��������� ����� � ����� �����!";
			TPr::SendToProtocol("�����: " + stext2);
			result = false;
			break;
		}

		break;
	}

// ���������� ����������� �������
//	digital->ResetOutSignal("��: ��� ��");
	digital->ResetOutSignal("��: �������� 1");
	digital->ResetOutSignal("��: �������� 2");
	digital->ResetOutSignal("��: �������� 3");

	digital->ResetOutSignal("��: STF");

	digital->ResetOutSignal("������ ����");
	digital->ResetOutSignal("������ �������");
	digital->ResetOutSignal("����� �������");

//	digital->ResetOutSignal("���� ��� ��");
	digital->ResetOutSignal("���� �������� 1");
	digital->ResetOutSignal("���� �������� 2");
	digital->ResetOutSignal("���� �������� 3");
	digital->ResetOutSignal("���� STF");
	digital->ResetOutSignal("����");
	digital->ResetOutSignal("��� ������");
	digital->ResetOutSignal("��� ��������");



	if (Terminated)
	{
		return false;
	}

	if ( result)
	{
		stext2 = "";
		stext1 = "����� ����� ������� ��������";
		TPr::SendToProtocol("�����: " + stext1);
		Synchronize( &UpdateMainForm );
		return true;
	}
	else
	{
		stext1 = "����� ����� �� ��������";
		TPr::SendToProtocol("�����: " + stext1);
		Synchronize( &UpdateMainForm );
		return false;
	}
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


bool CrashForwardThread::Transit()
{
	bool result=true;				// ��������� �������� (�������/�����)
	String ModuleResult;			// ��������� ����������� �������
	isTransit = true;
	// ������� ���������� � ��
	stext2="������� ���������� ������";
	Synchronize( &UpdateMainForm );
	if ( ModuleResult != "ok")
	{
		stext1 = "����� ������� �� ��������";
		stext2 = ModuleResult;
		TPr::SendToProtocol("�������: " + stext2);
		Synchronize( &UpdateMainForm );
		return false;
	}
	// ������� ���������� � ��
	stext2="������� ����������";
	Synchronize( &UpdateMainForm );
	if ( ModuleResult != "ok")
	{
		stext1 = "����� ������� �� ��������";
		stext2 = ModuleResult;
		TPr::SendToProtocol("�������: " + stext2);
		Synchronize( &UpdateMainForm );
		return false;
	}
	// ������� ��o������� � ��
	stext2="������� ���������� ������";
	Synchronize( &UpdateMainForm );
	if ( ModuleResult != "ok")
	{
		stext1 = "����� ������� �� ��������";
		stext2 = ModuleResult;
		TPr::SendToProtocol("�������: " + stext2);
		Synchronize( &UpdateMainForm );
		return false;
	}

	while (result)
	{
// ������������� ����� ������ ����������� �����
		digital->SetOutSignal("������ �������"); //�������");
		stext2="���� ���������� �����";
		Synchronize( &UpdateMainForm );
		if ( !digital->WaitForInSignal("������ ������",5000, TThread::CurrentThread) )
		{
			stext2="������ ����������� �����!";
			TPr::SendToProtocol("�������: " + stext2);
			result=false;
			break;
		}

// ������������� ����� ������ ����������� �������
		digital->SetOutSignal("����� �������");
		stext2="���� ���������� �������";
		Synchronize( &UpdateMainForm );
		if ( !digital->WaitForInSignal("����� ������",5000, TThread::CurrentThread) )
		{
			stext2="������ ����������� �������!";
			TPr::SendToProtocol("�������: " + stext2);
			result=false;
			break;
		}
// ������������� ����������� ������� � ���
		TPr::SendToProtocol("�������: ���������� ����������� �������" );

		digital->SetOutSignal("��� ������");
		digital->SetOutSignal("��� ��������");
		stext2 = "���� ����� ����� ������ (��1) ����������";
		Synchronize( &UpdateMainForm );
		if ( !digital->WaitForInSignal("��� ��1", 0 , TThread::CurrentThread) )
		{
			stext2 = "�� ��������� ����� ����� ������!";
			TPr::SendToProtocol("�������: " + stext2);
			result = false;
			break;
		}

		stext2 = "���� ����� �� ������ 1 ����������";
		Synchronize( &UpdateMainForm );
		if ( !digital->WaitForInSignal("������ 1", 0 , TThread::CurrentThread) )
		{
			stext2 = "����� �� �������� �����";
			TPr::SendToProtocol("�������: " + stext2);
			result = false;
			break;
		}

		stext2="����� ���� ������";
		Synchronize( &UpdateMainForm );
		digital->SetOutSignal("��: STF");

		stext2 = "���� ����� �� ������ 4";
		Synchronize( &UpdateMainForm );
		if ( !digital->WaitForInSignal("������ 4", 240000 , TThread::CurrentThread) )
		{
			stext2 = "�� ��������� ����� �� ������ 4!";
			TPr::SendToProtocol("�������: " + stext2);
			result = false;
			break;
		}

		stext2 = "���� ����� ����� �� ������ 4";
		Synchronize( &UpdateMainForm );
		if ( !digital->WaitForDropInSignal("������ 4", 120000 , TThread::CurrentThread) )
		{
			stext2 = "�� ��������� ����� ����� �� ������ 4!";
			TPr::SendToProtocol("�������: " + stext2);
			result = false;
			break;
		}
//		digital->ResetOutSignal("��: STF");

		stext2="���� ��������� ����� � ����� ����� (��2)";
		Synchronize( &UpdateMainForm );
		if ( !digital->WaitForInSignal("��� ��2", 120000 , TThread::CurrentThread) )
		{
			stext2 = "�� ��������� ����� � ����� �����!";
			TPr::SendToProtocol("�������: " + stext2);
			result = false;
			break;
		}

		break;
	}
	// ����� ���� ��������
//	digital->ResetOutSignal("��: ��� ��");
//	digital->ResetOutSignal("��: �������� 2");
//	digital->ResetOutSignal("��: �������� 1");
//	digital->ResetOutSignal("��: �������� 3");


	TThread::CurrentThread->Sleep(1000);
//	digital->ResetOutSignal("��: STF");

	digital->ResetOutSignal("������ �������");
	digital->ResetOutSignal("������ ����");
	digital->ResetOutSignal("����� �������");

	digital->ResetOutSignal("��� ������");
	digital->ResetOutSignal("��� ��������");


	if (Terminated)
	{
		return false;
	}

	if ( result)
	{
		stext2 = "";
		stext1 = "����� ������� ������� ��������";
		TPr::SendToProtocol("�������: " + stext1);
		Synchronize( &UpdateMainForm );
		return true;
	}
	else
	{
		stext1 = "����� ������� �� ��������";
		TPr::SendToProtocol("�������: " + stext1);
		Synchronize( &UpdateMainForm );
		return false;
	}
}
