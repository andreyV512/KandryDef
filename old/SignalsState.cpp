//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SignalsState.h"
#include "Global.h"
#include <Math.hpp>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSignalsStateForm *SignalsStateForm;

//---------------------------------------------------------------------------
__fastcall TSignalsStateForm::TSignalsStateForm(TComponent* Owner)
	: TForm(Owner)
{
// мониторим статус сигналов
	if ( digital->cards_exist >= digital->cards_needed )
	{
		width=100, height=20;		// ширина и высота панельки, отображающей сигнал
		mar=40,hmar=20;				// горизонтальный и вертик отступы, кол-во панелек в колонке
//		in_count = 0;				// число входных сигналов
//		out_count = 0;				// число выходных сигналов
		inmap = new map <UnicodeString,Signal>;
		outmap = new map <UnicodeString,Signal>;
		CopyMaps();
		cards = digital->cards_needed;
		for (int i = 0; i < cards; i++ )
		{
			int itemp = 0;
			int otemp = 0;
			for (cur = inmap->begin(); cur != inmap->end(); cur++)
			{
				if ( i != cur->second.card )
					continue;
		// инициализируем входы - панели
				in[i][itemp] = new  TPanel( SignalsStateForm );
				in[i][itemp] -> Parent = SignalsStateForm;
				in[i][itemp] -> Caption = cur->first;
				in[i][itemp] -> ParentBackground = false;
				in[i][itemp] -> Tag = cur->second.channel;
				in[i][itemp] -> Hint = IntToStr (in[i][itemp] -> Tag);
				in[i][itemp] -> ShowHint = true;
//				in[i][itemp] -> Width = in[num1]->ExplicitWidth;
				itemp ++;
			}
			in_count[i] = itemp;
		// инициализируем выходы - чекбоксы
			for (cur = outmap->begin(); cur != outmap->end(); cur++)
			{
				if ( i != cur->second.card )
					continue;
				out[i][otemp] = new TPanel( SignalsStateForm );
				out[i][otemp] -> Parent=SignalsStateForm;
				out[i][otemp] -> Caption = cur->first;
				out[i][otemp] -> ParentBackground = false;
				out[i][otemp] -> Tag = cur->second.channel;
				out[i][otemp] -> Hint = IntToStr (out[i][otemp] -> Tag);
				out[i][otemp] -> ShowHint = true;
//				out[i][otemp] -> Width = out[num2]->ExplicitWidth;
				otemp++;
			}
			out_count[i] = otemp;
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TSignalsStateForm::FormShow(TObject *Sender)
{
// считываем нужные цвета из файла
	TIniFile *ini = new TIniFile(Globals::IniFileName);
	activeIn = new TColor(ini->ReadInteger("Color","ActiveIn",0) );
	notActiveIn = new TColor(ini->ReadInteger("Color","NotActiveIn",0) );
	activeOut = new TColor(ini->ReadInteger("Color","ActiveOut",0) );
	notActiveOut = new TColor(ini->ReadInteger("Color","NotActiveOut",0) );
	col = ini->ReadInteger("Default","SignalsStateCols" , 16) ;
	eCols->Text = col;
	delete ini;
// перестраиваем элементы в соответствии с желанием пользовател€
	max_row = 0;
	int in_rows, out_rows;
	int in_total = 0, out_total = 0;
	for (int i = 0; i < cards; i++)
	{
		for ( int c = 0; c < in_count[i]; c++ )
		{
		// cur = inmap->begin(); cur != inmap->end(); cur++
			int ch = in[i][c]->Tag;
			num1 = ( i * 32 ) + ch;
			in[i][c] -> SetBounds(hmar + (num1/col)*(width+hmar),mar+(num1%col)*(height+5),width,height);
//			in[i][c] -> SetBounds(hmar + (in_count/col)*(width+hmar),mar+(in_count%col)*(height+5),width,height);
			if ( c % col == 0 && c > 0)
				max_row = col;
			else
			{
				int t = c;
				while (t > col)
					t -= col;
				if ( t > max_row)
					max_row = t;
			}
		}
		in_total +=in_count[i];
	}
	// считаем, сколько колонок понадобилось
	int p = Math::Floor((float) in_total / col );
	in_rows = ( in_total % col == 0 ) ? ( col*p) : ( col * (p+1) );

	for (int i = 0; i < cards; i++)
	{
		for ( int c = 0; c < out_count[i]; c++ )
		{
			int ch = out[i][c]->Tag;
			num2 = (i * 32) + ch;
			out[i][c] -> SetBounds(hmar + ((in_rows+num2)/col)*(width+hmar),mar+(num2%col)*(height+5),width,height);
//			out[i][num2] -> SetBounds(hmar + ((in_rows+out_count)/col)*(width+hmar),mar+(out_count%col)*(height+5),width,height);
			if ( c % col == 0 && c > 0 )
				max_row = col;
			else
			{
				int t = c;
				while (t > col)
					t -= col;
				if ( t > max_row)
					max_row = t ;
			}
		}
		out_total += out_count[i];
	}
	// считаем, сколько колонок понадобилось
	p = Math::Floor((float) out_total / col );
	out_rows = (out_total % col == 0) ? (col*p) : ( col * (p+1) );

	// форма должна быть красивой! :)
	SignalsStateForm->Width = 2*hmar + ( in_rows + out_rows )/col * ( width + hmar );
	SignalsStateForm->Height = 3*mar + max_row * (height+5);
	SignalsStateForm->Refresh();

	InOutTimer->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TSignalsStateForm::FormDestroy(TObject *Sender)
{
	InOutTimer->Enabled = false;
	delete inmap;
	delete outmap;
	for ( int i = 0; i < cards; i++ )
	{
		for ( int c = 0; c < in_count[i]; c++ )
			delete in[i][c];
		for ( int c = 0; c < out_count[i]; c++ )
			delete out[i][c];
	}
}
//---------------------------------------------------------------------------

void __fastcall TSignalsStateForm::InOutTimerTimer(TObject *Sender)
{
// по таймеру обновл€ем входы и выходы, показываем их состо€ние цветом
	unsigned int word;
	map <UnicodeString,Signal>::iterator cur;
	map <UnicodeString,Signal>::iterator q;
	for ( int i = 0; i < cards; i++ )
	{
		word = digital->GetOutWord(i);
		for ( int c = 0; c < out_count[i]; c++ )
		{
			if ( word & (1 << out[i][c]->Tag) )
				SignalsStateForm->out[i][c]->Color = *activeOut;
			else
				SignalsStateForm->out[i][c]->Color = *notActiveOut;
		}
		word = digital->GetInWord(i);
		for ( int c = 0; c < in_count[i]; c++ )
		{
			if ( word & (1 << in[i][c]->Tag) )
				SignalsStateForm->in[i][c]->Color = *activeIn;
			else
				SignalsStateForm->in[i][c]->Color = *notActiveIn;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TSignalsStateForm::FormKeyPress(TObject *Sender, wchar_t &Key)
{
	if (Key == 27) SignalsStateForm->Close();
}
//---------------------------------------------------------------------------

void __fastcall TSignalsStateForm::FormClose(TObject *Sender, TCloseAction &Action)
{
	InOutTimer->Enabled = false;
	delete activeIn;
	delete notActiveIn;
	delete activeOut;
	delete notActiveOut;
}
//---------------------------------------------------------------------------

void __fastcall TSignalsStateForm::eColsChange(TObject *Sender)
{
// сохран€ем число элементов на столбец в файл
	TIniFile *ini = new TIniFile(Globals::IniFileName);
	col = eCols->Text.ToInt();
	ini->WriteInteger("Default","SignalsStateCols" , col) ;
	delete ini;
}
//---------------------------------------------------------------------------

void __fastcall TSignalsStateForm::nExitClick(TObject *Sender)
{
	SignalsStateForm->Close();
}
//---------------------------------------------------------------------------

void __fastcall TSignalsStateForm::nShowChannelClick(TObject *Sender)
{
	nShowChannel->Checked = ! nShowChannel->Checked;
	if ( nShowChannel->Checked )
	{
		for (int i = 0; i < cards; i++)
		{
			for ( int c = 0; c < in_count[i]; c++ )
				in[i][c] -> Caption = IntToStr(in[i][c]->Tag) +" " + in[i][c]->Caption ;
			for ( int c = 0; c < out_count[i]; c++ )
				out[i][c] -> Caption = IntToStr(out[i][c]->Tag) +" " + out[i][c]->Caption;
		}
	}
	else
	{
		for (int i = 0; i < cards; i++)
		{
			for ( int c = 0; c < in_count[i]; c++ )
				in[i][c] -> Caption = in[i][c] -> Caption.SubString(3,50);
			for ( int c = 0; c < out_count[i]; c++ )
				out[i][c] -> Caption = out[i][c] -> Caption.SubString(3,50);
		}
	}
	SignalsStateForm->Refresh();
}
//---------------------------------------------------------------------------
void TSignalsStateForm::CopyMaps()
{
	pair <String, Signal> p;
	for (cur = digital->inmap->begin(); cur != digital->inmap->end(); cur++)
	{
		p.first = cur->first;
		p.second.card = cur->second.card;
		p.second.channel = cur->second.channel;
		inmap->insert (p);
	}
	for (cur = digital->outmap->begin(); cur != digital->outmap->end(); cur++)
	{
		p.first = cur->first;
		p.second.card = cur->second.card;
		p.second.channel = cur->second.channel;
		outmap->insert(p);
	}
}
//---------------------------------------------------------------------------

