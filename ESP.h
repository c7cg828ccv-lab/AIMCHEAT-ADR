#pragma once
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <pthread.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <Il2Cpp.h>
#include <Tools.h>
#include <Vector3.hpp>
#include <Color.h>
#include <Struct/main.h>
#include "obfuscate.h"
#include "Unity/unity.h"
#include "Class.h"
#include "Rect.h"
#include <deque>
#include <set>
#include <string>
#include <vector>
inline static int g_GlHeight, g_GlWidth;
#include "Bools.h"
bool SpeedHack = false;
bool Reset = false;
bool JumpHight = false;
#include <math.h>
#include <stdio.h>
typedef char PACKAGENAME;
bool SniperFast = false;
bool EspCiruleHead = true;

ImColor GetRainbowColor(float speed = 2.0f) {
    float time = ImGui::GetTime() * speed;
    float r = sin(time) * 0.5f + 0.5f;
    float g = sin(time + 2.0f) * 0.5f + 0.5f;
    float b = sin(time + 4.0f) * 0.5f + 0.5f;
    return ImColor(r, g, b, 1.0f);
}

int  glWidth, glHeight;

template <typename T>
struct UnityArray {
    void* klass;
    void* monitor;
    void* bounds;
    int max_length;
    T vector[64]; 
};

struct DictionaryEntry {
    int hashCode;
    int next;
    void* key;
    void* value;
};

struct MyDictionary {
    void* klass;
    void* monitor;
    void* buckets;
    UnityArray<DictionaryEntry>* entries;
    int count;
};

// উজ্জ্বল পিঙ্ক কালার (Bright Pink) ডিফাইন করা হলো
ImVec4 BrightPinkColor = ImVec4(1.0f, 0.23f, 0.62f, 1.0f);

struct sColor {
    float line[4] = {1.0f, 0.23f, 0.62f, 1.0f};
    float box[4] = {1.0f, 0.23f, 0.62f, 1.0f};
};
sColor Color;
   
bool isOutsideScreen(ImVec2 pos, ImVec2 screen) {
    if (pos.y < 0) return true;
    if (pos.x > screen.x) return true;
    if (pos.y > screen.y) return true;
    return pos.x < 0;
}

ImVec2 pushToScreenBorder(ImVec2 Pos, ImVec2 screen, int offset) {
    int x = (int) Pos.x;
    int y = (int) Pos.y;
    if (Pos.y < 0) y = -offset;
    if (Pos.x > screen.x) x = (int) screen.x + offset;
    if (Pos.y > screen.y) y = (int) screen.y + offset;
    if (Pos.x < 0) x = -offset;
    return ImVec2(x, y);
}

static float tm = 127 / 255.f;

void DrawCircleHealth(ImVec2 position, int health, int max_health, float radius) {
    float a_max = ((3.14159265359f * 2.0f));
    ImU32 healthColor = IM_COL32(255, 60, 160, 255);
    if (health <= (max_health * 0.6)) {
        healthColor = IM_COL32(255, 140, 200, 255);
    }
    if (health < (max_health * 0.3)) {
        healthColor = IM_COL32(255, 0, 100, 255);
    }
    ImGui::GetForegroundDrawList()->PathArcTo(position, radius, (-(a_max / 4.0f)) + (a_max / max_health) * (max_health - health), a_max - (a_max / 4.0f));
    ImGui::GetForegroundDrawList()->PathStroke(healthColor, ImDrawFlags_None, 4);
}

void drawText2(int x, int y, long color, const char *str, float size) {
    ImGui::GetBackgroundDrawList()->AddText(NULL, size, ImVec2(x, y), color, str);
}

void DrawRectFilled(int x, int y, int w, int h, ImColor color) {
    ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, 0, 0);
}

void DrawRectFilledHealth(int x, int y, int w, int h, ImVec4 color) {
    ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(color), 0, 0);
}

