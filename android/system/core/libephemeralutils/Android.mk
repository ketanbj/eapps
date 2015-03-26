LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

libepehemeral_sources := \
	ephemeral_client.c 
	
# For the host
# =====================================================
include $(CLEAR_VARS)
LOCAL_SRC_FILES:= $(libepehemeral_sources)
LOCAL_MODULE:= libephemeralutils
LOCAL_STATIC_LIBRARIES := libcutils liblog
LOCAL_CFLAGS += $(host_commonCflags)
LOCAL_MULTILIB := both
include $(BUILD_HOST_STATIC_LIBRARY)

# Shared and static library for target
# ========================================================
include $(CLEAR_VARS)
LOCAL_MODULE := libephemeralutils
LOCAL_SRC_FILES := $(libepehemeral_sources)
LOCAL_STATIC_LIBRARIES := libcutils liblog
LOCAL_C_INCLUDES := \
	system/core/include/ephemeralutils
#LOCAL_CFLAGS := -Werror
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libephemeralutils
LOCAL_SRC_FILES := $(libepehemeral_sources)
LOCAL_SHARED_LIBRARIES := libcutils liblog
LOCAL_C_INCLUDES := \
	system/core/include/ephemeralutils
#LOCAL_CFLAGS := -Werror
include $(BUILD_SHARED_LIBRARY)
