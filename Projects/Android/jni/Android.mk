LOCAL_PATH := $(call my-dir)
SRC_DIR := ../../../

include $(CLEAR_VARS)
LOCAL_MODULE := MaskedOC
LOCAL_C_INCLUDES += $(SRC_DIR)
LOCAL_LDLIBS := -llog
LOCAL_ARM_MODE := arm

ifeq ($(TARGET_ARCH_ABI),x86) 
LOCAL_CFLAGS := -msoft-float -march=i686 -mtune=intel -mssse3 -mfpmath=sse -m32
else
LOCAL_CFLAGS := -g -mfloat-abi=softfp -mfpu=neon-vfpv4
LOCAL_ARM_NEON := true
endif

LOCAL_SRC_FILES += $(SRC_DIR)/CullingThreadpool.cpp
LOCAL_SRC_FILES += $(SRC_DIR)/FrameRecorder.cpp
ifneq ($(TARGET_ARCH_ABI),x86) 
LOCAL_SRC_FILES += $(SRC_DIR)/MaskedOcclusionCullingNEON128.cpp
endif
LOCAL_SRC_FILES += $(SRC_DIR)/MaskedOcclusionCulling.cpp
LOCAL_SRC_FILES += $(SRC_DIR)/MaskedOcclusionCullingPlugin.cpp

# build
include $(BUILD_SHARED_LIBRARY)
