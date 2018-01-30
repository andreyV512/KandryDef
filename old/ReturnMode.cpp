//---------------------------------------------------------------------------


#pragma hdrstop

#include "ReturnMode.h"
#include "SignalsState.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

//---------------------------------------------------------------------------
__fastcall ReturnThread::ReturnThread(bool CreateSuspended , TStatusBar *St, AlarmThreadClass &_alth)
				: TThread(CreateSuspended)
{
	StatusBar = St;
}
//---------------------------------------------------------------------------
void __fastcall ReturnThread::UpdateMainForm()
{
	StatusBar->Panels->Items[1]->Text = stext1;
	StatusBar->Panels->Items[2]->Text = stext2;
	StatusBar->Refresh();
}
//---------------------------------------------------------------------------
void __fastcall ReturnThread::Execute()
{
	NameThreadForDebugging("ReturnThread");

	stext1 = "Режим \"Возврат\"";
	stext2 = "Начинаем работу";
	Synchronize( &UpdateMainForm );
	cool = ReturnMode();
	Terminate();
	return;
}
//---------------------------------------------------------------------------
bool ReturnThread::ReturnMode()
{
	bool result=true;
	String ModuleResult;

	while ( result )
	{
// проверяем, не застряла ли труба в модулях
		if ( digital->CheckInSignal("прод вход") ||
				digital->CheckInSignal("пРод выход") ||
				digital->CheckInSignal("ТолЩ Вход")  ||
				digital->CheckInSignal("ТолЩ Выход") ||
				digital->CheckInSignal("Попер Вход") ||
				digital->CheckInSignal("ПопеР Выход") )
		{
			stext2="Труба в модулях!";
			result=false;
			break;
		}
		digital->ResetOutSignal("Стойка Транзит");
		digital->ResetOutSignal("Стойка Авто");
// отводим сразу все модули
		stext2="Убираем все модули ";
		Synchronize( &UpdateMainForm );
		if ( ModuleResult != "ok")
		{
			stext2 = ModuleResult;
			result = false;
			break;
		}
// устанавливаем режим работы контроллера СТОЕК

		if ( !digital->WaitForDropInSignal("Стойка Работа",3000, TThread::CurrentThread) )
		{
			stext2="Ошибка контроллера стоек!";
			result=false;
			break;
		}
		Sleep(500);
		digital->SetOutSignal("Стойка Транзит");
		stext2="Ждем контроллер стоек";
		Synchronize( &UpdateMainForm );
		if ( !digital->WaitForInSignal("Стойка Прогон",3000, TThread::CurrentThread) )
		{
			stext2="Ошибка контроллера стоек!";
			result=false;
			break;
		}

// устанавливаем режим работы контроллера СХВАТОВ
		digital->ResetOutSignal("Схват Транзит");
		digital->ResetOutSignal("Схват Авто");
		if ( !digital->WaitForDropInSignal("Схват Работа",3000, TThread::CurrentThread) )
		{
			stext2="Ошибка контроллера схватов!";
			result=false;
			break;
		}
		digital->SetOutSignal("Схват Транзит");
		stext2="Ждем контроллер схватов";
		Synchronize( &UpdateMainForm );
		if ( !digital->WaitForInSignal("Схват Прогон",3000, TThread::CurrentThread) )
		{
			stext2="Ошибка контроллера схватов!";
			result=false;
			break;
		}

// устанавливаем выходы на частотник транспорта, труба едет назад
		stext2="Труба едет назад";
		Synchronize( &UpdateMainForm );


		stext2="Ждем появления трубы в начале линии (ТР1)";
		Synchronize( &UpdateMainForm );
		/*if ( digital->CheckInBouncelessSignal("Цех Реверс" , true) )
		{   */
			stext2="Ждем трубу на стойке 1";
			Synchronize( &UpdateMainForm );
			if ( !digital->WaitForInSignal("Стойка 1",55000, TThread::CurrentThread) )
			{
				stext2="Не дождались трубу на стойке 1!";
				result=false;
				break;
			}

			stext2="Ждем схода трубы со стойки 1";
			Synchronize( &UpdateMainForm );
			if ( !digital->WaitForDropInSignal("Стойка 1",55000, TThread::CurrentThread) )
			{
				stext2="Не дождались схода трубы со стойки 1!";
				result=false;
				break;
			}

	 /*	}
		else
		{
			while (true)
			{
				if ( digital->WaitForInSignal("цех тр1", 180000 , TThread::CurrentThread) )
				{
					if ( digital->CheckInBouncelessSignal("цех тр1",true) )
					{
						break;
					}
					else
						continue;
				}
				else
				{
					stext2 = "Не дождались трубу в начале линии!";
					result = false;
					break;
				}
			}
		}   */
// цикл идет только 1 раз
		break;
	}

// сбрасываем управляющие сигналы
//	digital->ResetOutSignal("ТС: Пит ПЧ");
	digital->ResetOutSignal("ТС: скорость 2");
	Sleep(1500);
	digital->ResetOutSignal("ТС: STR");

	digital->ResetOutSignal("Стойка транзит");
	digital->ResetOutSignal("Схват Транзит");
	digital->ResetOutSignal("Цех назад");
	digital->ResetOutSignal("Цех движение");


	if (Terminated)
	{
    	return false;
	}

	if ( result)
	{
		stext2="";
		stext1="Режим Возврат успешно завершен";
		TPr::SendToProtocol("Возврат: " + stext1);
		Synchronize( &UpdateMainForm );
		return true;
	}
	else
	{
		stext1="Режим Возврат не завершен";
		TPr::SendToProtocol("Выгон: " + stext1);
		Synchronize( &UpdateMainForm );
		return false;
	}
}




