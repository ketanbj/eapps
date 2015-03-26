LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)


LOCAL_SRC_FILES := client.c

LOCAL_SHARED_LIBRARIES := libcutils liblog libephemeralutils

LOCAL_MODULE := cliephemeralutils

include $(BUILD_EXECUTABLE)

