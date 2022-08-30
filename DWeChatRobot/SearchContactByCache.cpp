#include "pch.h"
#include <typeinfo>
#include <string>
#include <vector>
#include <map>
#include "json/json.hpp"
using namespace nlohmann;

// ��ȡ������ϢCALL1ƫ��
#define GetUserInfoCall1Offset 0x100BD5C0 - 0x10000000
// ��ȡ������ϢCALL2ƫ��
#define GetUserInfoCall2Offset 0x10771980 - 0x10000000
// ��ȡ������ϢCALL3ƫ��
#define GetUserInfoCall3Offset 0x104662A0 - 0x10000000
// ���������Ϣ�������
#define DeleteUserInfoCacheCall1Offset 0x106C52B0 - 0x10000000
// ���������Ϣ����CALL2
#define DeleteUserInfoCacheCall2Offset 0x100BE6D0 - 0x10000000

/*
* �ⲿ����ʱ�ķ�������
* message��wUserInfo.c_str()
* length��wUserInfo�ַ�������
*/
#ifndef USE_SOCKET
struct GetUserInfoStruct
{
    DWORD message;
    DWORD length;
} ret;
#endif

/*
* ���ݻ���������ƴ�Ӻ�����Ϣ
* address����������ַ
* return��void
*/
static wstring WxUserInfo(DWORD address)
{
    json jData;
    map<string, DWORD> key_addr_map;
    key_addr_map["wxId"] = address + 0x10;
    key_addr_map["wxNumber"] = address + 0x24;
    key_addr_map["wxV3"] = address + 0x38;
    key_addr_map["wxRemark"] = address + 0x58;
    key_addr_map["wxNickName"] = address + 0x6C;
    key_addr_map["wxBigAvatar"] = address + 0xFC;
    key_addr_map["wxSmallAvatar"] = address + 0x110;
    key_addr_map["wxSignature"] = address + 0x19C;
    key_addr_map["wxNation"] = address + 0x1B0;
    key_addr_map["wxProvince"] = address + 0x1C4;
    key_addr_map["wxCity"] = address + 0x1D8;
    key_addr_map["wxBackground"] = address + 0x27C;
    for (auto it = key_addr_map.begin(); it != key_addr_map.end(); it++)
    {
        string key = it->first;
        DWORD addr = it->second;
        wstring wstemp = ((*((DWORD *)addr)) != 0) ? (wstring)(WCHAR *)(*((LPVOID *)addr)) : L"null";
        string value = unicode_to_utf8((wchar_t *)wstemp.c_str());
        jData[key] = value;
    }
    wstring wUserInfo = utf8_to_unicode(jData.dump().c_str());
    return wUserInfo;
}

/*
* ���ⲿ���õĻ�ȡ������Ϣ�ӿ�
* lparamter���������wxid�ĵ�ַ
* return��DWORD��`ret`���׵�ַ
*/
#ifndef USE_SOCKET
DWORD GetWxUserInfoRemote(LPVOID lparamter)
{
    wchar_t *userwxid = (wchar_t *)lparamter;

    wstring wUserInfo = GetUserInfoByWxId(userwxid);

    ZeroMemory(&ret, sizeof(GetUserInfoStruct));
    wchar_t *message = new wchar_t[wUserInfo.length() + 1];
    memcpy(message, wUserInfo.c_str(), (wUserInfo.length() + 1) * 2);
    ret.message = (DWORD)message;
    ret.length = wUserInfo.length();
    return (DWORD)&ret;
}
#endif
/*
* ���ⲿ���õ���պ�����Ϣ����Ľӿ�
* return��void
*/
#ifndef USE_SOCKET
VOID DeleteUserInfoCacheRemote()
{
    if (ret.length)
    {
        delete[](wchar_t *) ret.message;
        ZeroMemory(&ret, sizeof(GetUserInfoStruct));
    }
}
#endif

