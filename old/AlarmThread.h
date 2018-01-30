//---------------------------------------------------------------------------

#ifndef AlarmThreadH
#define AlarmThreadH
//---------------------------------------------------------------------------
#include <Classes.hpp>
//---------------------------------------------------------------------------
#include "AdvantechSignals.h"


//! Класс циклической проверки аварийных ситуаций во время работы
class AlarmThreadClass : public TThread
{
private:
	bool CrossCycle;
	bool LinearCycle;
	bool InvA;

protected:
	//! Запуск треда на проверку аварий во время работы
	void __fastcall Execute();
	//! Мюьтекс проверки сигнала Контроль
	TMutex *cycleM;
	//! Проверка сигнала Контроль
	bool control;

public:
	//! Конструктор класса
	__fastcall AlarmThreadClass(bool CreateSuspended);
	//! Обновляем главную форму
	void __fastcall UpdateCaption();
	//! Была ли авария
	bool is_alarm;
	//! Причина аварии
	String reason;
	//! Начинаем проверять сигнал Попер цикл
	void SetCrossCycle();
	//! Закончили проверять сигнал Прод цикл
	void ResetCrossCycle();
	//! Начинаем проверять сигнал Попер цикл
	void SetLinearCycle();
	//! Закончили проверять сигнал Прод цикл
	void ResetLinearCycle();
	void SetA();
	void ResetA();
	//! Какие сигналы стоек проверять
	void SetStand(String st1, String st2 = "");
	//! Больше не проверять сигналы стоек
	void ResetStand();

};
//---------------------------------------------------------------------------
#endif
