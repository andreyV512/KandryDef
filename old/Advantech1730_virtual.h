//---------------------------------------------------------------------------
#ifndef Advantech1730_virtualH
#define Advantech1730_virtualH
#include <Classes.hpp>
//---------------------------------------------------------------------------
class Advantech1730_virtual
{
public:
	//! �����������, ������ ��
	Advantech1730_virtual()
	{
		DoValue = 0;
	};
	//! �������� ����� (32 ����)
	unsigned int	DoValue;
	//! ������� ����� (32 ����)
	unsigned int	DiValue;
	//! ������������� �����, ���������� true � ������ ������
	bool Init(long DevNum, bool ReadOnly){return(false);};
	//! �������� �����
	bool Close(){return(false);};
	//! ���������� �������� �����, ���������� true � ������ ������
	bool SetOutWord(unsigned int OutWord){return(false);};
	//! ���������� �������� ���, ���������� true � ������ ������
	bool SetOutBit(byte bit){return(false);};
	//! ���������� �������� ���, ���������� true � ������ ������
	bool ResetOutBit(byte bit){return(false);};
	//! @brief �������������� ���������� � ���������� �������� ���, ���������� true � ������ ������
	//! @param bit ����� ����, ������� ���������� �������
	//! @param userTime ����� ������� ������� � ��, ���� �� ������, �� ����� ������� blinkTime
	bool BlinkOutBit(byte bit , int userTime=0){return(false);};
	//! ��������� �������� ����� � ���������� ��� � DoValue, ���������� true � ������ ������
	bool CheckOutWord(){return(false);};
	//! ��������� ������� ��������� ���� � ���������� ������� ���� (���������� ����� ��� ���)
	bool CheckOutBit(byte bit){return(false);};
	//! ��������� ������� ����� � ���������� ��� � DiValue, ���������� true � ������ ������
	bool CheckInWord(){return(false);};
	//! ��������� ������� �������� ����, ���������� ������� ���� (����/��� ���������� �� �����)
	bool CheckInBit(byte bit){return(false);};
	//! @brief ��������� ������� ��� � ������ ��������, ���������� ������� ����.
	//! ������� ��������� ��� � ��������� ���������� ������������ ���.
	//! ���� ����������� ���������, �� ��������������� ������������ (check_status),
	//! ������� ���������� ������ � ���������� ������� �������.
	//! @param bit ������� ���, ��������� �������� ����� ��������� �� �����
	//! @param check_status ���������, ������� �� ���������. True - ��� ���������, false - �������
	//! @retval ���������� ������� ����, ���������� �� ����, ��� �� ���������
	bool CheckInBouncelessBit(byte bit, bool check_status){return(false);};
	//! @brief ���� �������� ���� ������������ ����� � ��������� ��������� �����, ���������� ������� ����
	//! @param bit ����� ����, ��������� �������� �� ����
	//! @param DelayTimeMSec �����, ������� �� ����� ����� ��� (���� ������ 0, ����� ����� ����������)
	//! @param thread �����, � ������ ��������� �������� ������� ���������� false
	//! @retval True, ���� ��� ��������. False, ���� ����������� ����� ��� ��������� �����
	bool WaitForInBit(byte bit, unsigned int DelayTimeMSec, TThread *thread){return(false);};
	//! @brief ���� ������ �������� ���� ������������ ����� � ��������� ��������� �����, ���������� ������� ����
	//! @param bit ����� ����, ������ �������� �� ����
	//! @param DelayTimeMSec �����, ������� �� ����� ����� ����� ���� (���� ������ 0, ����� ����� ����������)
	//! @param thread �����, � ������ ��������� �������� ������� ���������� false
	//! @retval true, ���� ��� �������. False, ���� ����������� ����� ��� ��������� �����
	bool WaitForDropInBit(byte bit, unsigned int DelayTimeMSec, TThread *thread){return(false);};
};
#endif

