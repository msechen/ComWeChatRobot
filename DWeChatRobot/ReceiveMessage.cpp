#include "pch.h"
#include <vector>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <map>
#include "json/json.hpp"
using namespace nlohmann;

#pragma comment(lib, "ws2_32.lib")

using namespace std;

#define CLTIP "127.0.0.1"

// 接收消息的HOOK地址偏移
#define ReceiveMessageHookOffset 0x5D39C359 - 0x5D1F0000
// 接收消息HOOK的CALL偏移
#define ReceiveMessageNextCallOffset 0x5D5F7F00 - 0x5D1F0000

// 发送消息的HOOK地址偏移
#define SendMessageHookOffset 0x78B88E42 - 0x786A0000
// 发送消息HOOK的CALL偏移
#define SendMessageNextCallOffset 0x78AA8170 - 0x786A0000

// 撤回消息的HOOK地址偏移
#define RevokeMessageHookOffset 0x5D6D8A4C - 0x5D1F0000
// 撤回消息HOOK的CALL偏移
#define RevokeMessageNextCallOffset 0x5D6D3430 - 0x5D1F0000

#define READ_WSTRING(addr, offset) wstring((wchar_t *)(*(DWORD *)(addr + offset)), *(DWORD *)(addr + offset + 0x4))

static int SRVPORT = 0;

// 是否开启接收消息HOOK标志
BOOL ReceiveMessageHooked = false;
// 保存HOOK前的字节码，用于恢复
static char OldReceiveMessageAsmCode[5] = {0};
static char OldSendMessageAsmCode[5] = {0};
static char OldRevokeMessageAsmCode[5] = {0};
static DWORD WeChatWinBase = GetWeChatWinBase();
// 接收消息HOOK地址
static DWORD ReceiveMessageHookAddress = WeChatWinBase + ReceiveMessageHookOffset;
// 接收消息HOOK的CALL地址
static DWORD ReceiveMessageNextCall = WeChatWinBase + ReceiveMessageNextCallOffset;
// 接收HOOK的跳转地址
static DWORD ReceiveMessageJmpBackAddress = ReceiveMessageHookAddress + 0x5;
// 发送消息HOOK地址
static DWORD SendMessageHookAddress = WeChatWinBase + SendMessageHookOffset;
// 发送消息HOOK的CALL地址
static DWORD SendMessageNextCall = WeChatWinBase + SendMessageNextCallOffset;
// 发送HOOK的跳转地址
static DWORD SendMessageJmpBackAddress = SendMessageHookAddress + 0x5;
// 撤回消息HOOK地址
static DWORD RevokeMessageHookAddress = WeChatWinBase + RevokeMessageHookOffset;
// 撤回消息HOOK的CALL地址
static DWORD RevokeMessageNextCall = WeChatWinBase + RevokeMessageNextCallOffset;
// 撤回HOOK的跳转地址
static DWORD RevokeMessageJmpBackAddress = RevokeMessageHookAddress + 0x5;

struct SocketMessageStruct
{
    char *buffer;
    int length;
    ~SocketMessageStruct()
    {
        if (this->buffer != NULL)
        {
            delete[] this->buffer;
            this->buffer = NULL;
        }
    }
};

