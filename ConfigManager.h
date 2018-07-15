#pragma once

//////////////////////////////////////////////////////////////////////////
//form: num;tttt
//i4 int long 
//i2 short	
//sx char[x]  
//b  bool 
//B unsigned char 
//Cx Choice 
//x ÷ÿ∏¥
//////////////////////////////////////////////////////////////////////////

class TiXmlNode;
class TiXmlElement;

typedef struct _cofnig_pack_
{
	BYTE		*data;
	LONG		configID;
	LONG		chnn;
	LPCTSTR		form;
	LONG		len;
	LONG		sublen;
	LPCTSTR		headers;
	int			type;
}CONFIG_PACK;

class CConfigManager
{
public:
	CConfigManager(void);
	virtual ~CConfigManager(void);

	void Feed(LONG ID, LONG chnn, const BYTE *data, LONG dataLen, LPTSTR form, LONG structLen, LPTSTR headers, int type = 0);
	BOOL GetData(LONG ID, BYTE *buff, LONG &len, LONG &sublen, int &chnn, int &type);

	BOOL Export(LPTSTR path);
	BOOL Import(LPTSTR path);

	void Destory();

protected:
	TiXmlElement*			ParseData(CONFIG_PACK*);
	BOOL					ParseXml(TiXmlElement*, CONFIG_PACK&);
	TiXmlElement*			LinkElement(TiXmlNode*, LPCTSTR);
	void					LinkText(TiXmlNode*, LPCTSTR);
	void					LineFeed(TiXmlNode*);
	CList<CONFIG_PACK>		m_data;
};
