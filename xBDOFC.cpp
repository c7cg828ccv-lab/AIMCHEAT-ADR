/*
===============================[ 𝗠𝗔𝗗𝗘 𝗕𝗬 @𝗕𝗗_𝗛𝗔𝗖𝗞𝗘𝗥_𝗢𝗙𝗙𝗜𝗖𝗜𝗔𝗟_𝗣𝗥𝗜𝗩𝗔𝗧𝗘_𝗦𝗥𝗖 ]============================
===============================[ 𝗠𝗔𝗗𝗘 𝗕𝗬 @𝗕𝗗_𝗛𝗔𝗖𝗞𝗘𝗥_𝗢𝗙𝗙𝗜𝗖𝗜𝗔𝗟_𝗣𝗥𝗜𝗩𝗔𝗧𝗘_𝗦𝗥𝗖 ]============================
===============================[ 𝗠𝗔𝗗𝗘 𝗕𝗬 @𝗕𝗗_𝗛𝗔𝗖𝗞𝗘𝗥_𝗢𝗙𝗙𝗜𝗖𝗜𝗔𝗟_𝗣𝗥𝗜𝗩𝗔𝗧𝗘_𝗦𝗥𝗖 ]============================
*/

#include <android/log.h>
#include <unistd.h>
#include <thread>
#include <limits>
#include <cmath>
#include <xdl.h>
#include <KittyUtils.h>
#include <KittyMemory.h>
#include <Il2Cpp.h>
#include <SubstrateHook.h>
#include <CydiaSubstrate.h>
#include "Struct/Gui.hpp"
#include <Struct/main.h>
#include "fonts/FontAwesome6_solid.h"
#include "ImGui/Toggle.h"
#include "zygisk.hpp"
#include "Chams.h"
#include "AimSilentOb55.h"
#include "KittyMemory/MemoryPatch.h"
#include "Bypass.h"
#include "Firewall.h"
#include "logo.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void hack_thread();

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    jvm = vm;
    std::thread(hack_thread).detach();
    return JNI_VERSION_1_6;
}

uintptr_t il2cpp_base = 0;
void *getRealAddr(ulong offset) {
    return reinterpret_cast<void*>(il2cpp_base + offset);
};

bool initImGui = false;
bool showMenu = true; 
bool CloseGui = false;
static ImVec2 tapPos(30.0f, 120.0f);
static GLuint CustomLogoTexture = 0;
static bool CustomLogoLoaded = false;

