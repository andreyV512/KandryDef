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

	map <String,Signal> *inmap;		// карта входов из AdvantechSignals.h
	map <String,Signal> *outmap;	// карта выходов из AdvantechSignals.h

//	TPanel *in[64];			// массив панелей, показывающих состояние входов
//	TPanel *out[64];		// массив панелей, показывающих состояние выходов
	TPanel *in[2][32];		// массив панелей, показывающих состояние входов
	TPanel *out[2][32];		// массив панелей, показывающих состояние выходов
	int in_count[2];		// количество входов по платам
	int out_count[2];		// количество выходов по платам
	int num1, num2;			// номера панелей in и out
	int col;				// число панелек в столбце (из ini файла)
	TColor *activeIn;		// цвет активного входа
	TColor *notActiveIn;	// цвет неактивного входа
	TColor *activeOut;		// цвет активного выхода
	TColor *notActiveOut;	// цвет неактивного выхода

	map <UnicodeString,Signal>::iterator cur;	// итератор для прогулок по картам
	int width , height;		// ширина и высота панельки, отображающей сигнал
	int mar , hmar;				// горизонтальный и вертик отступы, кол-во панелек в колонке
//	int in_count;				// число входных сигналов
//	int out_count;				// число выходных сигналов
	int max_row;				// хуета
	int cards;					// сколько плат в работе
	void CopyMaps();			// копирует карты из digital, чтобы не завалить их

public:		// User declarations
	__fastcall TSignalsStateForm(TComponent* Owner);

};
//---------------------------------------------------------------------------
extern PACKAGE TSignalsStateForm *SignalsStateForm;
//---------------------------------------------------------------------------
#endif

