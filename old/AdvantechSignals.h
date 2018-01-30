//---------------------------------------------------------------------------
#ifndef AdvantechSignalsH
#define AdvantechSignalsH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "TeEngine.hpp"
#include "TeeProcs.hpp"
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include "Protocol.h"
#include <map.h>

#include "c:/Advantech/BioDAQ/Inc/BDaqCL.h"
#include "Advantech1730_virtual.h"
using namespace Automation::BDaq;


//! Класс работы с платой дискретного ввода-вывода Advantech PCI1730U
class Advantech1730   {
private:
	//! handle to the device
	BDaqDevice *device;
	//! error code (0=Success)
	ErrorCode errorCode;
	//! handle to input output
	BDaqDio *dio;
	TMutex *AdvMutex;

	//! Начальный порт, по умолчанию = 0 (внутренние дела платы)
	long  startPort;
	//! Число портов, по умолчанию = 4 (внутренние дела платы)
	long  portCount;
	//! Буфер, содержащий все выходные сигналы по 4 портам
	BYTE  bufferForWriting[4];
	//! Буфер, содержащий все входные сигналы по 4 портам
	BYTE  bufferForReading[4];
	//! Время, после которого проверяем, выставился ли выходной сигнал
	short SleepTime;
	//! Время задержки выходного сигнала в команде BlinkOut
	short blinkTime;
	//! Время дребезга - интервал, через который будет проверяться состояние сигнала
	short bouncePeriod;
	//! Кол-во повторений дребезга
	short bounceRepeat;

public:
	//! Конструктор, задаем всё
	Advantech1730();
	//! Деструктор, разрушаем все
	~Advantech1730();
	//! Выходное слово (32 бита)
	unsigned int	DoValue;
	//! Входное слово (32 бита)
	unsigned int	DiValue;

	//! Инициализация платы, возвращает true в случае успеха
	bool Init(long DevNum, bool ReadOnly);
	//! Закрытие платы
	bool Close();
	//! Выставляет выходное слово, возвращает true в случае успеха
	bool SetOutWord(unsigned int OutWord);
	//! Выставляет выходной бит, возвращает true в случае успеха
	bool SetOutBit(byte bit);
	//! Сбрасывает выходной бит, возвращает true в случае успеха
	bool ResetOutBit(byte bit);
	//! @brief Кратковременно выставляет и сбрасывает выходной бит, возвращает true в случае успеха
	//! @param bit Адрес бита, которым необходимо мигнуть
	//! @param userTime Время мигания сигнала в мс, если не задать, то будет держать blinkTime
	bool BlinkOutBit(byte bit , int userTime=0);
	//! Считывает ВЫХОДНОЕ слово и записывает его в DoValue, возвращает true в случае успеха
	bool CheckOutWord();
	//! Проверяет наличие ВЫХОДНОГО бита и возвращает НАЛИЧИЕ бита (установлен выход или нет)
	bool CheckOutBit(byte bit);
	//! Считывает входное слово и записывает его в DiValue, возвращает true в случае успеха
	bool CheckInWord();
	//! Проверяет наличие входного бита, возвращает НАЛИЧИЕ бита (есть/нет напряжение на входе)
	bool CheckInBit(byte bit);
	//! @brief Проверяет входной бит с учетом дребезга, возвращает НАЛИЧИЕ бита.
	//! Функция несколько раз с задержкой опрашивает определенный бит.
	//! Если встречается состояние, не соответствующее запрошенному (check_status),
	//! функция прекращает работу и возвращает наличие сигнала.
	//! @param bit Входной бит, состояние которого будем проверять по циклу
	//! @param check_status Состояние, которое мы проверяем. True - бит выставлен, false - сброшен
	//! @retval Возвращает наличие бита, независимо от того, что мы проверяли
	bool CheckInBouncelessBit(byte bit, bool check_status);
	//! @brief Ждет входного бита определенное время и проверяет состояние треда, возвращает НАЛИЧИЕ бита
	//! @param bit Адрес бита, появления которого мы ждем
	//! @param DelayTimeMSec Время, которое мы будем ждать бит (если задать 0, будем ждать бесконечно)
	//! @param thread Поток, в случае остановки которого функция возвращает false
	//! @retval True, если бит появился. False, если закончилось время или сбросился поток
	bool WaitForInBit(byte bit, unsigned int DelayTimeMSec, TThread *thread);
	//! @brief Ждет сброса входного бита определенное время и проверяет состояние треда, возвращает НАЛИЧИЕ бита
	//! @param bit Адрес бита, сброса которого мы ждем
	//! @param DelayTimeMSec Время, которое мы будем ждать сброс бита (если задать 0, будем ждать бесконечно)
	//! @param thread Поток, в случае остановки которого функция возвращает false
	//! @retval true, если бит сброшен. False, если закончилось время или сбросился поток
	bool WaitForDropInBit(byte bit, unsigned int DelayTimeMSec, TThread *thread);


};

//! Структура Сигнал - описывает параметры одного дискретного сигнала на плату Advantech
struct Signal
{
	//! Номер платы
	byte card;
	//! Номер канала
	byte channel;
	//! Инвертирован ли сигнал
	bool inverted;
//	Signal operator = (int code);
//	Signal operator = (Signal s);
//	Signal& operator = (Signal& );
};


