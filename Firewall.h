/* ============================================================
 *
 *  █████╗ ██╗███╗   ███╗██████╗  ██████╗ ████████╗     ██╗  ██╗ █████╗ ██╗  ██╗███████╗
 * ██╔══██╗██║████╗ ████║██╔══██╗██╔═══██╗╚══██╔══╝    ██║  ██║██╔══██╗╚██╗██╔╝╚══███╔╝
 * ███████║██║██╔████╔██║██████╔╝██║   ██║   ██║       ███████║███████║ ╚███╔╝   ███╔╝ 
 * ██╔══██║██║██║╚██╔╝██║██╔══██╗██║   ██║   ██║       ██╔══██║██╔══██║ ██╔██╗  ███╔╝  
 * ██║  ██║██║██║ ╚═╝ ██║██████╔╝╚██████╔╝   ██║       ██║  ██║██║  ██║██╔╝ ██╗███████╗
 * ╚═╝  ╚═╝╚═╝╚═╝     ╚═╝╚═════╝  ╚═════╝    ╚═╝       ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝
 *
 *

 * ============================================================ */  
#ifndef OMEGA_FIREWALL_DEEP_H
#define OMEGA_FIREWALL_DEEP_H

#include <jni.h>
#include <string>
#include <vector>
#include <mutex>
#include <map>
#include <set>
#include <ctime>
#include <dlfcn.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <arpa/inet.h>
#include <android/log.h>
#include <pthread.h>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <fcntl.h>

#define LOG_TAG "OmegaFW"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)

// Default ඔක්කොම True කරලා තියෙන්නේ Anti-Ban එක උපරිම වෙන්න
bool g_fwEnabled = true;
bool g_fwVerbose = false; // අනවශ්‍ය සාමාන්‍ය ලොග්ස් වදින්නේ නෑ (Memory ඉතුරුයි)
bool g_fwDeepScan = true;
bool g_fwBlockAntiCheat = true;
bool g_fwBlockMemoryScan = true;

struct FWLogEntry {
    std::string time;
    std::string lib;
    std::string target;
    std::string port;
    bool blocked;
    std::string reason;
};

struct LibInfo {
    std::string name;
    uintptr_t base;
    size_t size;
    bool scanned;
};

struct AntiCheatPattern {
    const char* name;
    const char* pattern;
    const char* mask;
    size_t length;
};

static std::vector<FWLogEntry> g_fwLogs;
static std::mutex g_fwLogMutex;
static std::map<std::string, LibInfo> g_loadedLibs;
static std::mutex g_libMutex;
static const int MAX_LOGS = 200; // 500 සිට 200 දක්වා අඩු කරා Lag එක නැති කරන්න

// Free Fire වල Security එක තියෙන ප්‍රධාන ලිබ්ස් ටික විතරක් ඉතුරු කරා
static const char* FF_ANTICHEAT_LIBS[] = {
    "libanogs.so",
    "libGGP.so",
    "libthor_utils.so",
    nullptr
};

// Anti-Cheat එකෙන් අපේ Mod එක Scan කරන්න ගන්න Memory Patterns (Anti-Ban එකට වැදගත්ම කොටස)
static const AntiCheatPattern AC_PATTERNS[] = {
    {"anti_debug", "\x68\x00\x00\x80\x52\xc0\x03\x5f\xd6", "x????xxx", 9},
    {"ptrace_check", "\x20\x00\x80\xd2\xc0\x03\x5f\xd6", "xxxxxxxx", 8},
    {"anti_tamper", "\x00\x00\x80\x52\xc0\x03\x5f\xd6", "xxxxxxxx", 8},
    {"integrity_check", "\xff\x43\x00\xd1\xc0\x03\x5f\xd6", "xxxxxxxx", 8},
    {"debugger_detect", "\xe0\x03\x00\xaa\xc0\x03\x5f\xd6", "xxxxxxxx", 8},
    {"frida_check", "\x00\x00\x80\xd2\xe0\x03\x00\xaa", "xxxxxxxx", 8},
    {"root_check", "\x01\x00\x80\x52\xc0\x03\x5f\xd6", "xxxxxxxx", 8},
    {nullptr, nullptr, nullptr, 0}
};

// මේ Domains වලින් තමා උන් Ban Report සහ Security Logs සර්වර් එකට යවන්නේ. ඒක නිසා මේවා අනිවාර්යයෙන්ම බ්ලොක් වෙන්න ඕනේ
static const char* BLOCKED_DOMAINS[] = {
    "gamesecurity.us.freefiremobile.com",
    "gamesecurity.sea.freefiremobile.com",
    "device-check.garena.com",
    "anti-cheat.garena.com",
    "anticheat.garena.com",
    "anti.garena.com",
    "guard.garena.com",
    "ff-anticheat.garena.com",
    "ff-report.garena.com",
    "ff-antihack.garena.com",
    "ff-security.garena.com",
    "report.freefiremobile.com",
    "anticheat.freefiremobile.com",
    "security.freefiremobile.com",
    "ffban.garena.com",
    "ban.garena.com",
    "crashlytics.com",
    "app-measurement.com",
    nullptr
};