// লোগো টেক্সচার লোড করার ফাংশন
static void LoadCustomLogoTexture() {
    if (CustomLogoLoaded) return;
    int width = 0, height = 0, channels = 0;
    stbi_uc* pixels = stbi_load_from_memory(
        (const stbi_uc*)logo_data, (int)logo_data_len,
        &width, &height, &channels, 4);
    if (!pixels) return;

    glGenTextures(1, &CustomLogoTexture);
    glBindTexture(GL_TEXTURE_2D, CustomLogoTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(pixels);
    CustomLogoLoaded = (CustomLogoTexture != 0);
}

// পার্মানেন্ট উজ্জ্বল পিঙ্ক থিম ও স্টাইল সেটআপ
void ApplyPinkTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.04f, 0.07f, 0.98f);
    colors[ImGuiCol_Border] = ImVec4(1.0f, 0.23f, 0.62f, 1.0f); 
    colors[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.06f, 0.10f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.12f, 0.06f, 0.10f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(1.0f, 0.23f, 0.62f, 0.8f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(1.0f, 0.35f, 0.75f, 1.0f);
    colors[ImGuiCol_HeaderActive] = ImVec4(1.0f, 0.23f, 0.62f, 1.0f);
    
    colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 0.23f, 0.62f, 1.0f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.08f, 0.14f, 1.0f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.12f, 0.22f, 1.0f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(1.0f, 0.23f, 0.62f, 1.0f);

    style.WindowRounding = 12.0f;
    style.FrameRounding = 22.0f;
    style.WindowBorderSize = 2.0f;
}

struct FloatingToggle {
    const char* label;
    bool*       value;
    bool        show;
    float       x, y;
    bool        isDragging;
    ImVec2      dragOffset;
};

static FloatingToggle g_FloatingToggles[] = {
    { "SpeedHack", &SpeedHack, false, 80.0f, 420.0f, false, ImVec2(0,0) },
};

static int g_FloatingToggleCount = sizeof(g_FloatingToggles) / sizeof(g_FloatingToggles[0]);

void DrawFloatingToggles() {
    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* draw = ImGui::GetForegroundDrawList();

    static int activeDragIndex = -1;

    for (int i = 0; i < g_FloatingToggleCount; i++) {
        FloatingToggle& t = g_FloatingToggles[i];

        if (!t.show) continue;

        const float TOGGLE_W = 110.0f;
        const float TOGGLE_H = 42.0f;
        const float KNOB_R   = 15.0f;
        const float PAD      = 6.0f;

        ImVec2 pos(t.x, t.y);
        ImVec2 end(pos.x + TOGGLE_W, pos.y + TOGGLE_H);

        bool hovered = io.MousePos.x >= pos.x && io.MousePos.x <= end.x &&
                       io.MousePos.y >= pos.y && io.MousePos.y <= end.y;

        if (hovered && activeDragIndex == -1 && ImGui::IsMouseClicked(0)) {
            t.isDragging = true;
            activeDragIndex = i;
            t.dragOffset = ImVec2(io.MousePos.x - t.x, io.MousePos.y - t.y);
        }
        if (t.isDragging && activeDragIndex == i) {
            if (ImGui::IsMouseDown(0)) {
                t.x = io.MousePos.x - t.dragOffset.x;
                t.y = io.MousePos.y - t.dragOffset.y;
                pos = ImVec2(t.x, t.y);
                end = ImVec2(pos.x + TOGGLE_W, pos.y + TOGGLE_H);
            } else {
                t.isDragging = false;
                activeDragIndex = -1;
            }
        }

        if (hovered && ImGui::IsMouseReleased(0) && !t.isDragging) {
            *t.value = !(*t.value);
        }

        ImU32 bgCol   = *t.value ? IM_COL32(255, 60, 160, 255) : IM_COL32(45, 15, 35, 220);
        ImU32 knobCol = IM_COL32(255, 255, 255, 255);
        ImU32 textCol = IM_COL32(255, 255, 255, 255);

        draw->AddRectFilled(ImVec2(pos.x + 2, pos.y + 3), ImVec2(end.x + 2, end.y + 3),
                            IM_COL32(0, 0, 0, 90), TOGGLE_H * 0.5f);

        draw->AddRectFilled(pos, end, bgCol, TOGGLE_H * 0.5f);

        if (hovered)
            draw->AddRect(pos, end, IM_COL32(255, 255, 255, 180), TOGGLE_H * 0.5f, 0, 2.0f);

        float knobX = *t.value ? (end.x - KNOB_R - PAD) : (pos.x + KNOB_R + PAD);
        float knobY = pos.y + TOGGLE_H * 0.5f;

        draw->AddCircleFilled(ImVec2(knobX, knobY), KNOB_R + 1.5f, IM_COL32(0, 0, 0, 60));
        draw->AddCircleFilled(ImVec2(knobX, knobY), KNOB_R, knobCol);

        ImVec2 textSize = ImGui::CalcTextSize(t.label);
        ImVec2 textPos(pos.x + (TOGGLE_W - textSize.x) * 0.5f, pos.y - textSize.y - 2.0f);

        draw->AddText(ImVec2(textPos.x + 1, textPos.y + 1), IM_COL32(0, 0, 0, 180), t.label);
        draw->AddText(textPos, textCol, t.label);
    }
}

inline EGLBoolean (*old_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);
inline EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    eglQuerySurface(dpy, surface, EGL_WIDTH, &g_GlWidth);
    eglQuerySurface(dpy, surface, EGL_HEIGHT, &g_GlHeight);
    if (!g_IsSetup) {
        prevWidth = g_GlWidth;
        prevHeight = g_GlHeight;
        SetupImgui();
        ApplyPinkTheme();
        g_IsSetup = true;
    }

    static bool isTouching = false;
    static float touchStartTime = 0.0f;
    ImGuiIO& io = ImGui::GetIO(); 
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplAndroid_NewFrame(g_GlWidth, g_GlHeight);
    ImGui::NewFrame();
    
    int touchCount = ((int (*)())(Class_Input__get_touchCount))();
    if (touchCount > 0) {
        UnityEngine_Touch_Fields touch = ((UnityEngine_Touch_Fields(*)(int))(Class_Input__GetTouch))(0);
        float reverseY = io.DisplaySize.y - touch.m_Position.fields.y;
        io.MousePos = ImVec2(touch.m_Position.fields.x, reverseY);
        if (!isTouching) {
            touchStartTime = ImGui::GetTime();
        }
        io.MouseDown[0] = true;
        isTouching = true;
    } else {
        if (isTouching && (ImGui::GetTime() - touchStartTime < 0.06f)) {
            io.MouseDown[0] = true;
        } else {
            io.MouseDown[0] = false;
            isTouching = false;
        }  
    }
    
    DrawESP(g_GlWidth, g_GlHeight);
    DrawFloatingToggles();

    if (showMenu && !CloseGui) {
        ImGui::SetNextWindowSize(ImVec2(380, 440), ImGuiCond_Always);
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

        ImGui::Begin("BDHackerMenu", nullptr, windowFlags);

        // --- ফিক্সড টাইটেল বক্স ---
        {
            ImVec2 windowSize = ImGui::GetWindowSize();
            const char* titleText = "BD HACKER OFFICIAL V1 SAFE";
            ImVec2 textSize = ImGui::CalcTextSize(titleText);
            
            float boxWidth = windowSize.x - 20.0f;
            float boxHeight = 36.0f;
            
            ImGui::SetCursorPos(ImVec2(10.0f, 12.0f));
            ImVec2 cursorP = ImGui::GetCursorScreenPos();
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            
            drawList->AddRectFilled(cursorP, ImVec2(cursorP.x + boxWidth, cursorP.y + boxHeight), IM_COL32(25, 12, 22, 255), 6.0f);
            drawList->AddRect(cursorP, ImVec2(cursorP.x + boxWidth, cursorP.y + boxHeight), IM_COL32(255, 60, 160, 255), 6.0f, 0, 2.0f);
            
            float textX = cursorP.x + (boxWidth - textSize.x) * 0.5f;
            float textY = cursorP.y + (boxHeight - textSize.y) * 0.5f;
            drawList->AddText(ImVec2(textX, textY), IM_COL32(255, 255, 255, 255), titleText);
            
            ImGui::SetCursorPosY(12.0f + boxHeight + 10.0f);
        }

        // --- স্ক্রোলিং সেকশন ---
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, 305), false, ImGuiWindowFlags_None);

        auto BDOFCCustomButton = [](const char* label, bool* v) {
            ImVec2 p = ImGui::GetCursorScreenPos();
            ImVec2 sz = ImVec2(ImGui::GetContentRegionAvail().x, 50.0f);
            
            ImGui::InvisibleButton(label, sz);
            bool hovered = ImGui::IsItemHovered();
            bool clicked = ImGui::IsItemClicked();

            if (clicked) {
                *v = !(*v);
            }

            ImDrawList* drawList = ImGui::GetWindowDrawList();
            
            ImU32 bgCol = *v ? IM_COL32(255, 60, 160, 255) : IM_COL32(45, 15, 35, 220);
            if (hovered) {
                bgCol = *v ? IM_COL32(255, 90, 180, 255) : IM_COL32(65, 25, 48, 220);
            }

            drawList->AddRectFilled(p, ImVec2(p.x + sz.x, p.y + sz.y), bgCol, 8.0f);
            drawList->AddRect(p, ImVec2(p.x + sz.x, p.y + sz.y), IM_COL32(255, 100, 190, 255), 8.0f, 0, 1.5f);

            char buf[64];
            snprintf(buf, sizeof(buf), "%s : %s", label, *v ? "ON" : "OFF");
            
            ImVec2 textSize = ImGui::CalcTextSize(buf);
            ImVec2 textPos(p.x + (sz.x - textSize.x) * 0.5f, p.y + (sz.y - textSize.y) * 0.5f);

            drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), buf);

            ImGui::Spacing();
            ImGui::Spacing();
        };

        BDOFCCustomButton("SILENT AIM GOD", &AimSilentBDOFC);
        BDOFCCustomButton("ESP LOCATION", &Enable);
        BDOFCCustomButton("CHAMS GOD", &chams);
        BDOFCCustomButton("SPEED HACK", &g_FloatingToggles[0].show);
        BDOFCCustomButton("LONG JUMP", &JumpHight);
        BDOFCCustomButton("RESET ID", &Reset);

        ImGui::EndChild();

        ImGui::Spacing();

        // ক্লোজ বাটন
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.85f, 0.32f, 0.55f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.95f, 0.40f, 0.65f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.75f, 0.25f, 0.45f, 1.0f));
        
        if (ImGui::Button("CLOSE", ImVec2(-1, 40))) {
            CloseGui = true;
            tapPos = ImGui::GetWindowPos();
        }
        ImGui::PopStyleColor(3);

        ImGui::End();
    }

    if (CloseGui) {
        LoadCustomLogoTexture();
        const ImVec2 logoWindowSize(100.0f, 100.0f);
        ImGui::SetNextWindowPos(tapPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(logoWindowSize, ImGuiCond_Always);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::SetNextWindowBgAlpha(0.0f);
        ImGui::Begin("##TapToOpenLogo", nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);
            
        ImDrawList* logoDraw = ImGui::GetWindowDrawList();
        ImVec2 logoWinPos = ImGui::GetWindowPos();
        const float logoRadius = logoWindowSize.x * 0.5f;
        ImVec2 logoCenter(logoWinPos.x + logoRadius, logoWinPos.y + logoRadius);
        
        logoDraw->AddCircleFilled(logoCenter, logoRadius, IM_COL32(14, 10, 22, 245), 48);
        logoDraw->AddCircle(logoCenter, logoRadius, IM_COL32(255, 60, 160, 255), 48, 2.5f);
        
        if (CustomLogoLoaded && CustomLogoTexture) {
            logoDraw->AddImageRounded((ImTextureID)(intptr_t)CustomLogoTexture,
                ImVec2(logoCenter.x - logoRadius + 3.0f, logoCenter.y - logoRadius + 3.0f),
                ImVec2(logoCenter.x + logoRadius - 3.0f, logoCenter.y + logoRadius - 3.0f),
                ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 255), logoRadius);
        } else {
            logoDraw->AddText(ImVec2(logoCenter.x - 14.0f, logoCenter.y - 8.0f),
                IM_COL32(255, 255, 255, 255), "BD");
        }
        
        ImGui::InvisibleButton("##LogoTapDrag", logoWindowSize);
        if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 1.0f)) {
            ImVec2 delta = ImGui::GetIO().MouseDelta;
            tapPos.x += delta.x;
            tapPos.y += delta.y;
        }
        if (ImGui::IsItemDeactivated() && !ImGui::IsMouseDragging(ImGuiMouseButton_Left, 4.0f)) {
            CloseGui = false;
            showMenu = true;
        }
        ImGui::End();
        ImGui::PopStyleVar(2);
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    return old_eglSwapBuffers(dpy, surface);
}

