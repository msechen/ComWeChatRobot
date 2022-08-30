#pragma once
#ifdef USE_SOCKET
#include <signal.h>
// mongoose: https://github.com/cesanta/mongoose
#include "mongoose/mongoose.h"
#pragma comment(lib, "ws2_32.lib")
extern "C" __declspec(dllexport) void http_start(int port);
extern "C" __declspec(dllexport) int http_close();

string getMgStrA(struct mg_str str);
string getMgVarA(mg_http_message *hm, string name);
bool is_digit_number(string str);

typedef enum HTTP_METHODSTag
{
    HTTP_METHOD_GET = 1,
    HTTP_METHOD_POST
} HTTP_METHODS,
    *PHTTP_METHODS;

typedef enum WECHAT_HTTP_APISTag
{
    // login check
    WECHAT_IS_LOGIN = 0,
    // self info
    WECHAT_GET_SELF_INFO,
    // send message
    WECHAT_MSG_SEND_TEXT,
    WECHAT_MSG_SEND_AT,
    WECHAT_MSG_SEND_CARD,
    WECHAT_MSG_SEND_IMAGE,
    WECHAT_MSG_SEND_FILE,
    WECHAT_MSG_SEND_ARTICLE,
    WECHAT_MSG_SEND_APP,
    // receive message
    WECHAT_MSG_START_HOOK,
    WECHAT_MSG_STOP_HOOK,
    WECHAT_MSG_START_IMAGE_HOOK,
    WECHAT_MSG_STOP_IMAGE_HOOK,
    WECHAT_MSG_START_VOICE_HOOK,
    WECHAT_MSG_STOP_VOICE_HOOK,
    // contact
    WECHAT_CONTACT_GET_LIST,
    WECHAT_CONTACT_CHECK_STATUS,
    WECHAT_CONTACT_DEL,
    WECHAT_CONTACT_SEARCH_BY_CACHE,
    WECHAT_CONTACT_SEARCH_BY_NET,
    WECHAT_CONTACT_ADD_BY_WXID,
    WECHAT_CONTACT_ADD_BY_V3,
    WECHAT_CONTACT_ADD_BY_PUBLIC_ID,
    WECHAT_CONTACT_VERIFY_APPLY,
    WECHAT_CONTACT_EDIT_REMARK,
    // chatroom
    WECHAT_CHATROOM_GET_MEMBER_LIST,
    WECHAT_CHATROOM_GET_MEMBER_NICKNAME,
    WECHAT_CHATROOM_DEL_MEMBER,
    WECHAT_CHATROOM_ADD_MEMBER,
    WECHAT_CHATROOM_SET_ANNOUNCEMENT,
    WECHAT_CHATROOM_SET_CHATROOM_NAME,
    WECHAT_CHATROOM_SET_SELF_NICKNAME,
    // database
    WECHAT_DATABASE_GET_HANDLES,
    WECHAT_DATABASE_BACKUP,
    WECHAT_DATABASE_QUERY,
    // version
    WECHAT_SET_VERSION,
    // log
    WECHAT_LOG_START_HOOK,
    WECHAT_LOG_STOP_HOOK,
} WECHAT_HTTP_APIS,
    *PWECHAT_HTTP_APIS;
#endif