static std::string fw_get_time() {
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    char buf[32];
    strftime(buf, sizeof(buf), "%H:%M:%S", t);
    return buf;
}

static void fw_add_log(const char* lib, const char* target, int port, bool blocked, const char* reason) {
    if (!g_fwVerbose && !blocked) return;
    std::lock_guard<std::mutex> lock(g_fwLogMutex);
    FWLogEntry e;
    e.time = fw_get_time();
    e.lib = (lib && *lib) ? lib : "Unknown";
    e.target = target ? target : "N/A";
    e.port = (port > 0) ? std::to_string(port) : "DNS";
    e.blocked = blocked;
    e.reason = reason ? reason : "";
    g_fwLogs.push_back(e);
    if (g_fwLogs.size() > MAX_LOGS) g_fwLogs.erase(g_fwLogs.begin());
    if (blocked) LOGW("[BLOCK] %s -> %s:%d [%s]", e.lib.c_str(), e.target.c_str(), port, reason);
}

static bool fw_get_caller_lib(char* out, size_t sz) {
    void* addr = __builtin_return_address(0);
    Dl_info info;
    if (dladdr(addr, &info) && info.dli_fname) {
        const char* p = strrchr(info.dli_fname, '/');
        strncpy(out, p ? p + 1 : info.dli_fname, sz - 1);
        out[sz - 1] = 0;
        return true;
    }
    strcpy(out, "Unknown");
    return false;
}

static bool fw_is_ff_anticheat_lib(const char* lib) {
    for (int i = 0; FF_ANTICHEAT_LIBS[i]; i++) {
        if (strcmp(lib, FF_ANTICHEAT_LIBS[i]) == 0) return true;
    }
    return false;
}

static bool fw_is_blocked_domain(const char* domain) {
    if (!domain) return false;
    for (int i = 0; BLOCKED_DOMAINS[i]; i++) {
        if (strstr(domain, BLOCKED_DOMAINS[i]) != nullptr) return true;
    }
    return false;
}

static bool fw_pattern_match(uint8_t* data, size_t len, const AntiCheatPattern* pat) {
    if (len < pat->length) return false;
    for (size_t i = 0; i <= len - pat->length; i++) {
        bool match = true;
        for (size_t j = 0; j < pat->length; j++) {
            if (pat->mask[j] == 'x') {
                if (data[i + j] != (uint8_t)pat->pattern[j]) {
                    match = false;
                    break;
                }
            }
        }
        if (match) return true;
    }
    return false;
}

static void fw_scan_lib_memory(const char* libName, uintptr_t base, size_t size) {
    if (!g_fwDeepScan || !g_fwBlockAntiCheat || size == 0) return;
    if (!fw_is_ff_anticheat_lib(libName)) return;
    
    uint8_t* mem = (uint8_t*)base;
    size_t scanSize = std::min(size, (size_t)0x80000); // Max 512KB scan (Speed එක වැඩි කරා)
    
    for (int i = 0; AC_PATTERNS[i].name != nullptr; i++) {
        if (fw_pattern_match(mem, scanSize, &AC_PATTERNS[i])) {
            char buf[256];
            snprintf(buf, sizeof(buf), "ANTI-CHEAT: %s", AC_PATTERNS[i].name);
            fw_add_log(libName, buf, 0, true, "MEMORY BYPASS");
        }
    }
}

// Hooks
static void* (*orig_dlopen)(const char*, int);
static void* hook_dlopen(const char* name, int flags) {
    void* result = orig_dlopen(name, flags);
    if (name && result && g_fwEnabled) {
        const char* p = strrchr(name, '/');
        const char* shortName = p ? p + 1 : name;
        
        if (fw_is_ff_anticheat_lib(shortName)) {
            std::lock_guard<std::mutex> lock(g_libMutex);
            LibInfo info;
            info.name = shortName;
            info.base = (uintptr_t)result;
            info.size = 0; 
            info.scanned = false;
            g_loadedLibs[shortName] = info;
        }
    }
    return result;
}

static void* (*orig_dlsym)(void*, const char*);
static void* hook_dlsym(void* handle, const char* symbol) {
    void* result = orig_dlsym(handle, symbol);
    if (symbol && result && g_fwEnabled && g_fwBlockAntiCheat) {
        if (strstr(symbol, "ptrace") || strstr(symbol, "debug") ||
            strstr(symbol, "anti") || strstr(symbol, "check") ||
            strstr(symbol, "integrity") || strstr(symbol, "tamper")) {
            
            Dl_info info;
            if (dladdr(result, &info) && info.dli_fname) {
                const char* p = strrchr(info.dli_fname, '/');
                const char* lib = p ? p + 1 : info.dli_fname;
                if (fw_is_ff_anticheat_lib(lib)) {
                    fw_add_log(lib, symbol, 0, true, "DISABLED SECURITY FUNCTION");
                    return nullptr; // Security එක හොයන Function එක Disable කරනවා (Anti-Ban!)
                }
            }
        }
    }
    return result;
}

