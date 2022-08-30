#include "pch.h"

// �����ı���Ϣ��CALLƫ��
#define SendTextCallOffset 0x78BC1D30 - 0x786A0000
// ��ջ����CALLƫ��
#define DeleteTextCacheCallOffset 0x78757780 - 0x786A0000

/*
 * �ⲿ����ʱ���ݵĲ����ṹ
 * wxid��wxid�����ַ
 * wxmsg�����͵����ݱ����ַ
 */
#ifndef USE_SOCKET
struct SendTextStruct
{
    DWORD wxid;
    DWORD wxmsg;
};
#endif

/*
 * ���ⲿ���õķ����ı���Ϣ�ӿ�
 * lpParameter��SendTextStruct���ͽṹ��ָ��
 * return��void
 */
#ifndef USE_SOCKET
void SendTextRemote(LPVOID lpParameter)
{
    SendTextStruct *rp = (SendTextStruct *)lpParameter;
    wchar_t *wsWxId = (WCHAR *)rp->wxid;
    wchar_t *wsTextMsg = (WCHAR *)rp->wxmsg;
    SendText(wsWxId, wsTextMsg);
}
#endif

/*
 * �����ı���Ϣ�ľ���ʵ��
 * wsWxId��������wxid
 * wsTextMsg�����͵���Ϣ����
 * return��BOOL�����ͳɹ�����`1`������ʧ�ܷ���`0`
 */
BOOL __stdcall SendText(wchar_t *wsWxId, wchar_t *wsTextMsg)
{
    WxString wxWxid(wsWxId);
    WxString wxTextMsg(wsTextMsg);
    wchar_t **pWxmsg = &wxTextMsg.buffer;
    char buffer[0x3B0] = {0};

    WxString wxNull = {0};
    DWORD dllBaseAddress = GetWeChatWinBase();
    DWORD callAddress = dllBaseAddress + SendTextCallOffset;
    DWORD DeleteTextCacheCall = dllBaseAddress + DeleteTextCacheCallOffset;
    int isSuccess = 0;
    __asm {
        pushad;
        lea eax, wxNull;
        push 0x1;
        push eax;
        mov edi, pWxmsg;
        push edi;
        lea edx, wxWxid;
        lea ecx, buffer;
        call callAddress;
        mov isSuccess,eax;
        add esp, 0xC;
        lea ecx, buffer;
        call DeleteTextCacheCall;
        popad;
    }
    return isSuccess == 1;
}
