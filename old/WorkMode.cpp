//---------------------------------------------------------------------------

#pragma hdrstop

#include "IniFiles.hpp"

#include "WorkMode.h"
#include "Main.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

//---------------------------------------------------------------------------

	int centralizerSleep = 500;		// ����� �������� ����� ������������� ������
// �������
	int SleepTemp=750;

//-----------------------------------------------------------------------------
__fastcall WorkThreadClass::WorkThreadClass(bool CreateSuspended, 		// ��������� ����� ������������
					TStatusBar *St,							// ��������� �� ���������, ������������ ������� ��������
					bool _Linear,                           // ���������� �� ����������)
					bool IsPrepare)							// ������ ���� (����� �� �������� � ������ �������)
	: TThread(CreateSuspended)
{
	St1 = St;
	Linear = _Linear;
	Prepare = IsPrepare;

	// ������� ����� ������� ��� ������� ��������
	TIniFile * ini = new TIniFile(Globals::IniFileName);
	cross_len = ini->ReadInteger("Size","CrossModuleLength",1000);
	linear_len = ini->ReadInteger("Size","LinearModuleLength",1000);
	zone_len = ini->ReadInteger("Size","ZoneLength",250);
	st_len = ini->ReadInteger("Size","St1St4",250);
	delete ini;
}

//-----------------------------------------------------------------------------

void __fastcall WorkThreadClass::Execute()
{
	NameThreadForDebugging("WorkThread");
	transit = false;

	stext1 = "����� \"������\"";
	stext2 = "������� � ��������� ���������";
	TPr::SendToProtocol("-----------");
	TPr::SendToProtocol(stext1);
	Synchronize( &UpdateMainForm );

	if (Prepare)
	{
		UnicodeString prepare_result = PrepareForWork( );
		if ( prepare_result != "ok" )
		{
			stext1 = "����� \"������\" �� ��������!";
			stext2 = prepare_result;
			TPr::SendToProtocol("������: " + stext2);
			Synchronize( &UpdateMainForm );

			digital->ResetOutSignal("������ ����");
			digital->ResetOutSignal("����� ����");
			cool = false;
			return;
		}
	}

	TPr::SendToProtocol("������: ���������� ������ �������" );
	SetReturnValue( WorkMode() );
	cool = ReturnValue;
	Terminate();
	return;
}
//-----------------------------------------------------------------------------

void __fastcall WorkThreadClass::UpdateMainForm()
{
	St1->Panels->Items[1]->Text = stext1;
	St1->Panels->Items[2]->Text = stext2;
	St1->Refresh();

	Application->ProcessMessages();
	SignalsStateForm->Refresh();
}

