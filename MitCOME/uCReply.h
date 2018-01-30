// ---------------------------------------------------------------------------
#ifndef uCReplyH
#define uCReplyH
// ---------------------------------------------------------------------------
#include<System.hpp>
class CReply
{
private:
protected:
	AnsiString ENQ;
	AnsiString STX;
	AnsiString ETX;
	AnsiString ACK;
	AnsiString NAK;
	AnsiString CR;
	AnsiString LF;

	bool is_data_size;
	AnsiString CRC(AnsiString _packet);
	AnsiString IntToHex2(int _v);
	AnsiString IntToHex(int _v);
public:
	CReply(int _abonent);

	AnsiString result;
	AnsiString reply_type;
	int data_size;
	int data_size0;
	AnsiString abonent;
	virtual bool parse(AnsiString _l)
	{
		return(false);
    }
	inline bool IsDataSize()
	{
		return (is_data_size);
	}
};
class CReplyE : public CReply
{
public:
	CReplyE(int _abonent) : CReply(_abonent){}
	bool parse(AnsiString _l);
};
class CReplyE1 : public CReply
{
public:
	CReplyE1(int _abonent) : CReply(_abonent){}
	virtual bool parse(AnsiString _l);
};
class CReplyE2 : public CReply
{
public:
	CReplyE2(int _abonent) : CReply(_abonent){}
	virtual bool parse(AnsiString _l);
};
class CReplyE3 : public CReply
{
public:
	CReplyE3(int _abonent) : CReply(_abonent){}
	virtual bool parse(AnsiString _l);
};
class CReplyC : public CReply
{
public:
	CReplyC(int _abonent) : CReply(_abonent){}
	virtual bool parse(AnsiString _l);
};
class CReplyD : public CReply
{
public:
	CReplyD(int _abonent) : CReply(_abonent){}
	virtual bool parse(AnsiString _l);
};
#endif