// 通过socket将消息发送给服务端
BOOL SendSocketMessage(const char *buffer, size_t len)
{
    if (SRVPORT == 0)
    {
        return false;
    }
    SOCKET clientsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientsocket < 0)
    {
#ifdef _DEBUG
        cout << "create socket error,"
             << " errno:" << errno << endl;
#endif
        return false;
    }
    BOOL status = false;
    sockaddr_in clientAddr;
    memset(&clientAddr, 0, sizeof(clientAddr));
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons((u_short)SRVPORT);
    InetPtonA(AF_INET, CLTIP, &clientAddr.sin_addr.s_addr);

    if (connect(clientsocket, reinterpret_cast<sockaddr *>(&clientAddr), sizeof(sockaddr)) < 0)
    {
#ifdef _DEBUG
        cout << "connect error,"
             << " errno:" << errno << endl;
#endif
        return false;
    }
    char recvbuf[1024] = {0};
    int ret = send(clientsocket, buffer, len, 0);
    if (ret == -1 || ret == 0)
    {
#ifdef _DEBUG
        cout << "send fail,"
             << " errno:" << errno << endl;
#endif
        closesocket(clientsocket);
        return false;
    }
    memset(recvbuf, 0, sizeof(recvbuf));
    ret = recv(clientsocket, recvbuf, sizeof(recvbuf), 0);
    closesocket(clientsocket);
    if (ret == -1 || ret == 0)
    {
#ifdef _DEBUG
        cout << "the server close" << endl;
#endif
        return false;
    }
    return true;
}

void SendSocketMessageInThread(SocketMessageStruct *param)
{
    if (param == NULL)
        return;
    unique_ptr<SocketMessageStruct> sms(param);
    string jstr(param->buffer, param->length);
#ifdef USE_COM
    // 通过连接点，将消息广播给客户端；将广播过程放在线程中完成，客户端才可以等待图片、语音落地
    VARIANT vsaValue = (_variant_t)utf8_to_unicode(jstr.c_str()).c_str();
    json jMsg = json::parse(param->buffer, param->buffer + param->length, nullptr, false);
    if (jMsg.is_discarded() != true)
    {
        DWORD type = jMsg["type"].get<DWORD>();
        ULONG64 msgid = (type != 10000) ? jMsg["msgid"].get<ULONG64>() : 0;
        PostComMessage(jMsg["pid"].get<int>(), WX_MESSAGE, msgid, &vsaValue);
    }
#endif
    SendSocketMessage(jstr.c_str(), jstr.size());
}

static void dealMessage(DWORD messageAddr)
{
    json jMsg;
    unsigned long long msgid = *(unsigned long long *)(messageAddr + 0x30);
    jMsg["pid"] = GetCurrentProcessId();
    jMsg["type"] = *(DWORD *)(messageAddr + 0x38);
    jMsg["isSendMsg"] = *(BOOL *)(messageAddr + 0x3C);
    jMsg["msgid"] = msgid;
    jMsg["localId"] = *(unsigned int *)(messageAddr + 0x20);
    jMsg["sender"] = unicode_to_utf8((wchar_t *)READ_WSTRING(messageAddr, 0x48).c_str());
    int length = *(DWORD *)(messageAddr + 0x170 + 0x4);
    jMsg["wxid"] = length == 0 ? jMsg["sender"].get<std::string>() : unicode_to_utf8((wchar_t *)READ_WSTRING(messageAddr, 0x170).c_str());
    jMsg["message"] = unicode_to_utf8((wchar_t *)READ_WSTRING(messageAddr, 0x70).c_str());
    if (jMsg["type"].get<int>() != 10000)
    {
        jMsg["filepath"] = unicode_to_utf8((wchar_t *)READ_WSTRING(messageAddr, 0x1AC).c_str());
        string extrabuf = base64_encode((BYTE *)(*(DWORD *)(messageAddr + 0x8C)), *(DWORD *)(messageAddr + 0x8C + 0x4));
        jMsg["extrainfo"] = extrabuf;
    }
    else
    {
        // 为尊重他人隐私，不提供已撤回消息的原有信息
        jMsg["extrainfo"] = "";
    }
    jMsg["time"] = unicode_to_utf8((wchar_t *)GetTimeW(*(DWORD *)(messageAddr + 0x44)).c_str());
    jMsg["self"] = unicode_to_utf8((wchar_t *)GetSelfWxid().c_str());
    string jstr = jMsg.dump() + '\n';
    // 为保证线程安全，需要手动管理内存
    SocketMessageStruct *sms = new SocketMessageStruct;
    sms->buffer = new char[jstr.size() + 1];
    memcpy(sms->buffer, jstr.c_str(), jstr.size() + 1);
    sms->length = jstr.size();
    HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SendSocketMessageInThread, sms, NULL, 0);
    if (hThread)
    {
        CloseHandle(hThread);
    }
}