inline void StartGUI() {
    void *ptr_eglSwapBuffer = DobbySymbolResolver("/system/lib/libEGL.so", "eglSwapBuffers");
    if (ptr_eglSwapBuffer != nullptr) {
        DobbyHook((void *)ptr_eglSwapBuffer, (void *)hook_eglSwapBuffers, (void **)&old_eglSwapBuffers);
        LOGD("GUI started successfully");
    }
}

ProcMap unityMap, anogsMap, il2cppMap;

void hack_thread() {
    while (!unityMap.isValid()) {
        unityMap = KittyMemory::getLibraryBaseMap("libunity.so");
        anogsMap = KittyMemory::getLibraryBaseMap("libanogs.so");
        il2cppMap = KittyMemory::getLibraryBaseMap("libil2cpp.so");
    sleep(12);
    }
    pid_t pid = getpid();
    LOGD("Hack Thread initiated for PID: %i", pid);

    do {
        il2cpp_base = get_module_base(pid, "libil2cpp.so");
        sleep(1);
    } while (il2cpp_base == 0);

    LOGD("libil2cpp.so found at address: 0x%" PRIxPTR, il2cpp_base);
    
    Il2CppAttach();
    InitSpeedHackHook();
    InitFastReloadHackHook();
    LOGD("Initializing AimSilent hook...");
  InitAimSilentHook();
  LOGD("AimSilent hook installed");

       if (mlovinit()) {
        setShader("_AlphaMask");
        LogShaders();
        Wallhack();
    }

    void* resetGuestAddr = Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW"), OBFUSCATE("GameConfig"), OBFUSCATE("get_ResetGuest"), 0);
    if (resetGuestAddr) {
        DobbyHook(resetGuestAddr, (void *) ResetGuest, (void **) &_ResetGuest);
    }

    void* maxJumpAddr = Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"), OBFUSCATE("Player"), OBFUSCATE("get_MaxJumpHeight"), 0);
    if (maxJumpAddr) {
        DobbyHook(maxJumpAddr, (void*)my_get_MaxJumpHeight, (void**)&orig_get_MaxJumpHeight);
    }

    void* customGravAddr = Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"), OBFUSCATE("Player"), OBFUSCATE("get_CustomGravity"), 0);
    if (customGravAddr) {
        DobbyHook(customGravAddr, (void*)my_get_CustomGravity, (void**)&orig_get_CustomGravity);
    }

    void* risingGravAddr = Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"), OBFUSCATE("Player"), OBFUSCATE("get_RisingGravity"), 0);
    if (risingGravAddr) {
        DobbyHook(risingGravAddr, (void*)my_get_RisingGravity, (void**)&orig_get_RisingGravity);
    }

    void* fallingGravAddr = Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"), OBFUSCATE("Player"), OBFUSCATE("get_FallingGravity"), 0);
    if (fallingGravAddr) {
        DobbyHook(fallingGravAddr, (void*)my_get_FallingGravity, (void**)&orig_get_FallingGravity);
    }
    
    StartGUI();
}
/*
===============================[ 𝗠𝗔𝗗𝗘 𝗕𝗬 @𝗕𝗗_𝗛𝗔𝗖𝗞𝗘𝗥_𝗢𝗙𝗙𝗜𝗖𝗜𝗔𝗟_𝗣𝗥𝗜𝗩𝗔𝗧𝗘_𝗦𝗥𝗖 ]============================
===============================[ 𝗠𝗔𝗗𝗘 𝗕𝗬 @𝗕𝗗_𝗛𝗔𝗖𝗞𝗘𝗥_𝗢𝗙𝗙𝗜𝗖𝗜𝗔𝗟_𝗣𝗥𝗜𝗩𝗔𝗧𝗘_𝗦𝗥𝗖 ]============================
===============================[ 𝗠𝗔𝗗𝗘 𝗕𝗬 @𝗕𝗗_𝗛𝗔𝗖𝗞𝗘𝗥_𝗢𝗙𝗙𝗜𝗖𝗜𝗔𝗟_𝗣𝗥𝗜𝗩𝗔𝗧𝗘_𝗦𝗥𝗖 ]============================
*/
