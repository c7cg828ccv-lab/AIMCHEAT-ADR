///  PRABIR LIVE BYPASS PVT  ////
 
 /*
MemoryPatch::createWithHex("libil2cpp.so",0x4f3d4,"00 00 A0 E3 1E FF 2F E1",8).Modify();
MemoryPatch::createWithHex("libil2cpp.so",0x4f368,"00 00 A0 E3 1E FF 2F E1",8).Modify();
MemoryPatch::createWithHex("libil2cpp.so",0x4f2c0,"00 00 A0 E3 1E FF 2F E1",8).Modify();
MemoryPatch::createWithHex("libil2cpp.so",0x4f278,"00 00 A0 E3 1E FF 2F E1",8).Modify();
MemoryPatch::createWithHex("libil2cpp.so",0x4f284,"00 00 A0 E3 1E FF 2F E1",8).Modify();
MemoryPatch::createWithHex("libil2cpp.so",0x4f1d0,"00 00 A0 E3 1E FF 2F E1",8).Modify();
MemoryPatch::createWithHex("libil2cpp.so",0x4f164,"00 00 A0 E3 1E FF 2F E1",8).Modify();
MemoryPatch::createWithHex("libil2cpp.so",0x4f0ec,"00 00 A0 E3 1E FF 2F E1",8).Modify();
MemoryPatch::createWithHex("libil2cpp.so",0x4efd8,"00 00 A0 E3 1E FF 2F E1",8).Modify();
MemoryPatch::createWithHex("libil2cpp.so",0x4f014,"00 00 A0 E3 1E FF 2F E1",8).Modify();
MemoryPatch::createWithHex("libil2cpp.so",0x4ef9c,"00 00 A0 E3 1E FF 2F E1",8).Modify();
MemoryPatch::createWithHex("libil2cpp.so",0x4efc0,"00 00 A0 E3 1E FF 2F E1",8).Modify();
MemoryPatch::createWithHex("libil2cpp.so",0x4f20c,"00 00 A0 E3 1E FF 2F E1",8).Modify();
MemoryPatch::createWithHex("libil2cpp.so",0x4f434,"00 00 A0 E3 1E FF 2F E1",8).Modify();
MemoryPatch::createWithHex("libil2cpp.so",0x4f5cc,"00 00 A0 E3 1E FF 2F E1",8).Modify();
MemoryPatch::createWithHex("libil2cpp.so",0x4ef24,"00 00 A0 E3 1E FF 2F E1",8).Modify();
MemoryPatch::createWithHex("libil2cpp.so",0x4ef00,"00 00 A0 E3 1E FF 2F E1",8).Modify();
MemoryPatch::createWithHex("libil2cpp.so",0x4ee10,"00 00 A0 E3 1E FF 2F E1",8).Modify();
MemoryPatch::createWithHex("libil2cpp.so",0x5b4b4,"00 00 A0 E3 1E FF 2F E1",8).Modify();
MemoryPatch::createWithHex("libil2cpp.so",0x5b4e8,"00 00 A0 E3 1E FF 2F E1",8).Modify();
MemoryPatch::createWithHex("libil2cpp.so",0x5b4b8,"00 00 A0 E3 1E FF 2F E1",8).Modify();
MemoryPatch::createWithHex("libil2cpp.so",0x5b4b0,"00 00 A0 E3 1E FF 2F E1",8).Modify();
MemoryPatch::createWithHex("libil2cpp.so",0x5b4ac,"00 00 A0 E3 1E FF 2F E1",8).Modify();
MemoryPatch::createWithHex("libil2cpp.so",0x5b4c0,"00 00 A0 E3 1E FF 2F E1",8).Modify();
MemoryPatch::createWithHex("libil2cpp.so",0x5b474,"00 00 A0 E3 1E FF 2F E1",8).Modify();
MemoryPatch::createWithHex("libil2cpp.so",0x5b4a8,"00 00 A0 E3 1E FF 2F E1",8).Modify();

*/


using namespace std;

int32_t (*get_SystemPlatform)();
int32_t _get_SystemPlatform() {
    return (int32_t)2;
}

int32_t (*get_ChannelType)();
int32_t _get_ChannelType() {
    return (int32_t)0;
}