/*
 * 消息处理函数，根据消息缓冲区组装结构并存入容器
 * messageAddr：保存消息的缓冲区地址
 * return：void
 */
// VOID ReceiveMessage(DWORD messagesAddr)
//{
//     DWORD *messages = (DWORD *)messagesAddr;
//     for (DWORD messageAddr = messages[0]; messageAddr < messages[1]; messageAddr += 0x298)
//     {
//         dealMessage(messageAddr);
//     }
// }

/*
 * HOOK的具体实现，接收到消息后调用处理函数
 */
_declspec(naked) void dealReceiveMessage()
{
    __asm {
		pushad;
		pushfd;
		push edi;
		call dealMessage;
		add esp, 0x4;
		popfd;
		popad;
		call ReceiveMessageNextCall;
		jmp ReceiveMessageJmpBackAddress;
    }
}

/*
 * HOOK的具体实现，发送消息后调用处理函数
 */
_declspec(naked) void dealSendMessage()
{
    __asm {
		pushad;
		pushfd;
		push edi;
		call dealMessage;
		add esp, 0x4;
		popfd;
		popad;
		call SendMessageNextCall;
		jmp SendMessageJmpBackAddress;
    }
}

/*
 * HOOK的具体实现，接收到撤回消息后调用处理函数
 */
_declspec(naked) void dealRevokeMessage()
{
    __asm {
		pushad;
		pushfd;
		push edi;
		call dealMessage;
		add esp, 0x4;
		popfd;
		popad;
		call RevokeMessageNextCall;
		jmp RevokeMessageJmpBackAddress;
    }
}

/*
 * 开始接收消息HOOK
 * return：void
 */
VOID HookReceiveMessage(int port)
{
    SRVPORT = port;
    WeChatWinBase = GetWeChatWinBase();
    if (ReceiveMessageHooked || !WeChatWinBase)
        return;
    ReceiveMessageHookAddress = WeChatWinBase + ReceiveMessageHookOffset;
    ReceiveMessageNextCall = WeChatWinBase + ReceiveMessageNextCallOffset;
    ReceiveMessageJmpBackAddress = ReceiveMessageHookAddress + 0x5;
    SendMessageHookAddress = WeChatWinBase + SendMessageHookOffset;
    SendMessageNextCall = WeChatWinBase + SendMessageNextCallOffset;
    SendMessageJmpBackAddress = SendMessageHookAddress + 0x5;
    RevokeMessageHookAddress = WeChatWinBase + RevokeMessageHookOffset;
    RevokeMessageNextCall = WeChatWinBase + RevokeMessageNextCallOffset;
    RevokeMessageJmpBackAddress = RevokeMessageHookAddress + 0x5;
    HookAnyAddress(ReceiveMessageHookAddress, (LPVOID)dealReceiveMessage, OldReceiveMessageAsmCode);
    HookAnyAddress(SendMessageHookAddress, (LPVOID)dealSendMessage, OldSendMessageAsmCode);
    HookAnyAddress(RevokeMessageHookAddress, (LPVOID)dealRevokeMessage, OldRevokeMessageAsmCode);
    ReceiveMessageHooked = TRUE;
}

/*
 * 停止接收消息HOOK
 * return：void
 */
VOID UnHookReceiveMessage()
{
    SRVPORT = 0;
    if (!ReceiveMessageHooked)
        return;
    UnHookAnyAddress(ReceiveMessageHookAddress, OldReceiveMessageAsmCode);
    UnHookAnyAddress(SendMessageHookAddress, OldSendMessageAsmCode);
    UnHookAnyAddress(RevokeMessageHookAddress, OldRevokeMessageAsmCode);
    ReceiveMessageHooked = FALSE;
}
