//---------------------------------------------------------------------------

#ifndef WorkModeH
#define WorkModeH
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
#include "IniFiles.hpp"

#include "Result.h"
#include "Global.h"
#include "SMS.h"

//! @brief Класс отвечающий за рабочий цикл трубы
//! Отслеживает сигналы и управляет механизмами по мере прохождения трубы через установку
//! Все действия выполняются последовательно
class WorkThreadClass : public TThread
{

protected:

	void __fastcall Execute();
	//! указатель на статусбар, показывающий течение процесса
	TStatusBar *St1;
	//! используем ли толщинометрию
	bool Thick;
	//! используем ли продольный
	bool Linear;
	//! нужно ли готовить к началу работы
	bool Prepare;
	//! текст для надписи в статусбаре в панели 1
	String stext1;
	//! текст для надписи в статусбаре в панели 2
	String stext2;
	//! длина поперечного модуля , мм
	double cross_len;
	//! длина продольного модуля , мм
	double linear_len;
	//! расстояние стойка 1 - стойка 4
	double st_len;
	//! длина 1 зоны , мм
	int zone_len;

public:
//! @brief Конструктор
//! @param CreateSuspended создавать поток замороженным
//! @param *St указатель на статусбар, показывающий течение процесса
//! @param _Thick используем ли толщинометрию
//! @param _Linear используем ли продольный)
//! @param alth адрес аварийного треда
//! @param isPrepare первый цикл (нужно ли готовить к работе систему)

__fastcall WorkThreadClass(bool CreateSuspended,
					TStatusBar *St,
					bool _Linear,
					bool IsPrepare);
	//! выставляем исходное положение модулей перед работой
	UnicodeString PrepareForWork( );
	//! режим РАБОТЫ!!!
	bool WorkMode( );
	//! обновляет компоненты главной формы, обрабатывает события
	void __fastcall UpdateMainForm();
	//! вместо Return Value, которого нету
	bool cool;
	bool transit;
	//! @brief времена для расчетов трубы.
	//! времена поперечного
	DWORD cross_t1, cross_t2, cross_t3;
	//! времена продольного
	DWORD lin_t1, lin_t2, lin_t3;
	//! времена стоек 1 и 4
	DWORD st1 , st4 , st1_out;


};

//---------------------------------------------------------------------------
#endif