int32_t (*get_Architecture)();
int32_t _get_Architecture() {
    return (int32_t)3;
}

bool enableAntiReport = true;
bool enableCmdLogger = false;
bool showLogWindow = false;

std::mutex logMutex;
std::deque<std::string> cmdLogs;

const std::vector<uint32_t> BLOCKED_MSG_IDS = {
    431,453,1026,1079,1364,642,254,351,352,812,939,
    737,284,455,456,1047,1049,1051,1052,572,2067
};

std::set<uint32_t> blockedMsgIDs = {
    431,453,1026,1079,1364,642,812,939,254,351,352,737,
    455,456,1047,1049,1051,1052,572,2067
};

std::set<uint8_t> blockedCmds = {
    29,  // FFANTI - Main anti-cheat
    34,  // CREDITSCORE - Penalty/report system  
    35,  // GAMESERVERMANAGER - Server validation
    38,  // CUSTOMEVENT - Anti-cheat events
    40,  // GIN - GGP anti-cheat
    42,  // CSRANKINGMATCH - Ranked validation
    43,  // BRRANKINGMATCH - Ranked validation
    50,  // REMATCH - Rematch punishment sync
	51, // Garena ki maa ki chut Ggp boleto Google Play
};

bool (*orig_MatchGameClient_Send)(void* self, uint32_t messageID, void* msg, uint8_t sendOption, bool cacheMsgAnyWay) = nullptr;
bool (*orig_NetworkDetectionClient_Send)(void* self, uint32_t messageID, void* msg, uint8_t sendOption, bool cacheMsgAnyWay) = nullptr;
bool (*orig_ServiceClient2_Send)(void* self, uint8_t cmd, void* data, int32_t byteCount, uint8_t region) = nullptr;
bool (*orig_EGL_SwapBuffers)(void* dpy, void* surface) = nullptr;

typedef void* (*OnSplittedDatagram_t)(void *instance, void *splittedMsg, void **bigMsg);
OnSplittedDatagram_t orig_MatchGameClient_OnSplittedDatagram = nullptr;
OnSplittedDatagram_t orig_MobileMatchGameClient_OnSplittedDatagram = nullptr;

enum Proto : int {
    INIT        = 1,
    HEARTBEAT   = 2,
    MATCHMAKING = 3,
    ROOM        = 14,
    RECONNECTION = 17,
    HEARTBEATRANDOMMIN = 151,
    HEARTBEATRANDOMMAX = 255
};

typedef bool (*SendMessageToLobby_t)(void* thiz, int cmdType, uint32_t subCmdType, void* message, uint8_t regionID, bool ignoreDisconnectWnd);
SendMessageToLobby_t orig_SendMessageToLobby = nullptr;

void (*old_UDPSession_Send)(void* instance, uint32_t cmd, void* data, int32_t byteCount, uint8_t sendOption);
int (*orig_GGP_Init)(bool is_low_mem, int client_type, int64_t update_flag);


void AddLog(const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);
    cmdLogs.push_back(message);
    if (cmdLogs.size() > 200) cmdLogs.pop_front();
}

bool hk_MatchGameClient_Send(void* instance, uint32_t messageID, void* msg, uint8_t sendOption, bool cacheMsgAnyWay) {
    if (enableAntiReport && blockedMsgIDs.find(messageID) != blockedMsgIDs.end()) {
        AddLog("Blocked MSG: " + std::to_string(messageID));
        return false;
    }
    
    if (orig_MatchGameClient_Send) {
        return orig_MatchGameClient_Send(instance, messageID, msg, sendOption, cacheMsgAnyWay);
    }
    return false;
}

bool hook_NetworkDetectionClient_Send(void* self, uint32_t messageID, void* msg, uint8_t sendOption, bool cacheMsgAnyWay) {
    if (enableAntiReport && blockedMsgIDs.find(messageID) != blockedMsgIDs.end()) {
        AddLog("Blocked Network MSG: " + std::to_string(messageID));
        return false;
    }
    
    if (orig_NetworkDetectionClient_Send) {
        return orig_NetworkDetectionClient_Send(self, messageID, msg, sendOption, cacheMsgAnyWay);
    }
    return false;
}