/*
* ����wxid��ȡ������Ϣ�ľ���ʵ��
* wxid������wxid
* return��wstring���ɹ����غ�����Ϣ��ʧ�ܷ��ؿ��ַ���
*/
wstring __stdcall GetUserInfoByWxId(wchar_t *wxid)
{
    wstring wUserInfo = L"";
    DWORD WeChatWinBase = GetWeChatWinBase();
    DWORD WxGetUserInfoCall1 = WeChatWinBase + GetUserInfoCall1Offset;
    DWORD WxGetUserInfoCall2 = WeChatWinBase + GetUserInfoCall2Offset;
    DWORD WxGetUserInfoCall3 = WeChatWinBase + GetUserInfoCall3Offset;
    DWORD DeleteUserInfoCacheCall1 = WeChatWinBase + DeleteUserInfoCacheCall1Offset;
    DWORD DeleteUserInfoCacheCall2 = WeChatWinBase + DeleteUserInfoCacheCall2Offset;
    char buffer[0x3FC] = {0};
    WxString pWxid(wxid);
    DWORD address = 0;
    DWORD isSuccess = 0;
    __asm
    {
		pushad;
		call WxGetUserInfoCall1;
		lea ebx, buffer;
		push ebx;
		sub esp, 0x14;
		mov esi, eax;
		lea eax, pWxid;
		mov ecx, esp;
		push eax;
		call WxGetUserInfoCall2;
		mov ecx, esi;
		call WxGetUserInfoCall3;
		mov isSuccess, eax;
		mov address, ebx;
		popad;
    }
    if (isSuccess)
        wUserInfo = WxUserInfo(address);

    char deletebuffer[0x410] = {0};
    __asm {
		pushad;
		lea ecx, deletebuffer;
		call DeleteUserInfoCacheCall1;
		push eax;
		lea ebx,buffer;
		mov ecx, ebx;
		call DeleteUserInfoCacheCall2;
		popad;
    }
    return wUserInfo;
}

/*
* ����wxid��ȡ��ϵ���ǳƣ���Ҫ���ڷ��Ͱ�����Ϣ�ӿ�
* wxid����ϵ��wxid
* return��wchar_t*����ȡ����wxid
*/
wstring __stdcall GetUserNickNameByWxId(wchar_t *wxid)
{
    wstring wstr;
    DWORD WeChatWinBase = GetWeChatWinBase();
    DWORD WxGetUserInfoCall1 = WeChatWinBase + GetUserInfoCall1Offset;
    DWORD WxGetUserInfoCall2 = WeChatWinBase + GetUserInfoCall2Offset;
    DWORD WxGetUserInfoCall3 = WeChatWinBase + GetUserInfoCall3Offset;
    DWORD DeleteUserInfoCacheCall1 = WeChatWinBase + DeleteUserInfoCacheCall1Offset;
    DWORD DeleteUserInfoCacheCall2 = WeChatWinBase + DeleteUserInfoCacheCall2Offset;
    char buffer[0x3FC] = {0};
    WxString pWxid(wxid);
    DWORD address = 0;
    DWORD isSuccess = 0;
    __asm
    {
		pushad;
		call WxGetUserInfoCall1;
		lea ebx, buffer;
		push ebx;
		sub esp, 0x14;
		mov esi, eax;
		lea eax, pWxid;
		mov ecx, esp;
		push eax;
		call WxGetUserInfoCall2;
		mov ecx, esi;
		call WxGetUserInfoCall3;
		mov isSuccess, eax;
		mov address, ebx;
		popad;
    }
    if (isSuccess)
    {
        DWORD length = *(DWORD *)(address + 0x6C + 0x4);
        wchar_t *buffer = new wchar_t[length + 1];
        ZeroMemory(buffer, (length + 1) * 2);
        memcpy(buffer, (wchar_t *)(*(DWORD *)(address + 0x6C)), length * 2);
        wstr = wstring(buffer);
        delete[] buffer;
        buffer = NULL;
    }
    char deletebuffer[0x410] = {0};
    __asm {
		pushad;
		lea ecx, deletebuffer;
		call DeleteUserInfoCacheCall1;
		push eax;
		lea ebx, buffer;
		mov ecx, ebx;
		call DeleteUserInfoCacheCall2;
		popad;
    }
    return wstr;
}
