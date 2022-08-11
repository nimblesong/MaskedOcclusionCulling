LOCAL_PATH := $(call my-dir)
SRC_DIR := ../../../

include $(CLEAR_VARS)
LOCAL_MODULE := MaskedOC
LOCAL_C_INCLUDES += $(SRC_DIR)
LOCAL_CFLAGS := -mfpu=neon
LOCAL_LDLIBS := -llog
LOCAL_ARM_MODE := arm
LOCAL_ARM_NEON := true

LOCAL_SRC_FILES += $(SRC_DIR)/CullingThreadpool.cpp
LOCAL_SRC_FILES += $(SRC_DIR)/FrameRecorder.cpp
LOCAL_SRC_FILES += $(SRC_DIR)/MaskedOcclusionCullingNEON128.cpp
LOCAL_SRC_FILES += $(SRC_DIR)/MaskedOcclusionCulling.cpp
LOCAL_SRC_FILES += $(SRC_DIR)/MaskedOcclusionCullingPlugin.cpp

# build
include $(BUILD_SHARED_LIBRARY)

