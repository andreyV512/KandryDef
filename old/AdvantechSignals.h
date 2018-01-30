//---------------------------------------------------------------------------
#ifndef AdvantechSignalsH
#define AdvantechSignalsH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "TeEngine.hpp"
#include "TeeProcs.hpp"
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include "Protocol.h"
#include <map.h>

#include "c:/Advantech/BioDAQ/Inc/BDaqCL.h"
#include "Advantech1730_virtual.h"
using namespace Automation::BDaq;


//! ����� ������ � ������ ����������� �����-������ Advantech PCI1730U
class Advantech1730   {
private:
	//! handle to the device
	BDaqDevice *device;
	//! error code (0=Success)
	ErrorCode errorCode;
	//! handle to input output
	BDaqDio *dio;
	TMutex *AdvMutex;

	//! ��������� ����, �� ��������� = 0 (���������� ���� �����)
	long  startPort;
	//! ����� ������, �� ��������� = 4 (���������� ���� �����)
	long  portCount;
	//! �����, ���������� ��� �������� ������� �� 4 ������
	BYTE  bufferForWriting[4];
	//! �����, ���������� ��� ������� ������� �� 4 ������
	BYTE  bufferForReading[4];
	//! �����, ����� �������� ���������, ���������� �� �������� ������
	short SleepTime;
	//! ����� �������� ��������� ������� � ������� BlinkOut
	short blinkTime;
	//! ����� �������� - ��������, ����� ������� ����� ����������� ��������� �������
	short bouncePeriod;
	//! ���-�� ���������� ��������
	short bounceRepeat;

public:
	//! �����������, ������ ��
	Advantech1730();
	//! ����������, ��������� ���
	~Advantech1730();
	//! �������� ����� (32 ����)
	unsigned int	DoValue;
	//! ������� ����� (32 ����)
	unsigned int	DiValue;

	//! ������������� �����, ���������� true � ������ ������
	bool Init(long DevNum, bool ReadOnly);
	//! �������� �����
	bool Close();
	//! ���������� �������� �����, ���������� true � ������ ������
	bool SetOutWord(unsigned int OutWord);
	//! ���������� �������� ���, ���������� true � ������ ������
	bool SetOutBit(byte bit);
	//! ���������� �������� ���, ���������� true � ������ ������
	bool ResetOutBit(byte bit);
	//! @brief �������������� ���������� � ���������� �������� ���, ���������� true � ������ ������
	//! @param bit ����� ����, ������� ���������� �������
	//! @param userTime ����� ������� ������� � ��, ���� �� ������, �� ����� ������� blinkTime
	bool BlinkOutBit(byte bit , int userTime=0);
	//! ��������� �������� ����� � ���������� ��� � DoValue, ���������� true � ������ ������
	bool CheckOutWord();
	//! ��������� ������� ��������� ���� � ���������� ������� ���� (���������� ����� ��� ���)
	bool CheckOutBit(byte bit);
	//! ��������� ������� ����� � ���������� ��� � DiValue, ���������� true � ������ ������
	bool CheckInWord();
	//! ��������� ������� �������� ����, ���������� ������� ���� (����/��� ���������� �� �����)
	bool CheckInBit(byte bit);
	//! @brief ��������� ������� ��� � ������ ��������, ���������� ������� ����.
	//! ������� ��������� ��� � ��������� ���������� ������������ ���.
	//! ���� ����������� ���������, �� ��������������� ������������ (check_status),
	//! ������� ���������� ������ � ���������� ������� �������.
	//! @param bit ������� ���, ��������� �������� ����� ��������� �� �����
	//! @param check_status ���������, ������� �� ���������. True - ��� ���������, false - �������
	//! @retval ���������� ������� ����, ���������� �� ����, ��� �� ���������
	bool CheckInBouncelessBit(byte bit, bool check_status);
	//! @brief ���� �������� ���� ������������ ����� � ��������� ��������� �����, ���������� ������� ����
	//! @param bit ����� ����, ��������� �������� �� ����
	//! @param DelayTimeMSec �����, ������� �� ����� ����� ��� (���� ������ 0, ����� ����� ����������)
	//! @param thread �����, � ������ ��������� �������� ������� ���������� false
	//! @retval True, ���� ��� ��������. False, ���� ����������� ����� ��� ��������� �����
	bool WaitForInBit(byte bit, unsigned int DelayTimeMSec, TThread *thread);
	//! @brief ���� ������ �������� ���� ������������ ����� � ��������� ��������� �����, ���������� ������� ����
	//! @param bit ����� ����, ������ �������� �� ����
	//! @param DelayTimeMSec �����, ������� �� ����� ����� ����� ���� (���� ������ 0, ����� ����� ����������)
	//! @param thread �����, � ������ ��������� �������� ������� ���������� false
	//! @retval true, ���� ��� �������. False, ���� ����������� ����� ��� ��������� �����
	bool WaitForDropInBit(byte bit, unsigned int DelayTimeMSec, TThread *thread);


};

//! ��������� ������ - ��������� ��������� ������ ����������� ������� �� ����� Advantech
struct Signal
{
	//! ����� �����
	byte card;
	//! ����� ������
	byte channel;
	//! ������������ �� ������
	bool inverted;
//	Signal operator = (int code);
//	Signal operator = (Signal s);
//	Signal& operator = (Signal& );
};


