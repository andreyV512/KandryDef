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

//! @brief ����� ���������� �� ������� ���� �����
//! ����������� ������� � ��������� ����������� �� ���� ����������� ����� ����� ���������
//! ��� �������� ����������� ���������������
class WorkThreadClass : public TThread
{

protected:

	void __fastcall Execute();
	//! ��������� �� ���������, ������������ ������� ��������
	TStatusBar *St1;
	//! ���������� �� �������������
	bool Thick;
	//! ���������� �� ����������
	bool Linear;
	//! ����� �� �������� � ������ ������
	bool Prepare;
	//! ����� ��� ������� � ���������� � ������ 1
	String stext1;
	//! ����� ��� ������� � ���������� � ������ 2
	String stext2;
	//! ����� ����������� ������ , ��
	double cross_len;
	//! ����� ����������� ������ , ��
	double linear_len;
	//! ���������� ������ 1 - ������ 4
	double st_len;
	//! ����� 1 ���� , ��
	int zone_len;

public:
//! @brief �����������
//! @param CreateSuspended ��������� ����� ������������
//! @param *St ��������� �� ���������, ������������ ������� ��������
//! @param _Thick ���������� �� �������������
//! @param _Linear ���������� �� ����������)
//! @param alth ����� ���������� �����
//! @param isPrepare ������ ���� (����� �� �������� � ������ �������)

__fastcall WorkThreadClass(bool CreateSuspended,
					TStatusBar *St,
					bool _Linear,
					bool IsPrepare);
	//! ���������� �������� ��������� ������� ����� �������
	UnicodeString PrepareForWork( );
	//! ����� ������!!!
	bool WorkMode( );
	//! ��������� ���������� ������� �����, ������������ �������
	void __fastcall UpdateMainForm();
	//! ������ Return Value, �������� ����
	bool cool;
	bool transit;
	//! @brief ������� ��� �������� �����.
	//! ������� �����������
	DWORD cross_t1, cross_t2, cross_t3;
	//! ������� �����������
	DWORD lin_t1, lin_t2, lin_t3;
	//! ������� ����� 1 � 4
	DWORD st1 , st4 , st1_out;


};

//---------------------------------------------------------------------------
#endif