//! Класс работы со всеми цифровыми сигналами, независимо от платы, на которую посажен сигнал
class AllSignals {
private:
	static bool __stdcall EnumDeviceCallBack(BDaqEnumerator::EnumDeviceArgs *args, void *ctx);
	static BDaqEnumerator::EnumDeviceArgs devarg[7];
	//!Енумератор для подсчета существующих плат
	BDaqEnumerator en;
	//! Грузим имена цифровых сигналов из ini файла
	void LoadSignalNames();

public:
	//! 2 указателя на платы Advantech1730
	Advantech1730_virtual *AdvCards[2];
//	Advantech1730 *AdvCards[2];
	//! Конструктор, резервируем память под указатели на платы
	AllSignals();
	//! Деструктор, освобождаем память
	~AllSignals();

	//! Кол-во плат, существующих в системе (берется из EnumDeviceCallBack)
	static byte cards_exist;
	//! Кол-во плат, проинициализированных (не занятых другими приложениями)
	byte cards_inited;
	//! Кол-во плат, которые нужны для работы (берется из файла IO_management.ini)
	byte cards_needed;

	//! Карта входов, где ключ - имя сигнала, данные - номер платы и канала
	map <String, Signal> *inmap;
	//! Карта выходов, где ключ - имя сигнала, данные - номер платы и канала
	map <String, Signal> *outmap;

	//! Ищет все доступные платы Advantech на компьютере
	void FindCards();
	//! @brief Инициализация всех плат , возвращает число инициализированных плат
	//! @param ReadOnly Открывать в режиме "Только для чтения" (нельзя выставлять выходы)
	//! @retval Возвращает число успешно проинициализированных плат
	short InitializeCards(bool ReadOnly);
	//! Закрытие всех плат Advantech
	bool CloseCards();
	//! @brief Находит указанный сигнал на платах
	//! @param SignalName имя сигнала, адрес которого нужно найти
	//! @param mapname имя карты сигналов (входная или выходная), которой принадлежит сигнал
	Signal FindSignal(String SignalName, map<String,Signal> *mapname );
	//! Возвращает true , если сигнал есть в карте сигналов (файле IO_Management.ini)
	bool SignalExists(String SignalName);
	//! Выставляет выходной сигнал, возвращает true в случае успеха
	bool SetOutSignal(String SignalName);
	//! Сбрасывает выходной сигнал, возвращает true в случае успеха
	bool ResetOutSignal(String SignalName);
	//! @brief Выставляет, а затем после задержки сбрасывает выходной сигнал, возвращает true в случае успеха
	//! @param SignalName Имя сигнала, которым необходимо мигнуть
	//! @param userTime Время мигания сигнала в мс, если не задать, то будет держать blinkTime
	bool BlinkOutSignal(String SignalName , int userTime = 0);
	//! Проверяет, есть ли определенный ВЫХОДНОЙ сигнал, возвращает статус сигнала
	bool CheckOutSignal(String SignalName);
	//! Проверяет, есть ли определенный ВХОДНОЙ сигнал, возвращает стутус сигнала
	bool CheckInSignal(String SignalName);
	//! @brief Проверяет наличие входного сигнала c учетом дребезга .
	//! Функция несколько раз с задержкой опрашивает определенный входной сигнал.
	//! Если встречается состояние, не соответствующее запрошенному (check_status),
	//! функция прекращает работу и возвращает наличие сигнала
	//! @param SignalName Имя сигнала, состояние которого будем проверять по циклу
	//! @param check_status Состояние, которое мы проверяем. True - бит выставлен, false - сброшен
	//! @retval Возвращает наличие бита, независимо от того, что мы проверяли
	bool CheckInBouncelessSignal(String SignalName, bool check_status);

	//! @brief Ждет входного сигнала определенное время и проверяет состояние треда, возвращает НАЛИЧИЕ бита
	//! @param SignalName Имя входного сигнала, появления которого мы ждем
	//! @param DelayTimeMSec Время, которое мы будем ждать сигнал (если задать 0, будем ждать бесконечно)
	//! @param thread Поток, в случае остановки которого функция возвращает false
	//! @retval true, если сигнал появился. False, если закончилось время или сбросился поток
	bool WaitForInSignal(String SignalName, int DelayTimeMSec, TThread *thread = NULL);
	//! @brief Ждет сброса входного сигнала определенное время и проверяет состояние треда, возвращает НАЛИЧИЕ бита
	//! @param SignalName Имя входного сигнала, сброса которого мы ждем
	//! @param DelayTimeMSec Время, которое мы будем ждать сброс сигнала (если задать 0, будем ждать бесконечно)
	//! @param thread Поток, в случае остановки которого функция возвращает false
	//! @retval true, если сигнал сброшен. False, если закончилось время или сбросился поток
	bool WaitForDropInSignal(String SignalName, int DelayTimeMSec, TThread *thread = NULL);
	//! Возвращает входное слово (32 бита) нужной платы
	unsigned int GetInWord(int adv_card);
	//! Возвращает выходное слово (32 бита) нужной платы
	unsigned int GetOutWord(int adv_card);
};


	byte AllSignals::cards_exist = 0;
	BDaqEnumerator::EnumDeviceArgs AllSignals::devarg[7];
//---------------------------------------------------------------------------
//! глобальный экземпляр, управляющий цифровыми сигналами из любой точки программы.
extern AllSignals *digital;
//---------------------------------------------------------------------------
#endif
