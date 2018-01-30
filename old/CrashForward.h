//---------------------------------------------------------------------------
#ifndef CrashForwardH
#define CrashForwardH
//---------------------------------------------------------------------------

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "Chart.hpp"
#include "TeEngine.hpp"
#include "TeeProcs.hpp"
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include "Series.hpp"
#include <ComCtrls.hpp>
#include <windows.h>
#include "IniFiles.hpp"
#include <ImgList.hpp>

#include "Protocol.h"
#include "AlarmThread.h"


//! @brief Режим выгона, проверяет все сигналы и делает аварийный выгон застрявшей трубы.
//!	Режим Транзит - прогон трубы без модулей
//! Режим выгон/транзит не используются т.к. все перевели на контроллеры (с начала 2013г.)
class CrashForwardThread : public TThread
{
protected:
	//! Запускает поток
	void __fastcall Execute();
	//! Указатель на статусбар, показывающий течение процесса
	TStatusBar *StatusBar;
	//! Текст для надписи в статусбаре в панели 1
	String stext1;
	//! Текст для надписи в статусбаре в панели 2
	String stext2;

public:
	//! @brief Конструктор.
	//! @param CreateSuspended Запускать поток после работы конструктора или нет
	//! @param TStatusBar St Ссылка на статусбар, в который будет транслироваться состояние потока
	//! @param AlarmThreadClass alth Ссылка на поток аварийных ситуаций (не используется)
	__fastcall CrashForwardThread(bool CreateSuspended , TStatusBar *St , AlarmThreadClass &_alth);
	//! Обновляет компоненты главной формы, обрабатывает события
	void __fastcall UpdateMainForm();
	//! Поле вместо Return Value, которое тяжело получить в некоторых ситуациях
	bool cool;
	//! @brief Включен ли режим Транзит
	//! @li true Работает режим Транзит
	//! @li false Работает режим Выгон
	bool isTransit;

private:
	//! Режим Транзит - для прогона трубы без модулей
	bool Transit();
	//! Режим Выгон, основная функция
	bool CrashForwardMode();

};



#endif
