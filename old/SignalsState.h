//---------------------------------------------------------------------------

#ifndef SignalsStateH
#define SignalsStateH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "IniFiles.hpp"
#include <vcl.h>


#include "AdvantechSignals.h"
#include <ExtCtrls.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------
class TSignalsStateForm : public TForm
{
__published:	// IDE-managed Components
	TTimer *InOutTimer;
	TLabeledEdit *eCols;
	TPopupMenu *MyPopupMenu;
	TMenuItem *nCols;
	TMenuItem *nExit;
	TMenuItem *nShowChannel;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall InOutTimerTimer(TObject *Sender);
	void __fastcall FormKeyPress(TObject *Sender, wchar_t &Key);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall eColsChange(TObject *Sender);
	void __fastcall nExitClick(TObject *Sender);
	void __fastcall nShowChannelClick(TObject *Sender);
private:	// User declarations

	map <String,Signal> *inmap;		// ����� ������ �� AdvantechSignals.h
	map <String,Signal> *outmap;	// ����� ������� �� AdvantechSignals.h

//	TPanel *in[64];			// ������ �������, ������������ ��������� ������
//	TPanel *out[64];		// ������ �������, ������������ ��������� �������
	TPanel *in[2][32];		// ������ �������, ������������ ��������� ������
	TPanel *out[2][32];		// ������ �������, ������������ ��������� �������
	int in_count[2];		// ���������� ������ �� ������
	int out_count[2];		// ���������� ������� �� ������
	int num1, num2;			// ������ ������� in � out
	int col;				// ����� ������� � ������� (�� ini �����)
	TColor *activeIn;		// ���� ��������� �����
	TColor *notActiveIn;	// ���� ����������� �����
	TColor *activeOut;		// ���� ��������� ������
	TColor *notActiveOut;	// ���� ����������� ������

	map <UnicodeString,Signal>::iterator cur;	// �������� ��� �������� �� ������
	int width , height;		// ������ � ������ ��������, ������������ ������
	int mar , hmar;				// �������������� � ������ �������, ���-�� ������� � �������
//	int in_count;				// ����� ������� ��������
//	int out_count;				// ����� �������� ��������
	int max_row;				// �����
	int cards;					// ������� ���� � ������
	void CopyMaps();			// �������� ����� �� digital, ����� �� �������� ��

public:		// User declarations
	__fastcall TSignalsStateForm(TComponent* Owner);

};
//---------------------------------------------------------------------------
extern PACKAGE TSignalsStateForm *SignalsStateForm;
//---------------------------------------------------------------------------
#endif

