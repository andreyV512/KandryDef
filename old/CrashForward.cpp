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
// если модули пусты - то Транзит
	if ( !digital->CheckInSignal("Прод вход") &&
			!digital->CheckInSignal("Прод выход") &&
			!digital->CheckInSignal("Толщ вход") &&
			!digital->CheckInSignal("Толщ выход") &&
			!digital->CheckInSignal("попер вход") &&
			!digital->CheckInSignal("попер выход") &&
			!digital->CheckInSignal("стойка 1") &&
			!digital->CheckInSignal("стойка 3") &&
			!digital->CheckInSignal("стойка 4") )
	{
		stext1 = "Режим \"Транзит\"";
		stext2 = "Начинаем работу";
		TPr::SendToProtocol("-----------------");
		TPr::SendToProtocol("Режим Транзит");
		Synchronize( &UpdateMainForm );
		cool = Transit();
	}
// если где-то труба застряла - то Выгон
	else
	{
		stext1 = "Режим \"Выгон\"";
		stext2 = "Начинаем работу";
		TPr::SendToProtocol("-----------------");
		TPr::SendToProtocol("Режим Выгон");
		Synchronize( &UpdateMainForm );
		cool = CrashForwardMode();
	}

	Terminate();
	return;
}
//---------------------------------------------------------------------------

