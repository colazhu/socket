LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE    := runsocket
LOCAL_CFLAGS    := -Wno-error=unused-parameter -DOS_ANDROID=1
LOCAL_C_INCLUDES := \


LOCAL_SRC_FILES := \
main.cpp \
SocketClient.cpp \
Log.cpp \
SocketThread.cpp \
Condition.cpp \
SocketServer.cpp

LOCAL_LDLIBS := -llog
include $(BUILD_EXECUTABLE)

