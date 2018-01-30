//---------------------------------------------------------------------------
#ifndef ReturnModeH
#define ReturnModeH
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


//! Режим возврата, проверяет все сигналы и возвращает трубу на начало линии
//! Данный режим не используется, все перевели на контролеры (с начала 2013г.)
class ReturnThread : public TThread
{
protected:
	//! Запуск потока Возврата
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
	__fastcall ReturnThread(bool CreateSuspended , TStatusBar *St, AlarmThreadClass &_alth);
	//! Обновляет компоненты главной формы, обрабатывает события
	void __fastcall UpdateMainForm();
	//! Режим возврата, основная функция
	bool ReturnMode();
	//! Вместо Return Value, доступ к которому бывает ограничен
	bool cool;
};




#endif