void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int color, int T) {
    ImGui::GetBackgroundDrawList()->AddTriangleFilled(ImVec2(x1, y1), ImVec2(x2, y2), ImVec2(x3, y3), color);
}

bool IsVisible(void * player){
    if(player != NULL) {
        void *hitObj = NULL;
        Vector3 cameraLocation = Transform_GetPosition(Component_GetTransform(Camera_main()));
        Vector3 headLocation = Transform_GetPosition(Component_GetTransform(Player_GetHeadCollider(player)));
        return !Physics_Raycast(cameraLocation, headLocation, 12, &hitObj);
    }
    return false;
}

void DrawHealths(Rect box, float entityHealth, float maxHealth, int TeamID, const char* name, long color) {
    float x = box.x - (140 - box.w) / 2;
    float y = box.y;
    char dis[20];
    char TeamText[20];
    sprintf(TeamText, "%d", (int)TeamID);
    ImVec4 HPColor = ImVec4{1.0f, 0.23f, 0.62f, 127 / 255.f}; 

    if (entityHealth < maxHealth) {
        float healthPercentage = entityHealth / maxHealth;
        if (healthPercentage < 0.3f) {
            HPColor = ImVec4{1.0f, 0.0f, 0.4f, 127 / 255.f}; 
        } else if (healthPercentage < 0.6f) {
            HPColor = ImVec4{1.0f, 0.5f, 0.8f, 127 / 255.f}; 
        }
    }

    tm = 100.f / 255.f;
    DrawRectFilled(x - strlen(name), y - 41, 120 + strlen(name) * 2, 20, color); 
    tm = 255.f / 255.f;
    DrawRectFilled(x - strlen(name), box.y - 41, 25, 20, color);
    if (strlen(TeamText) < 2) {
        drawText2(x + 6 - strlen(name), box.y - 42, ImColor(255,255,255), TeamText, 18.943);
    } else {
        drawText2(x - strlen(name), box.y - 42, ImColor(255,255,255), TeamText, 18.943);
    }
    drawText2(x + 28 - strlen(name), y - 43, ImColor(255,255,255), name, 18.943);
    float maxWidth = 120; 
    float healthBarWidth = entityHealth * maxWidth / maxHealth; 
    DrawRectFilledHealth(x - maxWidth / 2, y - 18, healthBarWidth, 8, HPColor);
    drawText2(x + 125 + strlen(name), y - 43, ImColor(255,255,255), dis, 18.943);
    DrawTriangle(box.x + box.w / 2 - 10, y - 8, box.x + box.w / 2 + 15 - 10, y - 8, box.x + box.w / 2 - 2, y, ImColor(255,255,255), 1);
}

static float SetFieldOfView = 0, GetFieldOfView = 0;

void DroneView() {
    if (GetFieldOfView == 0) {
        GetFieldOfView = get_fieldOfView();
    }
    if (SetFieldOfView > 0 && GetFieldOfView != 0) {
        set_fieldOfView((float)GetFieldOfView + SetFieldOfView);
    }
    if (SetFieldOfView == 0 && GetFieldOfView != 0) {
        set_fieldOfView((float)GetFieldOfView);
    }
}

static bool g_FastReloadHookInstalled = false;
#define offset_FastReloadDashSpeed (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"), OBFUSCATE("GPBDEDFKJNA"), OBFUSCATE("DNGONFLICGC"), 0)
      
float (*FastReload)(void *instance);
float _FastReload(void *instance) {
    if (instance != NULL) {
        if (FastReload1) {
            return 99.0f;
        }
    }
    return FastReload(instance);
}

void InitFastReloadHackHook() {
    if (g_FastReloadHookInstalled) return;
    DobbyHook((void *)offset_FastReloadDashSpeed, (void *)_FastReload, (void **)&FastReload);
    g_FastReloadHookInstalled = true;
}

static bool g_SpeedHookInstalled = false;
#define offset_GetCurrentDashSpeed (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"), OBFUSCATE("Player"), OBFUSCATE("GetCurrentDashSpeed"), 0)
      
