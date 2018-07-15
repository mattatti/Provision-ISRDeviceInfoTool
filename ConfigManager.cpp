#include "StdAfx.h"
#include "ConfigManager.h"
#include "tinyxml.h"
#include "SafeFile.h"
#include "ConfigHelpers.h"
#include <string>
#include <WinSock2.h>

CConfigManager::CConfigManager(void)
{
}

CConfigManager::~CConfigManager(void)
{
	Destory();
}

void CConfigManager::Feed(LONG ID, LONG chnn, const BYTE *data, LONG dataLen, LPTSTR form, LONG structLen, LPTSTR headers, int type )
{
	CONFIG_PACK pack;
	pack.configID = ID;
	pack.len = dataLen;
	pack.form = form;
	pack.sublen = structLen;
	pack.headers = headers;
	pack.chnn = chnn;
	if (data)
	{
		pack.data = new BYTE[dataLen];
		memcpy(pack.data, data, dataLen);
	}
	else
	{
		pack.data = NULL;
	}
	pack.type = type;
	m_data.AddTail(pack);
}

BOOL CConfigManager::Export( LPTSTR path )
{
	
	CString fname = path;
	CString temp;
	CString tableName;
	//生成Config.html
	fname += _T("\\config.html");
	TiXmlDocument configDoc;
	TiXmlElement *configHtml = LinkElement(&configDoc, "html");
	LinkText(LinkElement(configHtml, "head"), " ");
	TiXmlElement *configBody = LinkElement(configHtml, "body");
	TiXmlElement *configStyle = LinkElement(configBody, "style");
	configStyle->SetAttribute("type", "text/css");
	LinkText(configStyle, CONFIG_CSS);

	POSITION pos = m_data.GetHeadPosition();
	while(pos != NULL)
	{
		CONFIG_PACK &pack = m_data.GetNext(pos);
		TiXmlElement *table = ParseData(&pack);
		tableName.Format(_T("config_%d"), pack.configID);
		table->SetAttribute("name", tableName.GetBuffer());
		table->SetAttribute("class", "table");
		table->SetAttribute("cellspacing", 0);
		table->SetAttribute("type", pack.type);
		TiXmlElement *title = LinkElement(configBody, "a");
		LinkText(title, ConfigID[pack.configID]);
		temp.Format(_T("anchor_%d"), pack.configID);
		title->SetAttribute("name", temp.GetBuffer());
		LineFeed(configBody);
		configBody->LinkEndChild(table);
		LineFeed(configBody);
	}

	configDoc.SaveFile(fname);

	//生成index.html
	TiXmlDocument indexDoc;
	fname = path;
	fname += _T("\\index.html");
	TiXmlElement *indexHtml = LinkElement(&indexDoc, "html");
	LinkText(LinkElement(indexHtml, "head"), " ");
	TiXmlElement *indexBody = LinkElement(indexHtml, "body");
	TiXmlElement *indexStyle = LinkElement(indexBody, "style");
	indexStyle->SetAttribute("type", "text/css");
	LinkText(indexStyle, INDEX_CSS);
	TiXmlElement *ul = LinkElement(indexBody, "ul");

	pos = m_data.GetHeadPosition();
	while(pos != NULL)
	{
		CONFIG_PACK &pack = m_data.GetNext(pos);
		TiXmlElement *li = LinkElement(ul, "li");
		TiXmlElement *title = LinkElement(li, "a");
		LinkText(title, ConfigID[pack.configID]);
		title->SetAttribute("target", "config");
		temp.Format(_T("config.html#anchor_%d"), pack.configID);
		title->SetAttribute("href", temp.GetBuffer());
		LineFeed(ul);
	}
	indexDoc.SaveFile(fname);

	//生成main.html
	TiXmlDocument mainDoc;
	fname = path;
	fname += _T("\\main.html");
	TiXmlElement *mainHtml = LinkElement(&mainDoc, "html");
	
	TiXmlElement *frameset = LinkElement(mainHtml, "frameset");
	frameset->SetAttribute("cols", "180, *");
	frameset->SetAttribute("border", 0);
	
	LinkElement(frameset, "frame")->SetAttribute("src", "index.html");
	TiXmlElement * right = LinkElement(frameset, "frame");
	right->SetAttribute("src", "Config.html");
	right->SetAttribute("name", "config");

	mainDoc.SaveFile(fname);

	return TRUE;
}

