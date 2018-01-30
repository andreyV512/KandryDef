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


//! @brief ����� ������, ��������� ��� ������� � ������ ��������� ����� ���������� �����.
//!	����� ������� - ������ ����� ��� �������
//! ����� �����/������� �� ������������ �.�. ��� �������� �� ����������� (� ������ 2013�.)
class CrashForwardThread : public TThread
{
protected:
	//! ��������� �����
	void __fastcall Execute();
	//! ��������� �� ���������, ������������ ������� ��������
	TStatusBar *StatusBar;
	//! ����� ��� ������� � ���������� � ������ 1
	String stext1;
	//! ����� ��� ������� � ���������� � ������ 2
	String stext2;

public:
	//! @brief �����������.
	//! @param CreateSuspended ��������� ����� ����� ������ ������������ ��� ���
	//! @param TStatusBar St ������ �� ���������, � ������� ����� ��������������� ��������� ������
	//! @param AlarmThreadClass alth ������ �� ����� ��������� �������� (�� ������������)
	__fastcall CrashForwardThread(bool CreateSuspended , TStatusBar *St , AlarmThreadClass &_alth);
	//! ��������� ���������� ������� �����, ������������ �������
	void __fastcall UpdateMainForm();
	//! ���� ������ Return Value, ������� ������ �������� � ��������� ���������
	bool cool;
	//! @brief ������� �� ����� �������
	//! @li true �������� ����� �������
	//! @li false �������� ����� �����
	bool isTransit;

private:
	//! ����� ������� - ��� ������� ����� ��� �������
	bool Transit();
	//! ����� �����, �������� �������
	bool CrashForwardMode();

};



#endif
