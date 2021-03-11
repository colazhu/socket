LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE    := testsocketclient
LOCAL_CFLAGS    := -Wno-error=unused-parameter
LOCAL_C_INCLUDES := \


LOCAL_SRC_FILES := \
main_client.cpp

LOCAL_LDLIBS := -llog
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_MODULE    := testsocketserver
LOCAL_CFLAGS    := -Wno-error=unused-parameter
LOCAL_C_INCLUDES := \


LOCAL_SRC_FILES := \
main_server.cpp

LOCAL_LDLIBS := -llog
include $(BUILD_EXECUTABLE)