float (*old_GetCurrentDashSpeed)(void *instance);
float hook_GetCurrentDashSpeed(void *instance) {
    if (!instance) return old_GetCurrentDashSpeed(instance);
    void *match = Curent_Match();
    if (!match) return old_GetCurrentDashSpeed(instance);
    void *localPlayer = GetLocalPlayer(match);
    if (instance == localPlayer && SpeedHack) {
        return 9.0f;
    }
    return old_GetCurrentDashSpeed(instance);
}

void InitSpeedHackHook() {
    if (g_SpeedHookInstalled) return;
    DobbyHook((void *)offset_GetCurrentDashSpeed, (void *)hook_GetCurrentDashSpeed, (void **)&old_GetCurrentDashSpeed);
    g_SpeedHookInstalled = false;
}

void *GetClosestEnemy() {
    float shortestDistance = 9999;
    void* closestEnemy = NULL;   
     
    void* get_MatchGame = Curent_Match();
    void* LocalPlayer = GetLocalPlayer(get_MatchGame);

    if (LocalPlayer != NULL && get_MatchGame != NULL && Enable) {
        MyDictionary* dict = *(MyDictionary**)((uintptr_t)get_MatchGame + ListPlayer);
        
        if (dict != nullptr && dict->entries != nullptr) {
            void* camera = Camera_main();
            int total = dict->count;

            for (int u = 0; u < total; u++) {
                void* Player = dict->entries->vector[u].value; 
                
                if (Player != NULL && Player != LocalPlayer && !get_isLocalTeam(Player) && !get_IsDieing(Player) && get_isVisible(Player) && get_MaxHP(Player)) {    
                    Vector3 PlayerPos = getPosition(Player);
                    Vector3 LocalPlayerPos = getPosition(LocalPlayer);
                    Vector3 pos2 = WorldToScreenPoint(camera, PlayerPos);
                    
                    bool isFov1 = isFov(Vector3(pos2.x, pos2.y), Vector3(g_GlWidth / 2, g_GlHeight / 2), Fov_Aim);
                    
                    float distance = Vector3::Distance(LocalPlayerPos, PlayerPos);
                    if (distance < Aimdis) {
                        Vector3 targetDir = Vector3::Normalized(PlayerPos - LocalPlayerPos);
                        float angle = Vector3::Angle(targetDir, GetForward(Component_GetTransform(camera))) * 100.0;             
                
                        if (angle <= Fov_Aim && isFov1 && angle < shortestDistance) {
                            shortestDistance = angle;
                            closestEnemy = Player;
                        }
                    }
                }
            }
        }
    }
    return closestEnemy;
}
float (*get_deltaTime)();
float smoothSpeedAIM = 5; 
void *MyPlayer = NULL;
bool Drone;

const float RADAR_SIZE = 200.0f; 
const ImVec2 RADAR_CENTER(100.0f, 100.0f); 

float x, y;
float enemy1 = 2.0f;

ImVec4 colorTextBackground = ImVec4(0, 0, 0, 255); 
ImVec4 colorBox = ImVec4(1.0f, 0.0f, 0.0f, 0.5f);       
ImVec4 colorHealth = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); 
ImVec4 colorDistance = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); 
ImVec4 colorLine = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);        
ImVec4 colorHexagono = ImVec4(150 / 255.0f, 0, 255 / 255.0f, 1.0f);
ImVec4 colorMenuText = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

void* CurrentMatch = nullptr;
void* closestEnemy = nullptr;
void* LocalPlayer = nullptr;
float aimSmoothness = 0.1f;

inline void DrawAIMBOT(float screenWidth, float screenHeight) {}