static void* (*orig_mmap)(void*, size_t, int, int, int, off_t);
static void* hook_mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset) {
    if (g_fwEnabled && g_fwBlockMemoryScan && (prot & PROT_EXEC) && (prot & PROT_WRITE)) {
        char lib[256];
        fw_get_caller_lib(lib, sizeof(lib));
        if (fw_is_ff_anticheat_lib(lib)) {
            fw_add_log(lib, "mmap RWX Scan", length, true, "BLOCKED MEMORY SCAN");
            return MAP_FAILED;
        }
    }
    return orig_mmap(addr, length, prot, flags, fd, offset);
}

static int (*orig_mprotect)(void*, size_t, int);
static int hook_mprotect(void* addr, size_t len, int prot) {
    if (g_fwEnabled && g_fwBlockMemoryScan && (prot & PROT_EXEC) && (prot & PROT_WRITE)) {
        char lib[256];
        fw_get_caller_lib(lib, sizeof(lib));
        if (fw_is_ff_anticheat_lib(lib)) {
            fw_add_log(lib, "mprotect RWX Scan", len, true, "BLOCKED MPROTECT SCAN");
            errno = EPERM;
            return -1;
        }
    }
    return orig_mprotect(addr, len, prot);
}

static int (*orig_getaddrinfo)(const char*, const char*, const struct addrinfo*, struct addrinfo**);
static int hook_getaddrinfo(const char* node, const char* svc, const struct addrinfo* hints, struct addrinfo** res) {
    if (g_fwEnabled && node) {
        char lib[256];
        fw_get_caller_lib(lib, sizeof(lib));
        
        // Garena එකට Ban Reports යවන එක මුලින්ම Block කරනවා
        if (fw_is_blocked_domain(node)) {
            fw_add_log(lib, node, 0, true, "BAN REPORT BLOCKED");
            return EAI_NONAME; 
        }
    }
    return orig_getaddrinfo(node, svc, hints, res);
}

static void* fw_scanner_thread(void*) {
    while (g_fwEnabled) {
        sleep(7); // Scanning Interval එක වැඩි කරා Lag එක අඩු කරන්න
        if (!g_fwDeepScan) continue;
        
        std::lock_guard<std::mutex> lock(g_libMutex);
        for (auto& pair : g_loadedLibs) {
            if (!pair.second.scanned) {
                FILE* fp = fopen("/proc/self/maps", "r");
                if (fp) {
                    char line[512];
                    while (fgets(line, sizeof(line), fp)) {
                        if (strstr(line, pair.first.c_str())) {
                            uintptr_t start, end;
                            sscanf(line, "%lx-%lx", &start, &end);
                            pair.second.size = end - start;
                            break;
                        }
                    }
                    fclose(fp);
                }
                fw_scan_lib_memory(pair.first.c_str(), pair.second.base, pair.second.size);
                pair.second.scanned = true;
            }
        }
    }
    return nullptr;
}

static bool fw_install() {
    void* libc = dlopen("libc.so", RTLD_NOW);
    if (libc) {
        void* p2 = dlsym(libc, "getaddrinfo");
        void* p3 = dlsym(libc, "mmap");
        void* p4 = dlsym(libc, "mprotect");
        
        if (p2) DobbyHook(p2, (void*)hook_getaddrinfo, (void**)&orig_getaddrinfo);
        if (p3) DobbyHook(p3, (void*)hook_mmap, (void**)&orig_mmap);
        if (p4) DobbyHook(p4, (void*)hook_mprotect, (void**)&orig_mprotect);
        dlclose(libc);
    }
    
    void* linker = dlopen("libdl.so", RTLD_NOW);
    if (!linker) linker = dlopen("libc.so", RTLD_NOW);
    if (linker) {
        void* p5 = dlsym(linker, "dlopen");
        void* p6 = dlsym(linker, "dlsym");
        
        if (p5) DobbyHook(p5, (void*)hook_dlopen, (void**)&orig_dlopen);
        if (p6) DobbyHook(p6, (void*)hook_dlsym, (void**)&orig_dlsym);
        dlclose(linker);
    }
    
    pthread_t tid;
    pthread_create(&tid, nullptr, fw_scanner_thread, nullptr);
    pthread_detach(tid);
    return true;
}

static void fw_enable(bool en) { g_fwEnabled = en; }
static void fw_deep_scan(bool en) { g_fwDeepScan = en; }
static std::vector<FWLogEntry> fw_get_logs() { std::lock_guard<std::mutex> l(g_fwLogMutex); return g_fwLogs; }
static void fw_clear_logs() { std::lock_guard<std::mutex> l(g_fwLogMutex); g_fwLogs.clear(); }

#endif

