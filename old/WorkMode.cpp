//---------------------------------------------------------------------------

#pragma hdrstop

#include "IniFiles.hpp"

#include "WorkMode.h"
#include "Main.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

//---------------------------------------------------------------------------

	int centralizerSleep = 500;		// время задержки перед срабатыванием схвата
// удалить
	int SleepTemp=750;

//-----------------------------------------------------------------------------
__fastcall WorkThreadClass::WorkThreadClass(bool CreateSuspended, 		// создавать поток замороженным
					TStatusBar *St,							// указатель на статусбар, показывающий течение процесса
					bool _Linear,                           // используем ли продольный)
					bool IsPrepare)							// первый цикл (нужно ли готовить к работе систему)
	: TThread(CreateSuspended)
{
	St1 = St;
	Linear = _Linear;
	Prepare = IsPrepare;

	// запишем длины модулей для расчета скорости
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

	stext1 = "Режим \"Работа\"";
	stext2 = "Готовим к исходному положению";
	TPr::SendToProtocol("-----------");
	TPr::SendToProtocol(stext1);
	Synchronize( &UpdateMainForm );

	if (Prepare)
	{
		UnicodeString prepare_result = PrepareForWork( );
		if ( prepare_result != "ok" )
		{
			stext1 = "Режим \"Работа\" не завершен!";
			stext2 = prepare_result;
			TPr::SendToProtocol("Работа: " + stext2);
			Synchronize( &UpdateMainForm );

			digital->ResetOutSignal("Стойка Авто");
			digital->ResetOutSignal("Схват Авто");
			cool = false;
			return;
		}
	}

	TPr::SendToProtocol("Работа: Подготовка прошла успешно" );
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
// проверяем, включен ли модуль группы прочности
	if (digital->SignalExists("ГП включен") && !digital->CheckInSignal("ГП включен"))
		return "Включите группу прочности!";

	return "ok";
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
bool WorkThreadClass::WorkMode()
{
	stext2 = "Начинаем работу";
	Synchronize( &UpdateMainForm );
	double lin_v;
	double cr_v;
	double st_v;

// прочитаем скорость вращения из ini
	TIniFile *ini = new TIniFile(Globals::IniFileName);
	String Sect = "Type_" + MainForm->cbTypeSize->Text;
	delete ini;
// открываем фиктивный цикл, который идет 1 раз
	bool result = true;
	while ( result )
	{
// ждем трубу перед позицией (сигнал ТР1)
		stext2 = "Ждем трубу перед позицией бесконечно";
		TPr::SendToProtocol("Работа: " + stext2);
		Synchronize( &UpdateMainForm );
		if ( !digital->WaitForInSignal("Цех тр1", 0, TThread::CurrentThread) )
		{
			stext2 = "Не дождались трубу перед позицией! (сигнала тр1)";
			TPr::SendToProtocol("Работа: " + stext2);
			result = false;
			break;
		}
		digital->ResetOutSignal("Цех Результат");
		digital->ResetOutSignal("Цех Признак");

// включаем магнитное поле, проверяем наличие
		digital->SetOutSignal("Соленоид Охлажд");
		digital->SetOutSignal("Соленоид питание");
		TThread::CurrentThread->Sleep(500);

// посмотрим, сколько датчиков используется
	TPr::SendToProtocol("Работа: типоразмер " + IntToStr(Globals::current_diameter));
	Singleton::Instance()->CrossResult->sensors = (Globals::current_diameter == 60) ? (10) : (12);
	Singleton::Instance()->LinearResult->sensors = 4;


// иниициализируем сбор данных с LCard
		TPr::SendToProtocol("Работа: инициализируем настройки LCard");
		lcard->setSettings();
// ждем сигнала Готовность от толщиномера
		digital->SetOutSignal("Работа");
		if ( Thick )
		{
			stext2="Ждем готовности толщиномера";
			Synchronize( &UpdateMainForm );
			if ( !digital->WaitForInSignal("Готовность",30000, TThread::CurrentThread))
			{
				stext2 = "Не дождались готовности толщиномера";
				TPr::SendToProtocol("Работа: " + stext2);
				result = false;
				break;
			}
		}

// крутим продольный
		if ( Linear )
		{
			digital->SetOutSignal("Прод Пит ПЧ");
			digital->SetOutSignal("Прод STF");

		}
			if ( Terminated )
				break;

// ждем вращения толщиномера
		digital->SetOutSignal("Цикл");
		if ( Thick )
		{
			stext2="Ждем вращения толщиномера";
			Synchronize( &UpdateMainForm );
			if ( !digital->WaitForInSignal("Контроль",10000, TThread::CurrentThread))
			{
				stext2 = "Не дождались вращения толщиномера";
				TPr::SendToProtocol("Работа: " + stext2);
				result = false;
				break;
			}
		}

// включаем движение трубы вперед
		digital->SetOutSignal("ТС: Пит ПЧ");

		digital->SetOutSignal("ТС: STF");
		if ( !digital->WaitForDropInSignal("ТС: ПЧ RUN",3000, TThread::CurrentThread) )
		{
			stext2 = "Не достигнута скорость вращения транспорта";
			TPr::SendToProtocol("Работа: " + stext2);
			result = false;
			break;
		}
		if ( Terminated )
			break;

// выставляем сигналы в цех, труба поехала
		digital->SetOutSignal("Цех вперед");
		digital->SetOutSignal("Цех движение");

//************************труба едет********************************************
// ждем трубу на первой стойке
		stext2 = "Ждем трубу на стойке 1";
		TPr::SendToProtocol("Работа: " + stext2);
		Synchronize( &UpdateMainForm );
		if ( !digital->WaitForInSignal("Стойка 1", 10000, TThread::CurrentThread ) )
		{
			stext2 = "Не дождались трубы на стойке 1!";
			TPr::SendToProtocol("Работа: " + stext2);
			result = false;
			break;
		}
		st1 = GetTickCount();
		if ( Terminated )
			break;
// ждем трубу на входе поперечного
		stext2 = "Ждем трубу на входе поперечного";
		Synchronize( &UpdateMainForm );
		if ( digital->WaitForInSignal("Попер вход",10000, TThread::CurrentThread) )
		{
			TThread::CurrentThread->Sleep(900);
			lcard->CrossIsStarted();
			lcard->Start();
			cross_t1 = GetTickCount();
		}
		else
		{
			stext2 = "Не дождались трубы на входе поперечного!";
			TPr::SendToProtocol("Работа: " + stext2);
			result = false;
			break;
		}
		if ( Terminated )
			break;
// ждем трубу на входе продольного, если нужно
		if ( Linear )
		{
			stext2 = "Ждем трубу на входе продольного";
			Synchronize( &UpdateMainForm );
			digital->SetOutSignal("Прод пит су");
			if ( digital->WaitForInSignal("Прод вход",15000, TThread::CurrentThread) )
			{
				TThread::CurrentThread->Sleep(900);
				lcard->LinearIsStarted();
				lin_t1 = GetTickCount();
			}
			else
			{
				stext2 = "Не дождались трубы на входе продольного!";
				TPr::SendToProtocol("Работа: " + stext2);
				result = false;
				break;
			}
		}

// ждем трубу на стойке 4
		stext2 = "Ждем трубу на стойке 4";
		TPr::SendToProtocol("Работа: " + stext2);
		Synchronize( &UpdateMainForm );
		if ( !digital->WaitForInSignal("Стойка 4",15000, TThread::CurrentThread) )
		{
			stext2 = "Не дождались трубы на стойке 4!";
			TPr::SendToProtocol("Работа: " + stext2);
			result = false;
			break;
		}
		st4 = GetTickCount();
		digital->SetOutSignal("База");
		//по новой схеме
		if ( digital->CheckInSignal("Прод выход") )
		{
			digital->ResetOutSignal("Прод скорость 1");
			digital->ResetOutSignal("Прод скорость 2");
			digital->ResetOutSignal("Прод скорость 3");
		//	ProtocolForm->SendToProtocol("Рабочая скорость вращения продольного");
		}

// ждем трубу на входе в гп
		stext2 = "Ждем трубу на входе в модуль гп";
		TPr::SendToProtocol("Работа: " + stext2);
		Synchronize( &UpdateMainForm );
		if ( !digital->WaitForInSignal("гп вход",2000, TThread::CurrentThread) )
		{
			stext2 = "Не дождались входа трубы в модуль гп!";
			TPr::SendToProtocol("Работа: " + stext2);
			result = false;
			break;
		}

// ждем схода трубы со стойки 1
		stext2 = "Ждем схода трубы со стойки 1";
		TPr::SendToProtocol("Работа: " + stext2);
		Synchronize( &UpdateMainForm );
		if ( !digital->WaitForDropInSignal( "Стойка 1" , 100000 , TThread::CurrentThread) )
		{
			stext2 = "Не дождались выхода трубы со стойки 1!";
			TPr::SendToProtocol("Работа: " + stext2);
			result = false;
			break;
		}
		st1_out = GetTickCount();

// ждем схода трубы с датчиков попера
		if ( !digital->WaitForDropInSignal( "Попер вход" , 4000 , TThread::CurrentThread) )
		{
			stext2 = "Не дождались схода трубы со входа поперечного!";
			TPr::SendToProtocol("Работа: " + stext2);
			result = false;
			break;
		}
		cross_t3 = GetTickCount();
		TThread::CurrentThread->Sleep(1000);
		lcard->CrossIsStopped();

		stext2 = "Ждем выхода трубы из поперечного";
		Synchronize( &UpdateMainForm );
		if ( !digital->WaitForDropInSignal( "Попер выход" , 4000 , TThread::CurrentThread) )
		{
			stext2 = "Не дождались выхода трубы из поперечного!";
			TPr::SendToProtocol("Работа: " + stext2);
			result = false;
			break;
		}
		digital->ResetOutSignal("соленоид питание");

		st_v = st_len / (st4 - st1);
		double st_tube = st_v * ( st1_out - st1);
		short cr_zones = (short) Math::Ceil( st_tube / zone_len);
		if (cr_zones > Globals::max_zones)
			cr_zones = Globals::max_zones;
		Singleton::Instance()->CrossResult->zones = cr_zones;
		sms->length_in_zones = cr_zones;

// ждем выхода из толщиномера
		if (Thick)
		{
			stext2 = "Ждем выхода трубы из толщиномера";
			Synchronize( &UpdateMainForm );
			if ( !digital->WaitForDropInSignal( "Толщ вход" , 8000 , TThread::CurrentThread) )
			{
				stext2 = "Не дождались схода трубы со входа толщиномера!";
				TPr::SendToProtocol("Работа: " + stext2);
				result = false;
				break;
			}

		}
		if (Linear)
		{
			digital->ResetOutSignal("Прод скорость 1");
			digital->ResetOutSignal("Прод скорость 2");
			digital->ResetOutSignal("Прод скорость 3");
			TPr::SendToProtocol("Выходная скорость вращения продольного");
		}

// ждем схода трубы с датчиков продольного
		if ( Linear )
		{
			if ( !digital->WaitForDropInSignal( "Прод вход" , 10000 , TThread::CurrentThread) )
			{
				stext2 = "Не дождались схода трубы со входа продольного!";
				TPr::SendToProtocol("Работа: " + stext2);
				result = false;
				break;
			}
			lin_t3 = GetTickCount();
			TThread::CurrentThread->Sleep(1200);
			lcard->LinearIsStopped();

			stext2 = "Ждем выхода трубы из продольного";
			Synchronize( &UpdateMainForm );
			if ( !digital->WaitForDropInSignal( "Прод выход" , 25000 , TThread::CurrentThread) )
			{
				stext2 = "Не дождались выхода трубы из продольного!";
				result = false;
				break;
			}
			digital->ResetOutSignal("Прод STF");
		}
	lcard->Stop();
// труба уехала, можно многое отключить
	stext2 = "Ждем схода трубы со стойки 4";
	TPr::SendToProtocol("Работа: " + stext2);
	Synchronize( &UpdateMainForm );
	if ( !digital->WaitForDropInSignal( "Стойка 4" , 30000 , TThread::CurrentThread) )
	{
		stext2 = "Не дождались схода трубы со стойки 4!";
		TPr::SendToProtocol("Работа: " + stext2);
		result = false;
		break;
	}
	digital->ResetOutSignal("ТС: STF");
	digital->ResetOutSignal("База");

	stext2 = "Ждем выхода трубы из установки";
	TPr::SendToProtocol("Работа: " + stext2);
	Synchronize( &UpdateMainForm );
	if( !digital->WaitForDropInSignal( "гп выход" , 5000 , TThread::CurrentThread) )
	{
		stext2 = "Не дождались выхода трубы из установки!";
		TPr::SendToProtocol("Работа: " + stext2);
		result = false;
		break;
	}

// просчитываем данные продольного
	if ( Linear )
	{
		short lin_zones = (short) Math::Ceil( st_tube / zone_len );
		if (lin_zones > Globals::max_zones)
			lin_zones = Globals::max_zones;
		Singleton::Instance()->LinearResult->zones = lin_zones;
		Singleton::Instance()->LinearResult->FillData(lcard->getLinearMeasure());
		Singleton::Instance()->LinearResult->ComputeZoneData(true);
		Singleton::Instance()->LinearResult->CalculateDeadZone(zone_len );
		TPr::SendToProtocol("Работа: рассчитали данные продольного" );
	}

	Singleton::Instance()->CrossResult->FillData(lcard->getCrossMeasure());
	Singleton::Instance()->CrossResult->ComputeZoneData();
	Singleton::Instance()->CrossResult->CalculateDeadZone(zone_len);

	TPr::SendToProtocol("Работа: рассчитали данные поперечного" );

	digital->ResetOutSignal("соленоид охлажд");
	digital->ResetOutSignal("соленоид питание");
	digital->ResetOutSignal("Работа");

// ждем, когда труба приедет в конец
		stext2 = "Ждем трубу в конце линии (ТР2)";
		TPr::SendToProtocol("Работа: " + stext2);
		Synchronize( &UpdateMainForm );
	  /*	while (true)
		{
			if ( Terminated )
				break;
			if ( digital->WaitForInSignal( "Цех ТР2" , 50000 , TThread::CurrentThread) )
			{
				if ( digital->CheckInBouncelessSignal("цех тр2",true) )
				{
					ProtocolForm->SendToProtocol("Работа: Получили ТР2");
					break;
				}
				else
					continue;
			}
			else
			{
				stext2 = "Не дождались трубу в конце линии! (ТР2)";
				ProtocolForm->SendToProtocol("Работа: " + stext2);
				result = false;
				break;
			}
		}
		*/
		break;
	}

	// сбросим управляющие сигналы
	digital->ResetOutSignal("соленоид охлажд");
	digital->ResetOutSignal("соленоид питание");
//	digital->ResetOutSignal("ТС: Пит ПЧ");
	digital->ResetOutSignal("ТС: скорость 1");
	digital->ResetOutSignal("ТС: скорость 2");
	digital->ResetOutSignal("ТС: скорость 3");
	digital->ResetOutSignal("ТС: STF");

	digital->ResetOutSignal("Стойка Авто");
	digital->ResetOutSignal("Схват Авто");

	digital->ResetOutSignal("Работа");
	digital->ResetOutSignal("Цикл");
	digital->ResetOutSignal("База");

	digital->ResetOutSignal("Цех вперед");
	digital->ResetOutSignal("Цех движение");

	if ( Linear )
	{
//		digital->ResetOutSignal("Прод Пит ПЧ");
		digital->ResetOutSignal("Прод STF");
		digital->ResetOutSignal("Прод Скорость 1");
		digital->ResetOutSignal("Прод Скорость 2");
		digital->ResetOutSignal("Прод Скорость 3");
		digital->ResetOutSignal("Прод пит су");
	}

	if ( Terminated )
	{
		lcard->Stop();


		TPr::SendToProtocol("Работа: Выпали из цикла по Terminate");
		ReturnValue = 0;
		return false;
	}

	if ( result)
	{
		stext1 = "Режим \"Работа\" завершен успешно";
		TPr::SendToProtocol(stext1);
		stext2 = "";
		Synchronize( &UpdateMainForm );
		return true;
	}
	else
	{

		lcard->Stop();

		stext1 = "Режим \"Работа\" не завершен!";
		TPr::SendToProtocol(stext1);
		Synchronize( &UpdateMainForm );
		return false;
	}
}

//-----------------------------------------------------------------------------