inline void DrawESP(float screenWidth, float screenHeight) {
    ImDrawList* draw = ImGui::GetBackgroundDrawList();
    if (!draw) return;   

    if (Aimbot) {
        void* CurrentMatch = Curent_Match();
        void* closestEnemy = GetClosestEnemy();
        void* LocalPlayer = GetLocalPlayer(CurrentMatch);

        if (closestEnemy != NULL && LocalPlayer != NULL && CurrentMatch != NULL) {
            Vector3 EnemyLocation = GetHeadPosition(closestEnemy);
            Vector3 PlayerLocation = CameraMain(LocalPlayer);
            Quaternion PlayerLook = GetRotationToLocation(EnemyLocation, 0.1f, PlayerLocation);
            bool IsScopeOn = get_IsSighting(LocalPlayer);
            bool IsFiring = get_IsFiring(LocalPlayer);

            if (AimWhen == 0) {
                set_aim(LocalPlayer, PlayerLook);
            } else if (AimWhen == 1 && IsFiring) {
                set_aim(LocalPlayer, PlayerLook);
            } else if (AimWhen == 2 && IsScopeOn) {
                set_aim(LocalPlayer, PlayerLook);
            }
        }
    }

    if (Enable) {   
        void* current_Match = Curent_Match();
        void* local_player = GetLocalPlayer(current_Match);
             
        if (local_player && current_Match) {
            MyDictionary* players = *(MyDictionary**)((uintptr_t)current_Match + ListPlayer);
            void* camera = Camera_main();

            if (players && players->entries && camera) {
                int total = players->count;
                for (int u = 0; u < total; u++) {
                    void* closestEnemy = players->entries->vector[u].value;

                    if (closestEnemy && closestEnemy != local_player && get_isVisible(closestEnemy) && !get_isLocalTeam(closestEnemy)) {
                        Vector3 Toepos = getPosition(closestEnemy);
                        Vector3 Toeposi = WorldToScreenPoint(camera, Toepos);
                        if (Toeposi.z < 1) continue;
                            
                        Vector3 HeadPos = Toepos + Vector3(0, 1.9f, 0);
                        Vector3 HeadPosition = WorldToScreenPoint(camera, HeadPos);
                        if (HeadPosition.z < 1) continue;

                        float distance = Vector3::Distance(getPosition(local_player), Toepos);
                        float Height = abs(HeadPosition.y - Toeposi.y) * 1.09f;
                        float Width = Height * 0.55f;
                        Rect rect = Rect(HeadPosition.x - Width / 2.f, screenHeight - HeadPosition.y, Width, Height);
                        
                        if (closestEnemy != nullptr && !get_IsDieing(closestEnemy)) {
                            
                            // ================= CROSSHAIR (রেইনবো এফেক্ট) =================
                            static float hue = 0.0f;
                            hue += ImGui::GetIO().DeltaTime * 0.2f;
                            if (hue > 1.0f) hue -= 1.0f;
                            
                            ImVec4 col;
                            ImGui::ColorConvertHSVtoRGB(hue, 1.0f, 1.0f, col.x, col.y, col.z);
                            ImU32 rainbowCol = ImGui::ColorConvertFloat4ToU32(ImVec4(col.x, col.y, col.z, 1.0f));

                            static float rotation = 0.0f;
                            rotation += ImGui::GetIO().DeltaTime * 3.0f;

                            ImVec2 center = ImVec2(ImGui::GetIO().DisplaySize.x / 2.0f, ImGui::GetIO().DisplaySize.y / 2.0f);
                            float size = 7.0f;

                            auto rotateP = [&](ImVec2 p) {
                                float s = sin(rotation), c = cos(rotation);
                                p.x -= center.x; p.y -= center.y;
                                return ImVec2((p.x * c - p.y * s) + center.x, (p.x * s + p.y * c) + center.y);
                            };

                            draw->AddLine(rotateP(ImVec2(center.x - size, center.y - size)), rotateP(ImVec2(center.x + size, center.y + size)), rainbowCol, 1.5f);
                            draw->AddLine(rotateP(ImVec2(center.x + size, center.y - size)), rotateP(ImVec2(center.x - size, center.y + size)), rainbowCol, 1.5f);
                            draw->AddCircleFilled(center, 1.2f, rainbowCol, 12);
                            
                            // ================= ESP BOX (উজ্জ্বল পিঙ্ক কালার - সরাসরি ড্র লিস্ট ব্যবহার করা হয়েছে) =================
                            draw->AddRect(ImVec2(rect.x, rect.y), ImVec2(rect.x + rect.w, rect.y + rect.h), IM_COL32(255, 60, 160, 255), 0.0f, 0, 2.0f);
                            
                            // ================= ESP LINE (উজ্জ্বল পিঙ্ক কালার) =================
                            draw->AddLine(ImVec2(screenWidth / 2.0f, 0.0f), ImVec2(rect.x + rect.w / 2.0f, rect.y), IM_COL32(255, 60, 160, 255), 1.5f);
                            
                            // ================= ESP HEALTH (উজ্জ্বল পিঙ্ক কালার) =================
                            float maxHp = get_MaxHP(closestEnemy);
                            float currentHp = GetHp(closestEnemy);
                            float healthPercentage = currentHp / maxHp;

                            float healthBarHeight = rect.h * healthPercentage;
                            float xx = rect.x + rect.w + 4.0f;
                            float yy = rect.y;

                            // হেলথবারের ব্যাকগ্রাউন্ড
                            draw->AddRectFilled(ImVec2(xx, yy), ImVec2(xx + 4.0f, yy + rect.h), IM_COL32(0, 0, 0, 180));
                            // মেইন হেলথবার (উজ্জ্বল পিঙ্ক)
                            draw->AddRectFilled(ImVec2(xx, yy + (rect.h - healthBarHeight)), ImVec2(xx + 4.0f, yy + rect.h), IM_COL32(255, 60, 160, 255));
                        }
                    }
                }
            }
        }
    }
}