bool hook_ServiceClient2_Send(void* self, uint8_t cmd, void* data, int32_t byteCount, uint8_t region) {
    if (enableAntiReport && blockedCmds.find(cmd) != blockedCmds.end()) {
        AddLog("Blocked CMD: " + std::to_string(cmd));
        return true;
    }
    
    if (orig_ServiceClient2_Send) {
        return orig_ServiceClient2_Send(self, cmd, data, byteCount, region);
    }
    return false;
}

bool (*orig_DCKLGOGDPCH_CIAFBJBMNKO)(const char* KPJJHNBFGBF, void* HGLMIKCDHFN) = nullptr;
bool hook_DCKLGOGDPCH_CIAFBJBMNKO(const char* KPJJHNBFGBF, void* HGLMIKCDHFN) {
    return true;
}

void (*orig_DCKLGOGDPCH_MPOLNCNGCIJ)(const char* APKJPKBCCGD, bool OAKKFNNNAOP) = nullptr;
void hook_DCKLGOGDPCH_MPOLNCNGCIJ(const char* APKJPKBCCGD, bool OAKKFNNNAOP) {
}

void (*orig_DCKLGOGDPCH_CCNEAFOPMIH)(const char* DJKDKPMGKGH, void* CLFFPBABFLP) = nullptr;
void hook_DCKLGOGDPCH_CCNEAFOPMIH(const char* DJKDKPMGKGH, void* CLFFPBABFLP) {
}

bool (*orig_DCKLGOGDPCH_GKCOOPMPOAD)() = nullptr;
bool hook_DCKLGOGDPCH_GKCOOPMPOAD() {
    return false;
}

void (*orig_DCKLGOGDPCH_OFNLFFJMJCO)(const char* HINJBPEDIAL, void* HPDHGHGKMAI, uint32_t AFFNEFKFLFA, uint32_t EOEMAGIFLJM) = nullptr;
void hook_DCKLGOGDPCH_OFNLFFJMJCO(const char* HINJBPEDIAL, void* HPDHGHGKMAI, uint32_t AFFNEFKFLFA, uint32_t EOEMAGIFLJM) {
}

void* hook_OnSplittedDatagram(void *instance, void *splittedMsg, void **bigMsg) {
    uint32_t cmd = *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(splittedMsg) + 0x18);
    
    if (blockedMsgIDs.find(cmd) != blockedMsgIDs.end()) {
        *bigMsg = nullptr;
        return nullptr;
    }
    
    if (orig_MatchGameClient_OnSplittedDatagram) {
        return orig_MatchGameClient_OnSplittedDatagram(instance, splittedMsg, bigMsg);
    } else if (orig_MobileMatchGameClient_OnSplittedDatagram) {
        return orig_MobileMatchGameClient_OnSplittedDatagram(instance, splittedMsg, bigMsg);
    }
    return nullptr;
}

bool Hook_SendMessageToLobby(void* thiz, int cmdType, uint32_t subCmdType, void* message, uint8_t regionID, bool ignoreDisconnectWnd) {
    if (enableAntiReport) {
        if (blockedCmds.find(cmdType) != blockedCmds.end()) {
            return true;
        }
        
        if (cmdType == INIT || cmdType == HEARTBEAT || cmdType == RECONNECTION || 
            (cmdType >= HEARTBEATRANDOMMIN && cmdType <= HEARTBEATRANDOMMAX)) {
            return orig_SendMessageToLobby(thiz, cmdType, subCmdType, message, regionID, ignoreDisconnectWnd);
        }

        if (cmdType == ROOM || cmdType == MATCHMAKING) {
            return orig_SendMessageToLobby(thiz, cmdType, subCmdType, message, regionID, ignoreDisconnectWnd);
        }

        return true;
    }
    
    return orig_SendMessageToLobby(thiz, cmdType, subCmdType, message, regionID, ignoreDisconnectWnd);
}

void UDPSession_Send_Hook(void* instance, uint32_t cmd, void* data, int32_t byteCount, uint8_t sendOption) {
    if (blockedMsgIDs.find(cmd) != blockedMsgIDs.end()) {
        return;
    }
    return old_UDPSession_Send(instance, cmd, data, byteCount, sendOption);
}





