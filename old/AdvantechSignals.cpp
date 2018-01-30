#pragma hdrstop
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "TeEngine.hpp"
#include "TeeProcs.hpp"
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <Math.h>
#include "Chart.hpp"
#include "Series.hpp"

#include "AdvantechSignals.h"


#include "IniFiles.hpp"
#include "Global.h"
//-----------------------------------------------------------------------------

#pragma package(smart_init)

AllSignals *digital;

Advantech1730::Advantech1730()
{
	AdvMutex = new TMutex( false );
	DoValue = 0;
	startPort = 0;
	// используем в БУРАНЕ только 16 входов и выходов
	portCount = 2;
	bufferForWriting[0] = 0;
	bufferForWriting[1] = 0;
	bufferForWriting[2] = 0;
	bufferForWriting[3] = 0;
	bufferForReading[0] = 0;
	bufferForReading[1] = 0;
	bufferForReading[2] = 0;
	bufferForReading[3] = 0;
	device=NULL;
	dio = NULL;
	SleepTime = 10;			// время "засыпания" программы (проверка установления выходов)
	blinkTime = 1000;		//	время, в течение которого держится выход в команде BlinkOut
	bouncePeriod = 10;		// время дребезга - интервал, через который будет проверяться состояние сигнала
	bounceRepeat = 5;		// кол-во повторений дребезга
}
//------------------------------------------------------------------------------
Advantech1730::~Advantech1730()
{
	delete AdvMutex;
}
//------------------------------------------------------------------------------
bool Advantech1730::Init(long DevNum, bool ReadOnly)
{
// если плата была открыта, закрываем ее
		if (Advantech1730::device != NULL)
		{
			Advantech1730::device->Close();
			Advantech1730::device = NULL;
		}
// открываем плату только для чтения или с полным доступом
		AccessMode AM = (ReadOnly) ? (ModeRead) : (ModeWrite);
			errorCode = BDaqDevice::Open(DevNum, AM, Advantech1730::device);
			if (BioFailed(errorCode))
			{
//				Application->MessageBoxW(L" Ошибка открытия платы Advantech 1730U",L"Ошибка!!!",MB_OK);
				return false;
			}
			else if (errorCode==Success)
			{
				errorCode = Advantech1730::device->GetModule(0, dio);//Get Dio Module
				if ( errorCode == Success && ReadOnly )
					return true;

				else if (errorCode == Success && !ReadOnly)
				{
					//write and read back DO ports' status
					for ( int i = startPort;i < portCount + startPort; ++i)
					bufferForWriting[i-startPort] = 0x00;
					//Write DO ports’Status
					errorCode = dio->DoWrite(startPort, portCount, bufferForWriting);
					if (errorCode == Success)
					{
						TThread::CurrentThread->Sleep(SleepTime);
						//Read back DO ports’Status
						errorCode = dio->DoRead(startPort, portCount, bufferForReading);
					}
					else return false;
				}
				else return false;
			}
		return true;
}
//------------------------------------------------------------------------------
bool Advantech1730::Close()
{
	if (device != NULL)
	{
		device->Close();
		device = NULL;
		return true;
	}
	else return false;
}
//------------------------------------------------------------------------------
bool Advantech1730::SetOutWord(unsigned int OutWord)
{
	   //write and read back DO ports' status
	   bufferForWriting[0] = OutWord & 0xFF;
	   bufferForWriting[1] = OutWord >> 8 & 0xFF ;
	   bufferForWriting[2] = OutWord >> 16 & 0xFF;
	   bufferForWriting[3] = OutWord >> 24 & 0xFF;
	   //Write DO ports’Status
	   errorCode = dio->DoWrite(startPort, portCount, bufferForWriting);
	   if (errorCode == Success)
			return true;
/*

	   {
		   TThread::CurrentThread->Sleep(SleepTime);
		   //Read back DO ports’Status
		   errorCode = dio->DoRead(startPort, portCount, bufferForReading);
		   if (errorCode == Success)
		   {
			   DoValue = bufferForReading[0]+
						(bufferForReading[1] << 8)+
						(bufferForReading[2] << 16)+
						(bufferForReading[3] << 24);
			   if (DoValue == OutWord) return true;
			   else return false;
		   }
		   else return false;
	   }
*/
		else
			return false;
}
//------------------------------------------------------------------------------
bool Advantech1730::SetOutBit(byte bit)
{
	AdvMutex->Acquire();
	errorCode = dio->DoRead(startPort, portCount, bufferForReading);
	   DoValue = bufferForReading[0]+
				(bufferForReading[1] << 8)+
				(bufferForReading[2] << 16)+
				(bufferForReading[3] << 24);
		DoValue = DoValue | (1<<bit);
	bool p = SetOutWord(DoValue);
	AdvMutex->Release();
	return p;
}
//------------------------------------------------------------------------------
bool Advantech1730::ResetOutBit(byte bit)
{
	AdvMutex->Acquire();
	errorCode = dio->DoRead(startPort, portCount, bufferForReading);
	   DoValue = bufferForReading[0]+
				(bufferForReading[1] << 8)+
				(bufferForReading[2] << 16)+
				(bufferForReading[3] << 24);
		DoValue = DoValue & (~(1<<bit));
	bool p = SetOutWord(DoValue);
	AdvMutex->Release();
	return p;
}
//------------------------------------------------------------------------------
bool Advantech1730::BlinkOutBit(byte bit , int userTime)
{
	int blink = (userTime) ? (userTime) : (blinkTime);
	if (SetOutBit(bit))
	{
		for( DWORD tt=GetTickCount(); GetTickCount()-tt < blink; Sleep(10) )
			Application->ProcessMessages();

		return  ResetOutBit(bit);
	}
	else return false;
}
//------------------------------------------------------------------------------
bool Advantech1730::CheckOutWord()
{
	bool r;
	AdvMutex->Acquire();
	errorCode = dio->DoRead(startPort, portCount, bufferForReading);	//Read DO ports’status
	if (errorCode==Success)
	{
		DoValue=bufferForReading[0]+
			   (bufferForReading[1] << 8)+
			   (bufferForReading[2] << 16)+
			   (bufferForReading[3] << 24);
		r = true;
	}
	else
	{
		Application->MessageBoxW(L"Ошибка при чтении выходного слова с платы Advantech1730!",L"Ошибка!",MB_ICONERROR);
		r = false;
	}
	AdvMutex->Release();
	return r;
}
//------------------------------------------------------------------------------
bool Advantech1730::CheckOutBit(byte bit)
{
	if ( CheckOutWord() )
	{
		AdvMutex->Acquire();
		bool t;
		if (DoValue & (1<<bit))
			t = true;
		else
			t = false;
		AdvMutex->Release();
		return t;
	}
	else {
//		Application->MessageBoxW(L"Ошибка при чтении дискретного сигнала с платы Advantech1730!",L"Ошибка!",MB_ICONERROR);
		return false;
	}
}
//------------------------------------------------------------------------------
bool Advantech1730::CheckInWord()
{
	bool r;
	AdvMutex->Acquire();
	errorCode = dio->DiRead(startPort, portCount, bufferForReading);//Read DI ports’status
	if (errorCode==Success)
	{
		DiValue=bufferForReading[0]+
			   (bufferForReading[1] << 8)+
			   (bufferForReading[2] << 16)+
			   (bufferForReading[3] << 24);
		r = true;
	}
	else
	{
		Application->MessageBoxW(L"Ошибка при чтении входного слова с платы Advantech1730!",L"Ошибка!",MB_ICONERROR);
		r = false;
	}
	AdvMutex->Release();
	return r;

}
//------------------------------------------------------------------------------
bool Advantech1730::CheckInBit(byte bit)
{
	if ( CheckInWord() )
	{
		AdvMutex->Acquire();
		bool t;
		if (DiValue & (1<<bit))
			t = true;
		else
			t = false;
		AdvMutex->Release();
		return t;
	}
	else
	{
//		Application->MessageBoxW(L"Ошибка при чтении дискретного сигнала с платы Advantech1730!",L"Ошибка!",MB_ICONERROR);
		return false;
	}
}
//------------------------------------------------------------------------------
bool Advantech1730::CheckInBouncelessBit(byte bit, bool check_status)
{
	bool status;
	bool result;
	for (int i=0; i < bounceRepeat; i++)
	{
		result = ( CheckInBit(bit) == check_status );		// результат должен соответствовать ожиданиям (check_status)
		status = ! (check_status ^ result) ;				// операция эквиваленции !XOR
		if ( !result )
			break;
		TThread::CurrentThread->Sleep( bouncePeriod );
		Application->ProcessMessages();
	}
	return status;
}
//------------------------------------------------------------------------------
bool Advantech1730::WaitForInBit(byte bit, unsigned int DelayTimeMSec, TThread *thread)
{
	DWORD startTime = GetTickCount();
	while (true)
	{
		if ( CheckInBit( bit ) )
			return true;
		if ( thread != NULL && thread->CheckTerminated() )
			break;
		if ( DelayTimeMSec && ((GetTickCount() - startTime) > DelayTimeMSec) )
			break;
		TThread::CurrentThread->Sleep(10);
		Application->ProcessMessages();
	}
	return false;

}
//------------------------------------------------------------------------------
bool Advantech1730::WaitForDropInBit(byte bit, unsigned int DelayTimeMSec, TThread *thread)
{
	DWORD startTime = GetTickCount();
	while (true)
	{
		if ( ! CheckInBit( bit ) )
			return true;
		if ( thread != NULL && thread->CheckTerminated() )
			break;
		if ( DelayTimeMSec && ((GetTickCount() - startTime) > DelayTimeMSec) )
			break;
		TThread::CurrentThread->Sleep(10);
		Application->ProcessMessages();
	}
	return false;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//------оператор присванивания для структуры Сигнал-----------------------------------------
/*
Signal Signal::operator = (int code)
{
	Signal ss;
	ss.card = code / 100;
	ss.channel = code % 100;
	return ss;
}
*/
//------------------------------------------------------------------------------
/*
Signal Signal::operator = (Signal s)
{
	Signal ss;
	ss.card = s.card;
	ss.channel = s.channel;
	return ss;
}
*/
//------------------------------------------------------------------------------
/*
Signal& Signal::operator = (Signal& s)
{
	card = s.card;
	channel = s.channel;
	return *this;
}
*/
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//-------------Методы общего класса AllSignals----------------------------------
//------------------------------------------------------------------------------
AllSignals::AllSignals()
{
//	AdvCards[0] = new Advantech1730();
//	AdvCards[1] = new Advantech1730();
	AdvCards[0] = new Advantech1730_virtual();
	AdvCards[1] = new Advantech1730_virtual();
	inmap = new map <String,Signal>;
	outmap = new map <String,Signal>;
}
//------------------------------------------------------------------------------
// очищаем память
AllSignals::~AllSignals()
{
	delete AdvCards[0];
	delete AdvCards[1];
	delete inmap;
	delete outmap;
}
//------------------------------------------------------------------------------
// записывает в ini файл все реальные платы системы
bool __stdcall AllSignals::EnumDeviceCallBack(BDaqEnumerator::EnumDeviceArgs *args, void *ctx)
{
	ErrorCode ret = Success;
	BDaqDio *dio = NULL;
	HANDLE dioStopEventHandle;
	ret = args->Device->GetModule(0, dio);
	if (ret == Success && (System::WideCharToString(args->Description).SubString(0,4))!=L"Demo")
	{
		TIniFile *ini = new TIniFile ( Globals::IniFileName );
		devarg[ cards_exist++ ] = *args;
		ini->WriteInteger("CardNames","ACard"+IntToStr(cards_exist-1), args->DeviceNumber);
		delete ini;
	}
	return true;
}
//------------------------------------------------------------------------------
// находит все платы (пока 2) в системе
void AllSignals::FindCards()
{
	TIniFile *ini = new TIniFile ( Globals::IniFileName );
	ini->DeleteKey("CardNames","ACard0");
	ini->DeleteKey("CardNames","ACard1");
	delete ini;

	void  *p=NULL;
	en = Automation::BDaq::BDaqEnumerator();
	en.EnumerateDevices( DaqDio, (BDaqEnumerator::EnumDeviceFunc) * EnumDeviceCallBack, p);
}
//------------------------------------------------------------------------------
// инициализирует обе платы Advantech1730, возвращает число открытых плат
short AllSignals::InitializeCards(bool ReadOnly)
{
	LoadSignalNames();
	cards_inited = 0;
	TIniFile *ini = new TIniFile ( Globals::IniFileName );

	for (int i = 0; i < Math::Min(cards_exist, cards_needed); i++)
	{
		int devnum = ini->ReadInteger("CardNames","ACard"+IntToStr(i),-1);
		if ( AdvCards[i]->Init(devnum, ReadOnly) )
			cards_inited++;
	}
	delete ini;

	if (cards_inited != Math::Min(cards_exist, cards_needed))
		Application->MessageBoxW(L"Не все платы Advantech иниицализированы!",L"",MB_ICONERROR);

	return cards_inited;
}
//------------------------------------------------------------------------------
// закрывает обе платы Advantech1730
bool AllSignals::CloseCards()
{
	bool ok_closed = true;
 	for (int i = 0; i < cards_inited; i++)
	{
		AdvCards[i]->SetOutWord(0);
		if ( !AdvCards[i]->Close() )
			ok_closed = false;
	}
	return ok_closed;
}
//------------------------------------------------------------------------------
// загружает имена цифровых сигналов из ini файла
void AllSignals::LoadSignalNames()
{
	TIniFile *ini = new TIniFile ( Globals::SignalsFileName );   // c:\\Project\\DEFECTOSCOPE\\Debug\\Win32\\
	TStringList *sections = new TStringList();		// список всех разделов
	TStringList *keys = new TStringList();			// список всех ключей внутри раздела
	String MapKey;			// ключ для карты сигналов
	String Sect;			// имя секции с сигналами в файлами
	String Key;				// имя ключа внутри раздела
	int cardnum;			// номер платы
	int chan;				// номер канала

	ini->ReadSections(sections);
	if ( sections->Count == 0)
		Application->MessageBoxW(L"Файл с именами цифровых сигналов платы Advantech1730 не найден.\n Программа работает некорректно.\n Ошибка 002",L"Ошибка!",MB_ICONERROR);
	else
	{
// читаем имена сигналов из ini файла, и по имени секции вычисляем все параметры
		for (int i = 0; i < sections->Count; i++)
		{
			Sect = sections->Strings[i];
			if ( Sect.SubString(1,5) == "ACard" )
			{
				ini -> ReadSection( Sect, keys );
				for (int j = 0; j < keys->Count ; j++)
				{
					Key = keys->Strings[j];
					cardnum = Sect . SubString(6,1) . ToInt();
					chan = ( Sect . SubString(12,1) == "T" ) ? ( Key.ToInt() + 16 ) : ( Key.ToInt() );
					MapKey = ini->ReadString( Sect , Key.ToInt() , "").LowerCase();
					pair <String, Signal> rr;
					rr.second.card = cardnum;
					rr.second.channel = chan;
					rr.second.inverted = ( MapKey.SubString(1,1) == L"!" );	// проверяем, инвертированный сигнал или нет
					MapKey = ( MapKey.SubString(1,1) != L"!" ) ? (MapKey) : ( MapKey.SubString(2,MapKey.Length()) );
					rr.first = MapKey;
					if ( Sect . SubString(8,3) == "IN_" )
					{
						inmap->insert(rr);
					}
					else if ( Sect . SubString(8,3) == "OUT" )
					{
						outmap->insert(rr);
					}
				}
			}
		}
	}
	// считаем, сколько плат нужно для работы
	DynamicArray<short> c,r;
	c.set_length(sections->Count);
	for (int i = 0; i < sections->Count; i++)
		c[i] = StrToInt( sections->Strings[i].SubString(6,1));
	r.set_length(1);
	r[0] = c[0];
	bool nw=true;
	for (int i = 1; i < c.Length; i++)
	{
		nw = true;
		for (int j = 0; j < r.Length; j++)
		{
			if (c[i] == r[j])
			{
				nw = false;
				break;
			}
		}
		if (nw)
		{
			r.Length++;
			r[r.High] = c[i];
		}
	}

	cards_needed = r.Length;

	r.~DynamicArray();
	c.~DynamicArray();
	sections->~TStringList();
	delete ini;
}
//------------------------------------------------------------------------------
// находит нужный сигнал по имени
Signal AllSignals::FindSignal(String SignalName, map<String,Signal> *mapname )
{
	SignalName = SignalName.LowerCase();
	map <String,Signal>::iterator it;
	for (it = mapname->begin(); it != mapname->end(); it++)
		if ( it->first == SignalName )
			return it->second;
// если не удалось найти сигнал
	String str = L"Программист! В твоей программе неправильно назван сигнал \"" + SignalName + "\"";
	Application->MessageBoxW(str.w_str(),L"ОШИБКА!",MB_ICONERROR);
	Signal shit;
	shit.card = 0;
	shit.channel = 0;
	shit.inverted = false;
	return shit ;
}
//------------------------------------------------------------------------------
// возвращает true , если сигнал есть в карте сигналов (файле IO_Management.ini)
bool AllSignals::SignalExists(String SignalName)
{
	SignalName = SignalName.LowerCase();
	map <String,Signal>::iterator it;
	for (it = inmap->begin(); it != inmap->end(); it++)
		if ( it->first == SignalName )
			return true;
	for (it = outmap->begin(); it != outmap->end(); it++)
		if ( it->first == SignalName )
			return true;
	return false;
}
//------------------------------------------------------------------------------
// выставить выходной сигнал по имени сигнала
bool AllSignals::SetOutSignal(String SignalName)
{
	Signal s = FindSignal(SignalName, outmap);
	return AdvCards[ s.card ] -> SetOutBit( s.channel );
}
//------------------------------------------------------------------------------
// сбросить выходной сигнал по имени сигнала
bool AllSignals::ResetOutSignal(String SignalName)
{
	Signal s = FindSignal(SignalName, outmap);
	return AdvCards[ s.card ] -> ResetOutBit( s.channel );
}
//------------------------------------------------------------------------------
// импульсно выдать выходной сигнал по имени сигнала
bool AllSignals::BlinkOutSignal(String SignalName , int userTime)
{
	Signal s = FindSignal(SignalName, outmap);
	return AdvCards[ s.card ] -> BlinkOutBit( s.channel , userTime);
}
//------------------------------------------------------------------------------
// проверяет наличие ВЫХОДНОГО сигнала и возвращает его статус
bool AllSignals::CheckOutSignal(String SignalName)
{
	Signal s = FindSignal(SignalName, outmap);
	return AdvCards[ s.card ] -> CheckOutBit( s.channel );
}
//------------------------------------------------------------------------------
// проверить наличие входного сигнала по имени сигнала
bool AllSignals::CheckInSignal(String SignalName)
{
	Signal s = FindSignal(SignalName, inmap);
	return AdvCards[ s.card ] -> CheckInBit( s.channel ) ^ s.inverted;		// XOR с inverted - чтобы учесть инвертированность
}
//------------------------------------------------------------------------------
// проверить наличие входного сигнала по имени сигнала c учетом дребезга
bool AllSignals::CheckInBouncelessSignal(String SignalName, bool check_status)
{
	Signal s = FindSignal(SignalName, inmap);
	if ( !s.inverted )
		return AdvCards[ s.card ] -> CheckInBouncelessBit( s.channel, check_status );
	else
		return ! AdvCards[ s.card ] -> CheckInBouncelessBit (s.channel, !check_status );	// для инвертированного сигнала - две инверсии в функции
}
//------------------------------------------------------------------------------
// ждет входного сигнала определенное время и выпадает из цикла, если тред завершили извне, время задается в DelayTimeMSec  в мс
bool AllSignals::WaitForInSignal(String SignalName, int DelayTimeMSec, TThread *thread)
{
	Signal s = FindSignal(SignalName, inmap);
	if ( !s.inverted )
		return AdvCards[ s.card ] -> WaitForInBit( s.channel, DelayTimeMSec, thread );
	else
		return AdvCards[ s.card ] -> WaitForDropInBit( s.channel, DelayTimeMSec, thread );
}
//------------------------------------------------------------------------------
// ждет пропадания входного сигнала определенное время и выпадает из цикла, если тред завершили извне, время задается в DelayTimeMSec  в мс
bool AllSignals::WaitForDropInSignal(String SignalName, int DelayTimeMSec, TThread *thread)
{
	Signal s = FindSignal(SignalName, inmap);
	if ( !s.inverted )
		return AdvCards[ s.card ] -> WaitForDropInBit( s.channel, DelayTimeMSec, thread );
	else
		return AdvCards[ s.card ] -> WaitForInBit( s.channel, DelayTimeMSec, thread );
}
//------------------------------------------------------------------------------
// возвращает входное слово определенной платы
unsigned int AllSignals::GetInWord(int adv_card)
{
	if ( AdvCards[adv_card]->CheckInWord() )
		return AdvCards[adv_card]->DiValue;
	else
		return 0;
}
//------------------------------------------------------------------------------
// возвращает выходное слово определенной платы
unsigned int AllSignals::GetOutWord(int adv_card)
{
	if ( AdvCards[adv_card]->CheckOutWord() )
		return AdvCards[adv_card]->DoValue;
	else
		return 0;
}
//------------------------------------------------------------------------------