TiXmlElement* CConfigManager::ParseData(CONFIG_PACK *pack)
{
	
	TiXmlElement *table = new TiXmlElement("table");
	table->SetAttribute("border", 1);
	TCHAR *start = NULL;
	int num = _tcstol(pack->form, &start, 10);
	start++;//跳过 ;
	CString head;

	TiXmlElement *headrow = LinkElement(table, "tr");
	TiXmlElement *headTd = LinkElement(headrow, "th");
	headTd->SetAttribute("style", "width:50px");
	LinkText(headTd, " ");
	for (int i = 0; i < num; i++)
	{
		AfxExtractSubString(head, pack->headers, i, '|');
		LinkText(LinkElement(headrow, "th"), head.GetBuffer());
	}
	
	int n = pack->len / pack->sublen;
	CString text;

	for (int i = 0; i < n; i++)
	{
		TiXmlElement *row = LinkElement(table, "tr");
		if (pack->chnn >= 0)
		{
			text.Format("%d", pack->chnn + 1);
			LinkText(LinkElement(row, "td"), text.GetBuffer());	
		}
		else
		{
			text.Format("%d", i + 1);
			LinkText(LinkElement(row, "td"), text.GetBuffer());
		}

		CSafeMemFile smf(pack->data + pack->sublen * i, pack->sublen);
		TCHAR *out = start;
		while (out - pack->form < _tcslen(pack->form))
		{
			if (*out == 'i')
			{	
				out++;
				
				TCHAR *temp = out;
				int n = _tcstol(temp, &out, 10);
				int times = 1;
				TCHAR * peek = out;
				if (*peek == 'x')
				{
					temp = ++peek;
					times = _tcstol(temp, &peek, 10);
					out = peek;
				}
				for (int j = 0; j <times; j++)
				{
					if (n == 4)
					{
						unsigned long v = smf.ReadUInt32();
						text.Format(_T("%d"), v);
						LinkText(LinkElement(row, "td"), text.GetBuffer());
					}
					else if(n == 2)
					{
						unsigned short v = smf.ReadUInt16();
						text.Format(_T("%d"), v);
						LinkText(LinkElement(row, "td"), text.GetBuffer());
					}
				}
			}
			else if(*out == 's')
			{
				out++;
				TCHAR *temp = out;
				int cn = _tcstol(temp, &out, 10);
				char *buff = new char[cn];

				int times = 1;
				TCHAR * peek = out;
				if (*peek == 'x')
				{
					temp = ++peek;
					times = _tcstol(temp, &peek, 10);
					out = peek;
				}
				for (int j = 0; j < times; j++)
				{
					memset(buff, 0, cn);
					smf.Read(buff, cn);
					LinkText(LinkElement(row, "td"), buff);
				}
				delete[] buff;
			}
			else if(*out == 'b')
			{
				out++;

				int times = 1;
				TCHAR * peek = out;
				if (*peek == 'x')
				{
					TCHAR *temp = ++peek;
					times = _tcstol(temp, &peek, 10);
					out = peek;
				}
				for (int j = 0; j < times; j++)
				{
					unsigned char v = smf.ReadUInt8();
					if (v)
					{
						text = "true";
					}
					else
					{
						text= "false";
					}
					LinkText(LinkElement(row, "td"), text.GetBuffer());
				}
				
			}
			else if (*out == 'B')
			{
				out++;
				int times = 1;
				TCHAR * peek = out;
				if (*peek == 'x')
				{
					TCHAR *temp = ++peek;
					times = _tcstol(temp, &peek, 10);
					out = peek;
				}
				for (int j = 0; j < times; j++)
				{
					unsigned char v = smf.ReadUInt8();
					text.Format(_T("%d"), v);
					LinkText(LinkElement(row, "td"), text.GetBuffer());
				}
			}
			else if (*out == 'C')
			{
				out++;
				TCHAR *temp = out;
				int n = _tcstol(temp, &out, 10);
				
				int times = 1;
				TCHAR * peek = out;
				if (*peek == 'x')
				{
					temp = ++peek;
					times = _tcstol(temp, &peek, 10);
					out = peek;
				}
				for (int j = 0; j < times; j++)
				{
					int choice = smf.ReadUInt32();
					AfxExtractSubString(text, ConfigChoice[n], choice, '|'); 
					LinkText(LinkElement(row, "td"), text.GetBuffer());
				}
			}
			else if(*out == 'p')
			{
				out++;

				int times = 1;
				TCHAR * peek = out;
				if (*peek == 'x')
				{
					TCHAR *temp = ++peek;
					times = _tcstol(temp, &peek, 10);
					out = peek;
				}
				for (int j = 0; j < times; j++)
				{
					int x = smf.ReadUInt8();
					int y = smf.ReadUInt8();
					text.Format(_T("%d, %d"), x, y);
					LinkText(LinkElement(row, "td"), text.GetBuffer());
				}
			}
			else if(*out == 'P')
			{
				out++;
				int times = 1;
				TCHAR * peek = out;
				if (*peek == 'x')
				{
					TCHAR *temp = ++peek;
					times = _tcstol(temp, &peek, 10);
					out = peek;
				}
				for (int j = 0; j < times; j++)
				{
					int ip = smf.ReadUInt32();
					IN_ADDR addr;
					addr.S_un.S_addr = ip;
					text = inet_ntoa(addr);
					LinkText(LinkElement(row, "td"), text.GetBuffer());
				}
			}
			else if (*out == 'h')
			{
				out++;
				int times = 1;
				TCHAR * peek = out;
				if (*peek == 'x')
				{
					TCHAR *temp = ++peek;
					times = _tcstol(temp, &peek, 10);
					out = peek;
				}
				for (int j = 0; j < times; j++)
				{
					int hex = smf.ReadUInt32();
					text.Format(_T("0x%x"), hex);
					LinkText(LinkElement(row, "td"), text.GetBuffer());
				}
			}
			else if (*out == 'M')
			{
				out++;
				int times = 1;
				TCHAR * peek = out;
				if (*peek == 'x')
				{
					TCHAR *temp = ++peek;
					times = _tcstol(temp, &peek, 10);
					out = peek;
				}
				for (int j = 0; j < times; j++)
				{
					char mac[8];
					for (int i = 0; i < 8; i++)
					{
						mac[i] = smf.ReadUInt8();
					}
					
					text.Format(_T("%02x:%02x:%02x:%02x:%02x:%02x"), mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
					LinkText(LinkElement(row, "td"), text.GetBuffer());
				}
			}
			else
			{
				ASSERT(FALSE);
				out++;
			}
		}
	}
	return table;
}

TiXmlElement* CConfigManager::LinkElement( TiXmlNode* node, LPCTSTR name)
{
	TiXmlElement *n = new TiXmlElement(name);
	node->LinkEndChild(n);
	return n;
}

void CConfigManager::LinkText( TiXmlNode* node, LPCTSTR name)
{
	TiXmlText *text = new TiXmlText(name);
	node->LinkEndChild(text);
}

void CConfigManager::LineFeed( TiXmlNode* node)
{
	TiXmlElement *br = new TiXmlElement("br");
	node->LinkEndChild(br);
}

void CConfigManager::Destory()
{
	while(!m_data.IsEmpty())
	{
		CONFIG_PACK& pack = m_data.GetHead();
		delete[] pack.data;
		m_data.RemoveHead();
	}
}

BOOL CConfigManager::Import( LPTSTR path )
{
	CString fname = path;
	fname += _T("\\config.html");
	TiXmlDocument configDoc(fname.GetBuffer());
	if (!configDoc.LoadFile())
	{
		return FALSE;
	}
	
	TiXmlElement *body = configDoc.RootElement()->IterateChildren("body",\
		configDoc.RootElement()->FirstChildElement())->ToElement();
	
	TiXmlNode *node = NULL;
	TiXmlElement *table = NULL;
	int i = 0;
	std::string tableName;
	CString tempName;

	//配置表格id大的会在id小的后面，所以不会遗漏
	while(i < m_data.GetCount() && (node = body->IterateChildren("table", table)))
	{
		table = node->ToElement();
		CONFIG_PACK &pack = m_data.GetAt(m_data.FindIndex(i));
		tempName.Format(_T("config_%d"), pack.configID);
		table->QueryStringAttribute("name", &tableName);

		if (!tempName.CompareNoCase(tableName.c_str()))
		{
			int type = 0;
			table->QueryIntAttribute("type", &type);

			i += type;//配置有一种配置项有多种格式的情况，用type来修正
			CONFIG_PACK &realpack = m_data.GetAt(m_data.FindIndex(i));

			if (!ParseXml(table, realpack))
			{
				return FALSE;
			}
			int t = pack.configID;

			while (i < m_data.GetCount() && m_data.GetAt(m_data.FindIndex(i)).configID == t)
			{
				i++;
			}
		}
	}
	return TRUE;
}

BOOL CConfigManager::ParseXml( TiXmlElement* table, CONFIG_PACK& pack)
{
	const char *text = NULL;
	char *start = NULL;
	int num = _tcstol(pack.form, &start, 10);

	CSafeMemFile smf;

	TiXmlNode *row = table->FirstChildElement("tr")->NextSiblingElement("tr");

	for (; row; row = table->IterateChildren("tr", row))
	{
		TiXmlNode *td = row->FirstChildElement("td");
		if (td)
		{
			text = td->ToElement()->GetText();
		}
		pack.sublen = 0;
		char *form = start;
		while (td)
		{
			if (*form == ';')
			{
				form++;
				int v = _tcstol(text, NULL, 10);
				if (pack.chnn != -1)
				{
					pack.chnn = v - 1;//如果表格有多行的话 chnn也是无效的 chnn会是最后一行的通道数
				}
				td = row->IterateChildren("td", td);
				if (td)
				{
					text = td->ToElement()->GetText();
				}
			}
			else if (*form == 'i')
			{	
				form++;

				TCHAR *temp = form;
				int n = _tcstol(temp, &form, 10);
				int times = 1;
				TCHAR * peek = form;
				if (*peek == 'x')
				{
					temp = ++peek;
					times = _tcstol(temp, &peek, 10);
					form = peek;
				}
				for (int j = 0; j <times; j++)
				{
					int v = _tcstol(text, NULL, 10);
					if (n == 4)
					{
						smf.WriteUInt32(v);
						pack.sublen += 4;
					}
					else if(n == 2)
					{
						smf.WriteUInt16(v);
						pack.sublen += 2;
					}

					td = row->IterateChildren("td", td);
					if (td)
					{
						text = td->ToElement()->GetText();
					}
				}
			}
			else if(*form == 's')
			{
				form++;
				TCHAR *temp = form;
				int cn = _tcstol(temp, &form, 10);
				char *buff = new char[cn];

				int times = 1;
				TCHAR * peek = form;
				if (*peek == 'x')
				{
					temp = ++peek;
					times = _tcstol(temp, &peek, 10);
					form = peek;
				}
				for (int j = 0; j < times; j++)
				{
					memset(buff, 0, cn);

					if (text)
					{
						memcpy(buff, text, _tcslen(text) + 1);
					}
					
					smf.Write(buff, cn);
					pack.sublen += cn;
					td = row->IterateChildren("td", td);
					if (td)
					{
						text = td->ToElement()->GetText();
					}
				}
				delete[] buff;
			}
			else if(*form == 'b')
			{
				form++;

				int times = 1;
				TCHAR * peek = form;
				if (*peek == 'x')
				{
					TCHAR *temp = ++peek;
					times = _tcstol(temp, &peek, 10);
					form = peek;
				}
				for (int j = 0; j < times; j++)
				{
					if (_tcscmp(text, "true") == 0)
					{
						smf.WriteUInt8(1);
					}
					else if (_tcscmp(text, "false") == 0)
					{
						smf.WriteUInt8(0);
					}
					else
					{
						return FALSE;
					}
					pack.sublen += 1;
					td = row->IterateChildren("td", td);
					if (td)
					{
						text = td->ToElement()->GetText();
					}
				}

			}
			else if (*form == 'B')
			{
				form++;
				int times = 1;
				TCHAR * peek = form;
				if (*peek == 'x')
				{
					TCHAR *temp = ++peek;
					times = _tcstol(temp, &peek, 10);
					form = peek;
				}
				for (int j = 0; j < times; j++)
				{
					unsigned char v = _tcstol(text, NULL, 10); 
					smf.WriteUInt8(v);
					pack.sublen += 1;
					td = row->IterateChildren("td", td);
					if (td)
					{
						text = td->ToElement()->GetText();
					}
				}
			}
			else if (*form == 'C')
			{
				form++;
				TCHAR *temp = form;
				int n = _tcstol(temp, &form, 10);

				int times = 1;
				TCHAR * peek = form;
				if (*peek == 'x')
				{
					temp = ++peek;
					times = _tcstol(temp, &peek, 10);
					form = peek;
				}
				for (int j = 0; j < times; j++)
				{
					CString choice; 
					int len = 1;

					for (int i = 0; i < _tcslen(ConfigChoice[n]); i++)
					{
						if (ConfigChoice[n][i] == '|')
						{
							len++;
						}
					}

					for (int i = 0; i < len; i++)
					{
						AfxExtractSubString(choice, ConfigChoice[n], i, '|');
						if (!choice.CompareNoCase(text))
						{
							smf.WriteUInt32(i);
							pack.sublen += 4;
							td = row->IterateChildren("td", td);
							if (td)
							{
								text = td->ToElement()->GetText();
							}

							break;
						}

						if (i == len - 1)
						{
							return FALSE;
						}
					}
				}
			}
			else if(*form == 'p')
			{
				form++;

				int times = 1;
				TCHAR * peek = form;
				if (*peek == 'x')
				{
					TCHAR *temp = ++peek;
					times = _tcstol(temp, &peek, 10);
					form = peek;
				}
				for (int j = 0; j < times; j++)
				{
					int x, y;
					sscanf_s(text, _T("%d, %d"), &x, &y);
					smf.WriteUInt8(x);
					smf.WriteUInt8(y);
					pack.sublen += 2;
					td = row->IterateChildren("td", td);
					if (td)
					{
						text = td->ToElement()->GetText();
					}
				}
			}
			else if(*form == 'P')
			{
				form++;
				int times = 1;
				TCHAR * peek = form;
				if (*peek == 'x')
				{
					TCHAR *temp = ++peek;
					times = _tcstol(temp, &peek, 10);
					form = peek;
				}
				for (int j = 0; j < times; j++)
				{
					unsigned long ip =inet_addr(text);
					smf.WriteUInt32(ip);
					pack.sublen += 4;
					td = row->IterateChildren("td", td);
					if (td)
					{
						text = td->ToElement()->GetText();
					}
				}
			}
			else if (*form == 'h')
			{
				form++;
				int times = 1;
				TCHAR * peek = form;
				if (*peek == 'x')
				{
					TCHAR *temp = ++peek;
					times = _tcstol(temp, &peek, 10);
					form = peek;
				}
				for (int j = 0; j < times; j++)
				{
					int hex = _tcstol(text, NULL, 16);
					smf.WriteUInt32(hex);
					pack.sublen += 4;
					td = row->IterateChildren("td", td);
					if (td)
					{
						text = td->ToElement()->GetText();
					}
				}
			}
			else if (*form == 'M')
			{
				form++;
				int times = 1;
				TCHAR * peek = form;
				if (*peek == 'x')
				{
					TCHAR *temp = ++peek;
					times = _tcstol(temp, &peek, 10);
					form = peek;
				}
				for (int j = 0; j < times; j++)
				{
					unsigned long mac[8];
					memset(mac, 0x0, 8);
					sscanf_s(text, _T("%02x:%02x:%02x:%02x:%02x:%02x"), &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
					for (int i = 0; i < 8; i++)
					{
						smf.WriteUInt8(mac[i]);
					}
					pack.sublen += 8;
					td = row->IterateChildren("td", td);
					if (td)
					{
						text = td->ToElement()->GetText();
					}
				}
			}
			else
			{
				ASSERT(FALSE);
				return FALSE;
				form++;
			}
		}
	}

	if (smf.GetLength())
	{
		pack.len = smf.GetLength();
		pack.data = new BYTE[smf.GetLength()];
		memcpy(pack.data, smf.GetBuffer(), smf.GetLength());
	}

	return TRUE;
}

BOOL CConfigManager::GetData( LONG ID, BYTE *buff, LONG &len, LONG &sublen, int &chnn, int &type)
{
	POSITION pos = m_data.GetHeadPosition();
	for (int i = 0; i < m_data.GetCount(); i++)
	{
		CONFIG_PACK &pack = m_data.GetNext(pos);
		if (pack.configID == ID && pack.data != NULL)
		{
			if (buff && len)
			{
				memcpy_s(buff, len, pack.data, pack.len);
			}
			type = pack.type;
			len = pack.len;
			sublen = pack.sublen;
			chnn = pack.chnn;
			return TRUE;
		}
	}
	return FALSE;
}