void (*orig_SendGameLagLog)(void*);
void (*orig_SendGameMemoryLog)(void*);
void (*orig_StartLogGameLag)(void*);
void (*orig_LogReportCheat)(void*, uint64_t, uint32_t, void*, uint32_t*, bool, uint32_t, int32_t, monoString*, int32_t);
void (*orig_LogReportCheatInHistory)(void*, uint64_t, uint32_t, void*, uint64_t, uint32_t*, uint32_t, monoString*);
void (*orig_LogBRMatchResult)(void*, uint32_t, uint32_t, int32_t, uint32_t, uint32_t, int32_t);
void (*orig_SendUGCOfflineMatchStats)(void*, void*, void*, void*);
void (*orig_SendRematchFinishLog)(void*);


void (*orig_ProcessBugReport)(void *instance, void *loginRes);
void hook_ProcessBugReport(void *instance, void *loginRes) {
    return;
}
/*

void (*orig_ProcessffantihackOpen)(void *instance, void *mtpSwitchDesc);
void hook_ProcessffantihackOpen(void *instance, void *mtpSwitchDesc) {
    return;
}*/

bool (*orig_GetffantihackEnabled)(void *instance);
bool hook_GetffantihackEnabled(void *instance) {
    return false;
}

void (*orig_ProcessffantihackGGP)(void *instance, void *loginRes);
void hook_ProcessffantihackGGP(void *instance, void *loginRes) {
    return;
}

bool (*orig_GetffantihackEnabledTempValue)(void* _this, void* mtpSwitchDesc);
bool my_GetffantihackEnabledTempValue(void* _this, void* mtpSwitchDesc) {
    if (mtpSwitchDesc) {
        *(void**)((uintptr_t)mtpSwitchDesc + 0x10) = U3DStr("IND");
        *(bool*)((uintptr_t)mtpSwitchDesc + 0x18) = false;
        *(bool*)((uintptr_t)mtpSwitchDesc + 0x19) = false;
        *(bool*)((uintptr_t)mtpSwitchDesc + 0x1a) = false;
        *(bool*)((uintptr_t)mtpSwitchDesc + 0x1b) = false;
        *(bool*)((uintptr_t)mtpSwitchDesc + 0x1c) = false;
        *(bool*)((uintptr_t)mtpSwitchDesc + 0x1d) = false;
    }
    return false;
}

void (*orig_ProcessffantihackOpen)(void* _this, void* mtpSwitchDesc);
void my_ProcessffantihackOpen(void* _this, void* mtpSwitchDesc) {
    if (mtpSwitchDesc) {
        *(void**)((uintptr_t)mtpSwitchDesc + 0x10) = U3DStr("IND");
        *(bool*)((uintptr_t)mtpSwitchDesc + 0x18) = false;
        *(bool*)((uintptr_t)mtpSwitchDesc + 0x19) = false;
        *(bool*)((uintptr_t)mtpSwitchDesc + 0x1a) = false;
        *(bool*)((uintptr_t)mtpSwitchDesc + 0x1b) = false;
        *(bool*)((uintptr_t)mtpSwitchDesc + 0x1c) = false;
        *(bool*)((uintptr_t)mtpSwitchDesc + 0x1d) = false;
    }
    if (orig_ProcessffantihackOpen) {
        orig_ProcessffantihackOpen(_this, mtpSwitchDesc);
    }
}



void hook_SendGameLagLog(void* _this) { return; }
void hook_SendGameMemoryLog(void* _this) { return; }
void hook_StartLogGameLag(void* _this) { return; }
void hook_LogReportCheat(void* _this, uint64_t a, uint32_t b, void* c, uint32_t* d, bool e, uint32_t f, int32_t g, monoString* h, int32_t i) { return; }
void hook_LogReportCheatInHistory(void* _this, uint64_t a, uint32_t b, void* c, uint64_t d, uint32_t* e, uint32_t f, monoString* g) { return; }
void hook_LogBRMatchResult(void* _this, uint32_t a, uint32_t b, int32_t c, uint32_t d, uint32_t e, int32_t f) { return; }
void hook_SendUGCOfflineMatchStats(void* _this, void* a, void* b, void* c) { return; }
void hook_SendRematchFinishLog(void* _this) { return; }