//! ����� ������ �� ����� ��������� ���������, ���������� �� �����, �� ������� ������� ������
class AllSignals {
private:
	static bool __stdcall EnumDeviceCallBack(BDaqEnumerator::EnumDeviceArgs *args, void *ctx);
	static BDaqEnumerator::EnumDeviceArgs devarg[7];
	//!���������� ��� �������� ������������ ����
	BDaqEnumerator en;
	//! ������ ����� �������� �������� �� ini �����
	void LoadSignalNames();

public:
	//! 2 ��������� �� ����� Advantech1730
	Advantech1730_virtual *AdvCards[2];
//	Advantech1730 *AdvCards[2];
	//! �����������, ����������� ������ ��� ��������� �� �����
	AllSignals();
	//! ����������, ����������� ������
	~AllSignals();

	//! ���-�� ����, ������������ � ������� (������� �� EnumDeviceCallBack)
	static byte cards_exist;
	//! ���-�� ����, ��������������������� (�� ������� ������� ������������)
	byte cards_inited;
	//! ���-�� ����, ������� ����� ��� ������ (������� �� ����� IO_management.ini)
	byte cards_needed;

	//! ����� ������, ��� ���� - ��� �������, ������ - ����� ����� � ������
	map <String, Signal> *inmap;
	//! ����� �������, ��� ���� - ��� �������, ������ - ����� ����� � ������
	map <String, Signal> *outmap;

	//! ���� ��� ��������� ����� Advantech �� ����������
	void FindCards();
	//! @brief ������������� ���� ���� , ���������� ����� ������������������ ����
	//! @param ReadOnly ��������� � ������ "������ ��� ������" (������ ���������� ������)
	//! @retval ���������� ����� ������� ��������������������� ����
	short InitializeCards(bool ReadOnly);
	//! �������� ���� ���� Advantech
	bool CloseCards();
	//! @brief ������� ��������� ������ �� ������
	//! @param SignalName ��� �������, ����� �������� ����� �����
	//! @param mapname ��� ����� �������� (������� ��� ��������), ������� ����������� ������
	Signal FindSignal(String SignalName, map<String,Signal> *mapname );
	//! ���������� true , ���� ������ ���� � ����� �������� (����� IO_Management.ini)
	bool SignalExists(String SignalName);
	//! ���������� �������� ������, ���������� true � ������ ������
	bool SetOutSignal(String SignalName);
	//! ���������� �������� ������, ���������� true � ������ ������
	bool ResetOutSignal(String SignalName);
	//! @brief ����������, � ����� ����� �������� ���������� �������� ������, ���������� true � ������ ������
	//! @param SignalName ��� �������, ������� ���������� �������
	//! @param userTime ����� ������� ������� � ��, ���� �� ������, �� ����� ������� blinkTime
	bool BlinkOutSignal(String SignalName , int userTime = 0);
	//! ���������, ���� �� ������������ �������� ������, ���������� ������ �������
	bool CheckOutSignal(String SignalName);
	//! ���������, ���� �� ������������ ������� ������, ���������� ������ �������
	bool CheckInSignal(String SignalName);
	//! @brief ��������� ������� �������� ������� c ������ �������� .
	//! ������� ��������� ��� � ��������� ���������� ������������ ������� ������.
	//! ���� ����������� ���������, �� ��������������� ������������ (check_status),
	//! ������� ���������� ������ � ���������� ������� �������
	//! @param SignalName ��� �������, ��������� �������� ����� ��������� �� �����
	//! @param check_status ���������, ������� �� ���������. True - ��� ���������, false - �������
	//! @retval ���������� ������� ����, ���������� �� ����, ��� �� ���������
	bool CheckInBouncelessSignal(String SignalName, bool check_status);

	//! @brief ���� �������� ������� ������������ ����� � ��������� ��������� �����, ���������� ������� ����
	//! @param SignalName ��� �������� �������, ��������� �������� �� ����
	//! @param DelayTimeMSec �����, ������� �� ����� ����� ������ (���� ������ 0, ����� ����� ����������)
	//! @param thread �����, � ������ ��������� �������� ������� ���������� false
	//! @retval true, ���� ������ ��������. False, ���� ����������� ����� ��� ��������� �����
	bool WaitForInSignal(String SignalName, int DelayTimeMSec, TThread *thread = NULL);
	//! @brief ���� ������ �������� ������� ������������ ����� � ��������� ��������� �����, ���������� ������� ����
	//! @param SignalName ��� �������� �������, ������ �������� �� ����
	//! @param DelayTimeMSec �����, ������� �� ����� ����� ����� ������� (���� ������ 0, ����� ����� ����������)
	//! @param thread �����, � ������ ��������� �������� ������� ���������� false
	//! @retval true, ���� ������ �������. False, ���� ����������� ����� ��� ��������� �����
	bool WaitForDropInSignal(String SignalName, int DelayTimeMSec, TThread *thread = NULL);
	//! ���������� ������� ����� (32 ����) ������ �����
	unsigned int GetInWord(int adv_card);
	//! ���������� �������� ����� (32 ����) ������ �����
	unsigned int GetOutWord(int adv_card);
};


	byte AllSignals::cards_exist = 0;
	BDaqEnumerator::EnumDeviceArgs AllSignals::devarg[7];
//---------------------------------------------------------------------------
//! ���������� ���������, ����������� ��������� ��������� �� ����� ����� ���������.
extern AllSignals *digital;
//---------------------------------------------------------------------------
#endif
