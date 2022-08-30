#include "pch.h"

// ����ͼƬCALL1ƫ��
#define SendImageCall1Offset (0x7875D780 - 0x786A0000)
// ����ͼƬCALL2ƫ��
#define SendImageCall2Offset (0x78E11980 - 0x786A0000)
// ����ͼƬCALL3ƫ��
#define SendImageCall3Offset (0x78BC1640 - 0x786A0000)
// ��ջ����CALLƫ��
#define DeleteSendImageCacheCallOffset (0x78757780 - 0x786A0000)

/*
 * �ⲿ����ʱ���ݵĲ����ṹ
 * wxid������wxid�ĵ�ַ
 * imagepath������ͼƬ����·���ĵ�ַ
 */
#ifndef USE_SOCKET
struct ImageParamStruct
{
    DWORD wxid;
    DWORD imagepath;
};
#endif

/*
 * ���ⲿ���õķ���ͼƬ��Ϣ�ӿ�
 * lpParamStruct��ImageParamStruct���ͽṹ��ָ��
 * return��void
 */
#ifndef USE_SOCKET
void SendImageRemote(LPVOID lpParamStruct)
{
    ImageParamStruct *params = (ImageParamStruct *)lpParamStruct;
    SendImage((WCHAR *)params->wxid, (WCHAR *)params->imagepath);
}
#endif

/*
 * ����ͼƬ��Ϣ�ľ���ʵ��
 * receiver��������wxid
 * ImagePath��ͼƬ����·��
 * return��BOOL�����ͳɹ�����`1`������ʧ�ܷ���`0`
 */
BOOL __stdcall SendImage(wchar_t *receiver, wchar_t *ImagePath)
{
    DWORD WeChatWinBase = GetWeChatWinBase();
    DWORD SendImageCall1 = WeChatWinBase + SendImageCall1Offset;
    DWORD SendImageCall2 = WeChatWinBase + SendImageCall2Offset;
    DWORD SendImageCall3 = WeChatWinBase + SendImageCall3Offset;
    DWORD DeleteSendImageCacheCall = WeChatWinBase + DeleteSendImageCacheCallOffset;
    char nullbuffer[0x50] = {0};
    char buffer[0x3B0] = {0};
    WxString pReceiver(receiver);
    WxString pImagePath(ImagePath);
    WxString nullStruct = {0};
    DWORD tempeax = 0;
    int isSuccess = 0;
    __asm {
		pushad;
		call SendImageCall1;
		sub esp, 0x14;
		mov tempeax, eax;
		lea eax, nullStruct;
		mov ecx, esp;
		lea edi, pImagePath;
		push eax;
		call SendImageCall2;
		mov ecx, dword ptr [tempeax] ;
		lea eax, pReceiver;
		push edi;
		push eax;
		lea eax, buffer;
		push eax;
		call SendImageCall3;
		mov isSuccess,eax;
		lea ecx, buffer;
		call DeleteSendImageCacheCall;
		popad;
    }
    return isSuccess == 1;
}
