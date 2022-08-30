#pragma once
#include <windows.h>
using namespace std;

/*
* ΢���еĻ������ݽṹ
* buffer��UNICODE�ַ���
* length��`buffer`�ַ���
* maxLength��`buffer`����ַ���
* fill1��ռλ��Ա1��Ĭ��Ϊ0
* fill2��ռλ��Ա2��Ĭ��Ϊ0
* WxString��Ĭ�Ϲ��캯��
*/
struct WxString
{
    wchar_t *buffer;
    DWORD length;
    DWORD maxLength;
    DWORD fill1 = 0;
    DWORD fill2 = 0;
    WxString()
    {
        WxString(NULL);
    }
    WxString(wstring str)
    {
        buffer = (wchar_t *)str.c_str();
        length = str.length();
        maxLength = str.length() * 2;
    }
    WxString(const wchar_t *pStr)
    {
        WxString((wchar_t *)pStr);
    }
    WxString(int tmp)
    {
        buffer = NULL;
        length = 0x0;
        maxLength = 0x0;
    }
    WxString(wchar_t *pStr)
    {
        buffer = pStr;
        length = wcslen(pStr);
        maxLength = wcslen(pStr) * 2;
    }
};

/*
* ���浥����Ϣ�Ľṹ
* messagetype����Ϣ����
* sender��������wxid
* wxid�����sender��Ⱥ��id����˳�Ա������巢����wxid��������`sender`һ��
* message����Ϣ���ݣ����ı���Ϣ��xml��ʽ
* filepath��ͼƬ���ļ���������Դ�ı���·��
*/
struct ReceiveMsgStruct
{
    DWORD pid = 0;
    DWORD messagetype = 0;
    BOOL isSendMessage = 0;
    unsigned long long msgid = 0;
    wstring sender;
    wstring wxid;
    wstring message;
    wstring filepath;
    wstring time;
    wstring extrainfo;
    ~ReceiveMsgStruct()
    {
    }
};

// vector���ڴ��еı�����ʽ
struct VectorStruct
{
#ifdef _DEBUG
    DWORD v_head;
#endif
    DWORD v_data;
    DWORD v_end1;
    DWORD v_end2;
};

struct UserInfo
{
    int errcode;
    wchar_t *keyword;
    int l_keyword;
    wchar_t *v3;
    int l_v3;
    wchar_t *NickName;
    int l_NickName;
    wchar_t *Signature;
    int l_Signature;
    wchar_t *v2;
    int l_v2;
    wchar_t *Nation;
    int l_Nation;
    wchar_t *Province;
    int l_Province;
    wchar_t *City;
    int l_City;
    wchar_t *BigAvatar;
    int l_BigAvatar;
    wchar_t *SmallAvatar;
    int l_SmallAvatar;
    DWORD sex;
    BOOL over;
};

/*
* �������ݿⵥ������Ϣ�Ľṹ��
* name��������l_name��`name`�ַ���
* tbl_name��������l_tbl_name��`tbl_name`�ַ���
* sql��������䣻l_sql��`sql`�ַ���
* rootpage�����ţ�l_rootpage��`rootpage`�ַ���
*/
struct TableInfoStruct
{
    char *name;
    DWORD l_name;
    char *tbl_name;
    DWORD l_tbl_name;
    char *sql;
    DWORD l_sql;
    char *rootpage;
    DWORD l_rootpage;
};

/*
* �������ݿ���Ϣ�Ľṹ��
* handle�����ݿ���
* dbname�����ݿ���
* l_dbname��`dbname`�ַ���
* tables������������б���Ϣ������
* count�����б������
*/
struct DbInfoStruct
{
    DWORD handle;
    wchar_t *dbname;
    DWORD l_dbname;
    vector<TableInfoStruct> tables;
    DWORD count;
};

/*
* ���浥��������Ϣ�Ľṹ��
* wxIdAddr��wxid�����ַ
* wxNumberAddr��΢�źű����ַ
* wxNickNameAddr���ǳƱ����ַ
* wxRemarkAddr����ע�����ַ
* WxFriendStructW��Ĭ�Ϲ��캯��
*/
struct WxFriendStruct
{
    DWORD wxIdAddr;
    DWORD wxNumberAddr;
    DWORD wxNickNameAddr;
    DWORD wxRemarkAddr;
    WxFriendStruct(DWORD wxIdAddr, DWORD wxNumberAddr, DWORD wxNickNameAddr, DWORD wxRemarkAddr)
    {
        this->wxIdAddr = wxIdAddr;
        this->wxNumberAddr = wxNumberAddr;
        this->wxNickNameAddr = wxNickNameAddr;
        this->wxRemarkAddr = wxRemarkAddr;
    }
};