//-----------------------------------------------------------------------------
//-------------------------------------------------------------------------------
UnicodeString WorkThreadClass::PrepareForWork( )
{
// ���������, ������� �� ������ ������ ���������
	if (digital->SignalExists("�� �������") && !digital->CheckInSignal("�� �������"))
		return "�������� ������ ���������!";

	return "ok";
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
bool WorkThreadClass::WorkMode()
{
	stext2 = "�������� ������";
	Synchronize( &UpdateMainForm );
	double lin_v;
	double cr_v;
	double st_v;

// ��������� �������� �������� �� ini
	TIniFile *ini = new TIniFile(Globals::IniFileName);
	String Sect = "Type_" + MainForm->cbTypeSize->Text;
	delete ini;
// ��������� ��������� ����, ������� ���� 1 ���
	bool result = true;
	while ( result )
	{
// ���� ����� ����� �������� (������ ��1)
		stext2 = "���� ����� ����� �������� ����������";
		TPr::SendToProtocol("������: " + stext2);
		Synchronize( &UpdateMainForm );
		if ( !digital->WaitForInSignal("��� ��1", 0, TThread::CurrentThread) )
		{
			stext2 = "�� ��������� ����� ����� ��������! (������� ��1)";
			TPr::SendToProtocol("������: " + stext2);
			result = false;
			break;
		}
		digital->ResetOutSignal("��� ���������");
		digital->ResetOutSignal("��� �������");

// �������� ��������� ����, ��������� �������
		digital->SetOutSignal("�������� ������");
		digital->SetOutSignal("�������� �������");
		TThread::CurrentThread->Sleep(500);

// ���������, ������� �������� ������������
	TPr::SendToProtocol("������: ���������� " + IntToStr(Globals::current_diameter));
	Singleton::Instance()->CrossResult->sensors = (Globals::current_diameter == 60) ? (10) : (12);
	Singleton::Instance()->LinearResult->sensors = 4;


// ��������������� ���� ������ � LCard
		TPr::SendToProtocol("������: �������������� ��������� LCard");
		lcard->setSettings();
// ���� ������� ���������� �� �����������
		digital->SetOutSignal("������");
		if ( Thick )
		{
			stext2="���� ���������� �����������";
			Synchronize( &UpdateMainForm );
			if ( !digital->WaitForInSignal("����������",30000, TThread::CurrentThread))
			{
				stext2 = "�� ��������� ���������� �����������";
				TPr::SendToProtocol("������: " + stext2);
				result = false;
				break;
			}
		}

// ������ ����������
		if ( Linear )
		{
			digital->SetOutSignal("���� ��� ��");
			digital->SetOutSignal("���� STF");

		}
			if ( Terminated )
				break;

// ���� �������� �����������
		digital->SetOutSignal("����");
		if ( Thick )
		{
			stext2="���� �������� �����������";
			Synchronize( &UpdateMainForm );
			if ( !digital->WaitForInSignal("��������",10000, TThread::CurrentThread))
			{
				stext2 = "�� ��������� �������� �����������";
				TPr::SendToProtocol("������: " + stext2);
				result = false;
				break;
			}
		}

// �������� �������� ����� ������
		digital->SetOutSignal("��: ��� ��");

		digital->SetOutSignal("��: STF");
		if ( !digital->WaitForDropInSignal("��: �� RUN",3000, TThread::CurrentThread) )
		{
			stext2 = "�� ���������� �������� �������� ����������";
			TPr::SendToProtocol("������: " + stext2);
			result = false;
			break;
		}
		if ( Terminated )
			break;

// ���������� ������� � ���, ����� �������
		digital->SetOutSignal("��� ������");
		digital->SetOutSignal("��� ��������");

//************************����� ����********************************************
// ���� ����� �� ������ ������
		stext2 = "���� ����� �� ������ 1";
		TPr::SendToProtocol("������: " + stext2);
		Synchronize( &UpdateMainForm );
		if ( !digital->WaitForInSignal("������ 1", 10000, TThread::CurrentThread ) )
		{
			stext2 = "�� ��������� ����� �� ������ 1!";
			TPr::SendToProtocol("������: " + stext2);
			result = false;
			break;
		}
		st1 = GetTickCount();
		if ( Terminated )
			break;
// ���� ����� �� ����� �����������
		stext2 = "���� ����� �� ����� �����������";
		Synchronize( &UpdateMainForm );
		if ( digital->WaitForInSignal("����� ����",10000, TThread::CurrentThread) )
		{
			TThread::CurrentThread->Sleep(900);
			lcard->CrossIsStarted();
			lcard->Start();
			cross_t1 = GetTickCount();
		}
		else
		{
			stext2 = "�� ��������� ����� �� ����� �����������!";
			TPr::SendToProtocol("������: " + stext2);
			result = false;
			break;
		}
		if ( Terminated )
			break;
// ���� ����� �� ����� �����������, ���� �����
		if ( Linear )
		{
			stext2 = "���� ����� �� ����� �����������";
			Synchronize( &UpdateMainForm );
			digital->SetOutSignal("���� ��� ��");
			if ( digital->WaitForInSignal("���� ����",15000, TThread::CurrentThread) )
			{
				TThread::CurrentThread->Sleep(900);
				lcard->LinearIsStarted();
				lin_t1 = GetTickCount();
			}
			else
			{
				stext2 = "�� ��������� ����� �� ����� �����������!";
				TPr::SendToProtocol("������: " + stext2);
				result = false;
				break;
			}
		}

// ���� ����� �� ������ 4
		stext2 = "���� ����� �� ������ 4";
		TPr::SendToProtocol("������: " + stext2);
		Synchronize( &UpdateMainForm );
		if ( !digital->WaitForInSignal("������ 4",15000, TThread::CurrentThread) )
		{
			stext2 = "�� ��������� ����� �� ������ 4!";
			TPr::SendToProtocol("������: " + stext2);
			result = false;
			break;
		}
		st4 = GetTickCount();
		digital->SetOutSignal("����");
		//�� ����� �����
		if ( digital->CheckInSignal("���� �����") )
		{
			digital->ResetOutSignal("���� �������� 1");
			digital->ResetOutSignal("���� �������� 2");
			digital->ResetOutSignal("���� �������� 3");
		//	ProtocolForm->SendToProtocol("������� �������� �������� �����������");
		}

// ���� ����� �� ����� � ��
		stext2 = "���� ����� �� ����� � ������ ��";
		TPr::SendToProtocol("������: " + stext2);
		Synchronize( &UpdateMainForm );
		if ( !digital->WaitForInSignal("�� ����",2000, TThread::CurrentThread) )
		{
			stext2 = "�� ��������� ����� ����� � ������ ��!";
			TPr::SendToProtocol("������: " + stext2);
			result = false;
			break;
		}

// ���� ����� ����� �� ������ 1
		stext2 = "���� ����� ����� �� ������ 1";
		TPr::SendToProtocol("������: " + stext2);
		Synchronize( &UpdateMainForm );
		if ( !digital->WaitForDropInSignal( "������ 1" , 100000 , TThread::CurrentThread) )
		{
			stext2 = "�� ��������� ������ ����� �� ������ 1!";
			TPr::SendToProtocol("������: " + stext2);
			result = false;
			break;
		}
		st1_out = GetTickCount();

// ���� ����� ����� � �������� ������
		if ( !digital->WaitForDropInSignal( "����� ����" , 4000 , TThread::CurrentThread) )
		{
			stext2 = "�� ��������� ����� ����� �� ����� �����������!";
			TPr::SendToProtocol("������: " + stext2);
			result = false;
			break;
		}
		cross_t3 = GetTickCount();
		TThread::CurrentThread->Sleep(1000);
		lcard->CrossIsStopped();

		stext2 = "���� ������ ����� �� �����������";
		Synchronize( &UpdateMainForm );
		if ( !digital->WaitForDropInSignal( "����� �����" , 4000 , TThread::CurrentThread) )
		{
			stext2 = "�� ��������� ������ ����� �� �����������!";
			TPr::SendToProtocol("������: " + stext2);
			result = false;
			break;
		}
		digital->ResetOutSignal("�������� �������");

		st_v = st_len / (st4 - st1);
		double st_tube = st_v * ( st1_out - st1);
		short cr_zones = (short) Math::Ceil( st_tube / zone_len);
		if (cr_zones > Globals::max_zones)
			cr_zones = Globals::max_zones;
		Singleton::Instance()->CrossResult->zones = cr_zones;
		sms->length_in_zones = cr_zones;

// ���� ������ �� �����������
		if (Thick)
		{
			stext2 = "���� ������ ����� �� �����������";
			Synchronize( &UpdateMainForm );
			if ( !digital->WaitForDropInSignal( "���� ����" , 8000 , TThread::CurrentThread) )
			{
				stext2 = "�� ��������� ����� ����� �� ����� �����������!";
				TPr::SendToProtocol("������: " + stext2);
				result = false;
				break;
			}

		}
		if (Linear)
		{
			digital->ResetOutSignal("���� �������� 1");
			digital->ResetOutSignal("���� �������� 2");
			digital->ResetOutSignal("���� �������� 3");
			TPr::SendToProtocol("�������� �������� �������� �����������");
		}

// ���� ����� ����� � �������� �����������
		if ( Linear )
		{
			if ( !digital->WaitForDropInSignal( "���� ����" , 10000 , TThread::CurrentThread) )
			{
				stext2 = "�� ��������� ����� ����� �� ����� �����������!";
				TPr::SendToProtocol("������: " + stext2);
				result = false;
				break;
			}
			lin_t3 = GetTickCount();
			TThread::CurrentThread->Sleep(1200);
			lcard->LinearIsStopped();

			stext2 = "���� ������ ����� �� �����������";
			Synchronize( &UpdateMainForm );
			if ( !digital->WaitForDropInSignal( "���� �����" , 25000 , TThread::CurrentThread) )
			{
				stext2 = "�� ��������� ������ ����� �� �����������!";
				result = false;
				break;
			}
			digital->ResetOutSignal("���� STF");
		}
	lcard->Stop();
// ����� ������, ����� ������ ���������
	stext2 = "���� ����� ����� �� ������ 4";
	TPr::SendToProtocol("������: " + stext2);
	Synchronize( &UpdateMainForm );
	if ( !digital->WaitForDropInSignal( "������ 4" , 30000 , TThread::CurrentThread) )
	{
		stext2 = "�� ��������� ����� ����� �� ������ 4!";
		TPr::SendToProtocol("������: " + stext2);
		result = false;
		break;
	}
	digital->ResetOutSignal("��: STF");
	digital->ResetOutSignal("����");

	stext2 = "���� ������ ����� �� ���������";
	TPr::SendToProtocol("������: " + stext2);
	Synchronize( &UpdateMainForm );
	if( !digital->WaitForDropInSignal( "�� �����" , 5000 , TThread::CurrentThread) )
	{
		stext2 = "�� ��������� ������ ����� �� ���������!";
		TPr::SendToProtocol("������: " + stext2);
		result = false;
		break;
	}

// ������������ ������ �����������
	if ( Linear )
	{
		short lin_zones = (short) Math::Ceil( st_tube / zone_len );
		if (lin_zones > Globals::max_zones)
			lin_zones = Globals::max_zones;
		Singleton::Instance()->LinearResult->zones = lin_zones;
		Singleton::Instance()->LinearResult->FillData(lcard->getLinearMeasure());
		Singleton::Instance()->LinearResult->ComputeZoneData(true);
		Singleton::Instance()->LinearResult->CalculateDeadZone(zone_len );
		TPr::SendToProtocol("������: ���������� ������ �����������" );
	}

	Singleton::Instance()->CrossResult->FillData(lcard->getCrossMeasure());
	Singleton::Instance()->CrossResult->ComputeZoneData();
	Singleton::Instance()->CrossResult->CalculateDeadZone(zone_len);

	TPr::SendToProtocol("������: ���������� ������ �����������" );

	digital->ResetOutSignal("�������� ������");
	digital->ResetOutSignal("�������� �������");
	digital->ResetOutSignal("������");

// ����, ����� ����� ������� � �����
		stext2 = "���� ����� � ����� ����� (��2)";
		TPr::SendToProtocol("������: " + stext2);
		Synchronize( &UpdateMainForm );
	  /*	while (true)
		{
			if ( Terminated )
				break;
			if ( digital->WaitForInSignal( "��� ��2" , 50000 , TThread::CurrentThread) )
			{
				if ( digital->CheckInBouncelessSignal("��� ��2",true) )
				{
					ProtocolForm->SendToProtocol("������: �������� ��2");
					break;
				}
				else
					continue;
			}
			else
			{
				stext2 = "�� ��������� ����� � ����� �����! (��2)";
				ProtocolForm->SendToProtocol("������: " + stext2);
				result = false;
				break;
			}
		}
		*/
		break;
	}

	// ������� ����������� �������
	digital->ResetOutSignal("�������� ������");
	digital->ResetOutSignal("�������� �������");
//	digital->ResetOutSignal("��: ��� ��");
	digital->ResetOutSignal("��: �������� 1");
	digital->ResetOutSignal("��: �������� 2");
	digital->ResetOutSignal("��: �������� 3");
	digital->ResetOutSignal("��: STF");

	digital->ResetOutSignal("������ ����");
	digital->ResetOutSignal("����� ����");

	digital->ResetOutSignal("������");
	digital->ResetOutSignal("����");
	digital->ResetOutSignal("����");

	digital->ResetOutSignal("��� ������");
	digital->ResetOutSignal("��� ��������");

	if ( Linear )
	{
//		digital->ResetOutSignal("���� ��� ��");
		digital->ResetOutSignal("���� STF");
		digital->ResetOutSignal("���� �������� 1");
		digital->ResetOutSignal("���� �������� 2");
		digital->ResetOutSignal("���� �������� 3");
		digital->ResetOutSignal("���� ��� ��");
	}

	if ( Terminated )
	{
		lcard->Stop();


		TPr::SendToProtocol("������: ������ �� ����� �� Terminate");
		ReturnValue = 0;
		return false;
	}

	if ( result)
	{
		stext1 = "����� \"������\" �������� �������";
		TPr::SendToProtocol(stext1);
		stext2 = "";
		Synchronize( &UpdateMainForm );
		return true;
	}
	else
	{

		lcard->Stop();

		stext1 = "����� \"������\" �� ��������!";
		TPr::SendToProtocol(stext1);
		Synchronize( &UpdateMainForm );
		return false;
	}
}

//-----------------------------------------------------------------------------

