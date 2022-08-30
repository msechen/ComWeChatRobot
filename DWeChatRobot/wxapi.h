#pragma once
#include "SendImage.h"
#include "SendText.h"
#include "SendFile.h"
#include "SendArticle.h"
#include "FriendList.h"
#include "SearchContact.h"
#include "SelfInfo.h"
#include "SendCard.h"
#include "CheckFriendStatus.h"
#include "LogMsgInfo.h"
#include "ReceiveMessage.h"
#include "SendAtText.h"
#include "GetChatRoomMembers.h"
#include "GetDbHandles.h"
#include "DbExecuteSql.h"
#include "DbBackup.h"
#include "VerifyFriendApply.h"
#include "AddFriend.h"
#include "sqlite3.h"
#include "wechatver.h"
#include "DeleteUser.h"
#include "SendAppMsg.h"
#include "EditRemark.h"
#include "DelChatRoomMember.h"
#include "AddChatRoomMember.h"
#include "SetChatRoomAnnouncement.h"
#include "SetChatRoomSelfNickname.h"
#include "SetChatRoomName.h"
#include "GetChatRoomMemberNickname.h"

using namespace std;
#pragma comment(lib, "version.lib")
#pragma warning(disable : 4731 26812)
// ���ڵ�����������Ҫʹ�ô˺�����
#define DLLEXPORT extern "C" __declspec(dllexport)

BOOL CreateConsole(void);
DWORD GetWeChatWinBase();
string unicode_to_gb2312(wchar_t *wchar);
string utf8_to_gb2312(const char *strUTF8);
string gb2312_to_utf8(const char *strGB2312);
string unicode_to_utf8(wchar_t *wstr);
wstring utf8_to_unicode(const char *buffer);
void HookAnyAddress(DWORD dwHookAddr, LPVOID dwJmpAddress, char *originalRecieveCode);
void UnHookAnyAddress(DWORD dwHookAddr, char *originalRecieveCode);
DLLEXPORT void UnHookAll();
wstring wreplace(wstring source, wchar_t replaced, wstring replaceto);
void PrintProcAddr();
wstring GetTimeW(long long timestamp);
BOOL ProcessIsWeChat();
BOOL FindOrCreateDirectory(const wchar_t *pszPath);

template <typename T1, typename T2>
vector<T1> split(T1 str, T2 letter)
{
    vector<T1> arr;
    size_t pos;
    while ((pos = str.find_first_of(letter)) != T1::npos)
    {
        T1 str1 = str.substr(0, pos);
        arr.push_back(str1);
        str = str.substr(pos + 1, str.length() - pos - 1);
    }
    arr.push_back(str);
    return arr;
}