bool CrashForwardThread::CrashForwardMode()
{
	bool result=true;				// результат выгона (успешно/завал)
	bool lin = false;				// участвует ли продольный в выгоне
	bool thick = false;				// участвует ли толщиномер в выгоне
	bool cross = false;				// участвует ли поперечный в выгоне
	String ModuleResult;			// результат перемещения модуля
	isTransit = false;
	while (result)
	{

// смотрим, где застряла труба
		thick = ( ( digital->CheckInSignal("Толщ вход") || digital->CheckInSignal("Толщ выход") ||
					digital->CheckInSignal("Стойка 3")  || digital->CheckInSignal("Попер выход") )
				 && digital->CheckInSignal("Толщ рп") );
		lin = ( ( digital->CheckInSignal("Прод вход") || digital->CheckInSignal("Прод выход") ||
				  digital->CheckInSignal("Стойка 3")  || digital->CheckInSignal("Стойка 4") )
				&& digital->CheckInSignal("Прод рп") );
		cross = ( ( digital->CheckInSignal("Попер вход") || digital->CheckInSignal("Попер выход") ||
					digital->CheckInSignal("Стойка 1")  || digital->CheckInSignal("Толщ вход") )
				 && digital->CheckInSignal("Попер рп") );
// труба застряла в поперечном, продольный уберем
		if ( cross && !lin )
		{
			stext2 = "Отводим продольный модуль";
			Synchronize( &UpdateMainForm );
			if ( ModuleResult != "ok")
			{
				stext2 = ModuleResult;
				TPr::SendToProtocol("Выгон: " + stext2);
				result = false;
				break;
			}
		}
// труба застряла в поперечном, толщину уберем
		if ( cross && !thick )
		{
			stext2 = "Отводим модуль толщинометрии";
			Synchronize( &UpdateMainForm );
			if ( ModuleResult != "ok")
			{
				stext2 = ModuleResult;
				TPr::SendToProtocol("Выгон: " + stext2);
				result = false;
				break;
			}
		}
// устанавливаем режим работы контроллера стоек (Выход 1 и 2)
		digital->SetOutSignal("Стойка Транзит");
		digital->SetOutSignal("Стойка Авто");
		stext2 = "Ждем контроллер стоек";
		Synchronize( &UpdateMainForm );
		if ( !( digital->WaitForInSignal("Стойка Прогон",5000 , TThread::CurrentThread)
				&& digital->CheckInSignal("Стойка Работа") ) )
		{
			stext2 = "Ошибка контроллера стоек!";
			TPr::SendToProtocol("Выгон: " + stext2);
			result = false;
			break;
		}
		digital->ResetOutSignal("Схват Транзит");
		digital->ResetOutSignal("Схват Авто");

/*
// устанавливаем режим работы контроллера схватов (Выход 1 и 2)
		digital->SetOutSignal("Схват Транзит");
		stext2 = "Ждем контроллер схватов";
		Synchronize( &UpdateMainForm );
		if ( !( digital->WaitForInSignal("Схват Прогон",3000 , TThread::CurrentThread) ) )
//		&& digital->CheckInSignal("Стойка вход2") ) )
		{
			stext2 = "Ошибка контроллера схватов!";
			result = false;
			break;
		}
*/
// ждем вращения толщины

		if (thick)
		{
			stext2="Включите вращение толщиномера";
			Synchronize( &UpdateMainForm );
			if ( !digital->WaitForInSignal("Контроль",20000 , TThread::CurrentThread))
			{
				stext2 = "Не дождались вращения толщиномера";
				TPr::SendToProtocol("Выгон: " + stext2);
				result = false;
				break;
			}
		}
		// сигнал для толщиномера
		digital->SetOutSignal("Цикл");

// вращаем продольный, если нужно
		if (lin)
		{
			stext2 = "Вращаем продольный модуль";
			Synchronize( &UpdateMainForm );
			digital->SetOutSignal("Прод Пит ПЧ");
			digital->SetOutSignal("Прод STF");
			digital->SetOutSignal("Прод скорость 1");
		}

		stext2= "Труба едет вперед";
		Synchronize( &UpdateMainForm );
		TPr::SendToProtocol("Выгон: " + stext2);
		if ( !digital->WaitForDropInSignal( "Стойка 4" , 80000 , TThread::CurrentThread) )
		{
			stext2 = "Не дождались схода трубы со стойки 4!";
			TPr::SendToProtocol("Выгон: " + stext2);
			result = false;
			break;
		}
	   //	digital->ResetOutSignal("Прод STF");

		stext2="Ждем появления трубы в конце линии (ТР2)";
		Synchronize( &UpdateMainForm );
		if ( !digital->WaitForInSignal("цех тр2", 120000 , TThread::CurrentThread) )
		{
			stext2 = "Не дождались трубу в конце линии!";
			TPr::SendToProtocol("Выгон: " + stext2);
			result = false;
			break;
		}

		break;
	}

// сбрасываем управляющие сигналы
//	digital->ResetOutSignal("ТС: Пит ПЧ");
	digital->ResetOutSignal("ТС: скорость 1");
	digital->ResetOutSignal("ТС: скорость 2");
	digital->ResetOutSignal("ТС: скорость 3");

	digital->ResetOutSignal("ТС: STF");

	digital->ResetOutSignal("Стойка Авто");
	digital->ResetOutSignal("Стойка Транзит");
	digital->ResetOutSignal("Схват Транзит");

//	digital->ResetOutSignal("Прод Пит ПЧ");
	digital->ResetOutSignal("Прод Скорость 1");
	digital->ResetOutSignal("Прод Скорость 2");
	digital->ResetOutSignal("Прод Скорость 3");
	digital->ResetOutSignal("Прод STF");
	digital->ResetOutSignal("Цикл");
	digital->ResetOutSignal("цех Вперед");
	digital->ResetOutSignal("цех движение");



	if (Terminated)
	{
		return false;
	}

	if ( result)
	{
		stext2 = "";
		stext1 = "Режим выгон успешно завершен";
		TPr::SendToProtocol("Выгон: " + stext1);
		Synchronize( &UpdateMainForm );
		return true;
	}
	else
	{
		stext1 = "Режим выгон не завершен";
		TPr::SendToProtocol("Выгон: " + stext1);
		Synchronize( &UpdateMainForm );
		return false;
	}
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


bool CrashForwardThread::Transit()
{
	bool result=true;				// результат транзита (успешно/завал)
	String ModuleResult;			// результат перемещения модулей
	isTransit = true;
	// двигаем поперечный в ПО
	stext2="Убираем поперечный модуль";
	Synchronize( &UpdateMainForm );
	if ( ModuleResult != "ok")
	{
		stext1 = "Режим Транзит не завершен";
		stext2 = ModuleResult;
		TPr::SendToProtocol("Транзит: " + stext2);
		Synchronize( &UpdateMainForm );
		return false;
	}
	// двигаем толщиномер в ПО
	stext2="Убираем толщиномер";
	Synchronize( &UpdateMainForm );
	if ( ModuleResult != "ok")
	{
		stext1 = "Режим Транзит не завершен";
		stext2 = ModuleResult;
		TPr::SendToProtocol("Транзит: " + stext2);
		Synchronize( &UpdateMainForm );
		return false;
	}
	// двигаем прoдольный в ПО
	stext2="Убираем продольный модуль";
	Synchronize( &UpdateMainForm );
	if ( ModuleResult != "ok")
	{
		stext1 = "Режим Транзит не завершен";
		stext2 = ModuleResult;
		TPr::SendToProtocol("Транзит: " + stext2);
		Synchronize( &UpdateMainForm );
		return false;
	}

	while (result)
	{
// устанавливаем режим работы контроллера СТОЕК
		digital->SetOutSignal("Стойка Транзит"); //Транзит");
		stext2="Ждем контроллер стоек";
		Synchronize( &UpdateMainForm );
		if ( !digital->WaitForInSignal("Стойка Прогон",5000, TThread::CurrentThread) )
		{
			stext2="Ошибка контроллера стоек!";
			TPr::SendToProtocol("Транзит: " + stext2);
			result=false;
			break;
		}

// устанавливаем режим работы контроллера СХВАТОВ
		digital->SetOutSignal("Схват Транзит");
		stext2="Ждем контроллер схватов";
		Synchronize( &UpdateMainForm );
		if ( !digital->WaitForInSignal("Схват Прогон",5000, TThread::CurrentThread) )
		{
			stext2="Ошибка контроллера схватов!";
			TPr::SendToProtocol("Транзит: " + stext2);
			result=false;
			break;
		}
// устанавливаем разрешающие сигналы в цех
		TPr::SendToProtocol("Транзит: установили разрешающие сигналы" );

		digital->SetOutSignal("цех вперед");
		digital->SetOutSignal("цех движение");
		stext2 = "Ждем трубу перед линией (ТР1) бесконечно";
		Synchronize( &UpdateMainForm );
		if ( !digital->WaitForInSignal("цех тр1", 0 , TThread::CurrentThread) )
		{
			stext2 = "Не дождались трубу перед линией!";
			TPr::SendToProtocol("Транзит: " + stext2);
			result = false;
			break;
		}

		stext2 = "Ждем трубу на стойке 1 бесконечно";
		Synchronize( &UpdateMainForm );
		if ( !digital->WaitForInSignal("Стойка 1", 0 , TThread::CurrentThread) )
		{
			stext2 = "Вышли из ожидания трубы";
			TPr::SendToProtocol("Транзит: " + stext2);
			result = false;
			break;
		}

		stext2="Труба едет вперед";
		Synchronize( &UpdateMainForm );
		digital->SetOutSignal("тс: STF");

		stext2 = "Ждем трубу на стойке 4";
		Synchronize( &UpdateMainForm );
		if ( !digital->WaitForInSignal("Стойка 4", 240000 , TThread::CurrentThread) )
		{
			stext2 = "Не дождались трубу на стойке 4!";
			TPr::SendToProtocol("Транзит: " + stext2);
			result = false;
			break;
		}

		stext2 = "Ждем ухода трубы со стойки 4";
		Synchronize( &UpdateMainForm );
		if ( !digital->WaitForDropInSignal("Стойка 4", 120000 , TThread::CurrentThread) )
		{
			stext2 = "Не дождались ухода трубы со стойки 4!";
			TPr::SendToProtocol("Транзит: " + stext2);
			result = false;
			break;
		}
//		digital->ResetOutSignal("тс: STF");

		stext2="Ждем появления трубы в конце линии (ТР2)";
		Synchronize( &UpdateMainForm );
		if ( !digital->WaitForInSignal("цех тр2", 120000 , TThread::CurrentThread) )
		{
			stext2 = "Не дождались трубу в конце линии!";
			TPr::SendToProtocol("Транзит: " + stext2);
			result = false;
			break;
		}

		break;
	}
	// сброс всех сигналов
//	digital->ResetOutSignal("тс: пит пч");
//	digital->ResetOutSignal("тс: скорость 2");
//	digital->ResetOutSignal("тс: скорость 1");
//	digital->ResetOutSignal("тс: скорость 3");


	TThread::CurrentThread->Sleep(1000);
//	digital->ResetOutSignal("тс: STF");

	digital->ResetOutSignal("Стойка Транзит");
	digital->ResetOutSignal("Стойка Авто");
	digital->ResetOutSignal("Схват Транзит");

	digital->ResetOutSignal("Цех вперед");
	digital->ResetOutSignal("цех движение");


	if (Terminated)
	{
		return false;
	}

	if ( result)
	{
		stext2 = "";
		stext1 = "Режим Транзит успешно завершен";
		TPr::SendToProtocol("Транзит: " + stext1);
		Synchronize( &UpdateMainForm );
		return true;
	}
	else
	{
		stext1 = "Режим Транзит не завершен";
		TPr::SendToProtocol("Транзит: " + stext1);
		Synchronize( &UpdateMainForm );
		return false;
	}
}
