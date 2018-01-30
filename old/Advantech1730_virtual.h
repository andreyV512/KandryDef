//---------------------------------------------------------------------------
#ifndef Advantech1730_virtualH
#define Advantech1730_virtualH
#include <Classes.hpp>
//---------------------------------------------------------------------------
class Advantech1730_virtual
{
public:
	//! Конструктор, задаем всё
	Advantech1730_virtual()
	{
		DoValue = 0;
	};
	//! Выходное слово (32 бита)
	unsigned int	DoValue;
	//! Входное слово (32 бита)
	unsigned int	DiValue;
	//! Инициализация платы, возвращает true в случае успеха
	bool Init(long DevNum, bool ReadOnly){return(false);};
	//! Закрытие платы
	bool Close(){return(false);};
	//! Выставляет выходное слово, возвращает true в случае успеха
	bool SetOutWord(unsigned int OutWord){return(false);};
	//! Выставляет выходной бит, возвращает true в случае успеха
	bool SetOutBit(byte bit){return(false);};
	//! Сбрасывает выходной бит, возвращает true в случае успеха
	bool ResetOutBit(byte bit){return(false);};
	//! @brief Кратковременно выставляет и сбрасывает выходной бит, возвращает true в случае успеха
	//! @param bit Адрес бита, которым необходимо мигнуть
	//! @param userTime Время мигания сигнала в мс, если не задать, то будет держать blinkTime
	bool BlinkOutBit(byte bit , int userTime=0){return(false);};
	//! Считывает ВЫХОДНОЕ слово и записывает его в DoValue, возвращает true в случае успеха
	bool CheckOutWord(){return(false);};
	//! Проверяет наличие ВЫХОДНОГО бита и возвращает НАЛИЧИЕ бита (установлен выход или нет)
	bool CheckOutBit(byte bit){return(false);};
	//! Считывает входное слово и записывает его в DiValue, возвращает true в случае успеха
	bool CheckInWord(){return(false);};
	//! Проверяет наличие входного бита, возвращает НАЛИЧИЕ бита (есть/нет напряжение на входе)
	bool CheckInBit(byte bit){return(false);};
	//! @brief Проверяет входной бит с учетом дребезга, возвращает НАЛИЧИЕ бита.
	//! Функция несколько раз с задержкой опрашивает определенный бит.
	//! Если встречается состояние, не соответствующее запрошенному (check_status),
	//! функция прекращает работу и возвращает наличие сигнала.
	//! @param bit Входной бит, состояние которого будем проверять по циклу
	//! @param check_status Состояние, которое мы проверяем. True - бит выставлен, false - сброшен
	//! @retval Возвращает наличие бита, независимо от того, что мы проверяли
	bool CheckInBouncelessBit(byte bit, bool check_status){return(false);};
	//! @brief Ждет входного бита определенное время и проверяет состояние треда, возвращает НАЛИЧИЕ бита
	//! @param bit Адрес бита, появления которого мы ждем
	//! @param DelayTimeMSec Время, которое мы будем ждать бит (если задать 0, будем ждать бесконечно)
	//! @param thread Поток, в случае остановки которого функция возвращает false
	//! @retval True, если бит появился. False, если закончилось время или сбросился поток
	bool WaitForInBit(byte bit, unsigned int DelayTimeMSec, TThread *thread){return(false);};
	//! @brief Ждет сброса входного бита определенное время и проверяет состояние треда, возвращает НАЛИЧИЕ бита
	//! @param bit Адрес бита, сброса которого мы ждем
	//! @param DelayTimeMSec Время, которое мы будем ждать сброс бита (если задать 0, будем ждать бесконечно)
	//! @param thread Поток, в случае остановки которого функция возвращает false
	//! @retval true, если бит сброшен. False, если закончилось время или сбросился поток
	bool WaitForDropInBit(byte bit, unsigned int DelayTimeMSec, TThread *thread){return(false);};
};
#endif