// ==================== CRASH-PROOF SAFE HOOKS ====================
bool(*_ResetGuest)(void* _this);
bool ResetGuest(void* _this) {
    if (_this == NULL) return false;
    if (Reset) {
        if (access("/storage/emulated/0/com.garena.msdk/guest100067.dat", F_OK) != -1) {
            remove("/storage/emulated/0/com.garena.msdk/guest100067.dat");
        }
        return true; 
    }
    if (_this != NULL && _ResetGuest) {
        return _ResetGuest(_this);
    }
    return false;
}

float (*orig_get_MaxJumpHeight)(void *instance);
float (*orig_get_CustomGravity)(void *instance);
float (*orig_get_RisingGravity)(void *instance);
float (*orig_get_FallingGravity)(void *instance);

float my_get_MaxJumpHeight(void *instance) {
    if (instance == NULL) return 0.0f;
    if (JumpHight && orig_get_MaxJumpHeight) {
        return 1.4f;
    }
    return orig_get_MaxJumpHeight ? orig_get_MaxJumpHeight(instance) : 0.0f;
}

float my_get_CustomGravity(void *instance) {
    if (instance == NULL) return 0.0f;
    if (JumpHight && orig_get_CustomGravity) {
        return -3.0f;
    }
    return orig_get_CustomGravity ? orig_get_CustomGravity(instance) : 0.0f;
}

float my_get_RisingGravity(void *instance) {
    if (instance == NULL) return 0.0f;
    if (JumpHight && orig_get_RisingGravity) {
        return -3.0f;
    }
    return orig_get_RisingGravity ? orig_get_RisingGravity(instance) : 0.0f;
}

float my_get_FallingGravity(void *instance) {
    if (instance == NULL) return 0.0f;
    if (JumpHight && orig_get_FallingGravity) {
        return -3.0f;
    }
    return orig_get_FallingGravity ? orig_get_FallingGravity(instance) : 0.0f;
}

bool(*Bypass)(void* bypass); 
bool _Bypass(void* bypass) { 
   return false;
}



