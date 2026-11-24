LOCAL_PATH := $(call my-dir)

# ================= Dobby =================
include $(CLEAR_VARS)
LOCAL_MODULE    := libdobby
LOCAL_SRC_FILES := Dobby/$(TARGET_ARCH_ABI)/libdobby.a
include $(PREBUILT_STATIC_LIBRARY)

# ================= MAIN =================
include $(CLEAR_VARS)

LOCAL_MODULE := BDHACKEROFC

LOCAL_CFLAGS := -Wno-error=format-security -fvisibility=hidden -ffunction-sections -fdata-sections -w
LOCAL_CFLAGS += -fno-rtti -fno-exceptions -fpermissive

LOCAL_CPPFLAGS := -Wno-error=format-security -fvisibility=hidden -ffunction-sections -fdata-sections -w -std=c++17
LOCAL_CPPFLAGS += -Wno-error=c++11-narrowing -fms-extensions -fno-rtti -fno-exceptions -fpermissive

LOCAL_LDFLAGS += -Wl,--gc-sections,--strip-all
LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv3 -lGLESv2 -lz

LOCAL_ARM_MODE := arm

# ===== INCLUDES FIX =====
LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/Dobby   # ✅ Dooby → Dobby FIX
LOCAL_C_INCLUDES += $(LOCAL_PATH)/imgui
LOCAL_C_INCLUDES += $(LOCAL_PATH)/KittyMemory
LOCAL_C_INCLUDES += $(LOCAL_PATH)/Unity
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/And64InlineHook
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/Substrate

# ===== FILE LIST =====
FILE_LIST := $(wildcard $(LOCAL_PATH)/imgui/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/xdl/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/KittyMemory/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/xBDHACKER-PAPAJI/IL2CppSDKGenerator/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/*.c*)

LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)

# ===== HOOK SELECT =====
ifeq ($(TARGET_ARCH_ABI), arm64-v8a)
    HOOK_SRC := $(wildcard $(LOCAL_PATH)/include/And64InlineHook/*.c*)
    LOCAL_SRC_FILES += $(HOOK_SRC:$(LOCAL_PATH)/%=%)
else ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)
    HOOK_SRC := $(wildcard $(LOCAL_PATH)/include/Substrate/*.c*)
    LOCAL_SRC_FILES += $(HOOK_SRC:$(LOCAL_PATH)/%=%)
endif

LOCAL_STATIC_LIBRARIES := libdobby
LOCAL_CPP_FEATURES := exceptions

include $(BUILD_SHARED_LIBRARY)
