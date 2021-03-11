#if OS_ANDROID
#include "SocketServer.h"
class SocketListener : public SocketCallback
{
public:
    SocketListener(const std::string& name, SocketServer* dest) : m_name(name), m_dest(dest) {}
    virtual ~SocketListener() {}

    // void setDst(SocketServer* dest) { m_dest = dest; }
    virtual void onReceiveData(const std::string& data, int clientid = 0) {
        LOG_BASE("[%s]onRceiveData:%s", m_name.c_str(), data.c_str());
        if (m_dest) {
            m_dest->sendData(data);
        }
    }
    std::string m_name;
    SocketServer* m_dest;
};

int main(int argc, char **argv)
{
    // char* address = "192.168.5.111";

    SocketServer serverImx8("ServerImx8", 2000);
    SocketServer serverCube("ServerCube", 2001);

    serverImx8.setListener(new SocketListener("ServerImx8",  &serverCube));
    serverImx8.start();

    serverCube.setListener(new SocketListener("ServerCube", &serverImx8));
    serverCube.start();

    while (true) {
        if (!serverCube.isConnected() || !serverImx8.isConnected()) {
            system("ifconfig eth0 up \n");
            system("ifconfig eth0 192.168.5.112 \n");
        }
        sleep(2);
    }

    return 0;
}
#else

#include "unistd.h"
#include "stdio.h"
#include "GLFW/glfw3.h"
#include "Flags.h"
#include "Log.h"

#include "SocketServer.h"
static SocketClient* s_clientSocket = NULL;
enum MOUSE_STATUS {
    MOUSE_ENTER = 0x0001,
    MOUSE_LEFT = 0x0002,
    MOUSE_RIGHT = 0x0004,
    MOUSE_MIDDLE = 0x0008,
};

DECLARE_FLAGS(MouseStatus, MOUSE_STATUS)

enum MOUSE_EVENT {
    MOUSE_DOWN_LEFT,
    MOUSE_UP_LEFT,
    MOUSE_DOWN_RIGHT,
    MOUSE_UP_RIGHT,
    MOUSE_DOWN_MIDDLE,
    MOUSE_UP_MIDDLE,
    MOUSE_DOWN_ALL,
    MOUSE_UP_ALL,
    MOUSE_MOVE,

    MOUSE_DOWN = MOUSE_DOWN_LEFT,
    MOUSE_UP = MOUSE_UP_LEFT,
};

struct MouseEvent {
    int event;
    int x;
    int y;
    MouseStatus status;
};
static MouseEvent s_mouseevent;

//void onGestureEvent(const struct Gesture_Event_Data_List* list, void *data)
//{
//    if (!list) {
//        return;
//    }

//    printf("\n onGestureEvent: eventlist:%p data:%p +++ \n", list, data);
//    GestureCommonFun::printGestureDataList(list);

//    const Gesture_Event_Data_List* curList = list;
//    while (curList) {
//        GestureEvent gestureev;
//        gestureev.gclass = curList->data->gclass;
//        gestureev.gtype = curList->data->gtype;
//        Director::instance()->injectGesture(gestureev);
//        curList = curList->next;
//    }
//    printf("\n onGestureEvent --- \n");

//}

void onMouseEvent(const MouseEvent& event) {
    printf("\n onMouseEvent: event:%d x:%d, y:%d flg:0x%08x +++ \n", event.event, event.x, event.y, event.status);

    int action = 3;

    switch (event.event) {
    case MOUSE_DOWN:
        action = 0;
//        touchevent = TouchAction_Down;
//        motionEvent = MOTION_EVENT_ACTION_DOWN;
//        point[0].state = TouchPointPressed;
        break;
    case MOUSE_UP:
        action = 1;
//        touchevent = TouchAction_Up;
//        motionEvent = MOTION_EVENT_ACTION_UP;
//        point[0].state = TouchPointReleased;
        break;
    case MOUSE_MOVE:
        action = 2;
//        touchevent = TouchAction_Move;
//        motionEvent = MOTION_EVENT_ACTION_MOVE;
//        point[0].state = TouchPointMoved;
        break;
    default:
        return;
    }
////    GestureManager::instance()->processTouchEvent(motionEvent, 0, point, 1, GestureCommonFun::currentTime());
//    TouchEvent touchev;
//    // touchev.eventtype;
//    touchev.action = touchevent;
//    touchev.x = event.x;
//    touchev.y = event.y;
//    Director::instance()->injectTouch(touchev);
    char data[22] = {0};
    snprintf(data, 22, "touch%04d%04d%04d%04d", action, event.x*4, event.y*4, 0);

    if (s_clientSocket) {
        s_clientSocket->sendData(std::string(data, 21));
    }
}

void framebuffsize_callback(GLFWwindow* window, int width, int height) {
    printf("\n framebuffsize_callback:%p  w:%d h:%d +++ \n", window, width, height);
    glViewport(0, 0, width, height);
//    Director::instance()->setWindowSize(width, height);
    printf("\n framebuffsize_callback --- \n");
}

void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
    printf("\n mouse_callback :%p button:%d action:%d mods:%d +++ \n", window, button, action, mods);
    enum {
        MOUSEBUTTON_LEFT = 0,
        MOUSEBUTTON_RIGHT,
        MOUSEBUTTON_MIDDLE
    };

    if (!s_mouseevent.status.testFlag(MOUSE_ENTER)) {
        return;
    }

    switch (button) {
    case MOUSEBUTTON_LEFT:
        if (GLFW_PRESS == action) {
            s_mouseevent.event = MOUSE_DOWN_LEFT;
            s_mouseevent.status.setFlag(MOUSE_LEFT, true);
        }
        else {
            s_mouseevent.event = MOUSE_UP_LEFT;
            s_mouseevent.status.setFlag(MOUSE_LEFT, false);
        }
        break;
    case MOUSEBUTTON_RIGHT:
        if (GLFW_PRESS == action) {
            s_mouseevent.event = MOUSE_DOWN_RIGHT;
            s_mouseevent.status.setFlag(MOUSE_RIGHT, true);
        }
        else {
            s_mouseevent.event = MOUSE_UP_RIGHT;
            s_mouseevent.status.setFlag(MOUSE_RIGHT, false);
        }
        break;
    case MOUSEBUTTON_MIDDLE:
        if (GLFW_PRESS == action) {
            s_mouseevent.event = MOUSE_DOWN_MIDDLE;
            s_mouseevent.status.setFlag(MOUSE_MIDDLE, true);
        }
        else {
            s_mouseevent.event = MOUSE_UP_MIDDLE;
            s_mouseevent.status.setFlag(MOUSE_MIDDLE, false);
        }
        break;
    default:
        return;
    }
    onMouseEvent(s_mouseevent);
    printf("\n mouse_callback --- \n");
}

void cursorenter_callback(GLFWwindow* window, int enter)
{
    printf("\n cursorenter_callback :%p enter:%d +++ \n", window, enter);

    bool preEnter = s_mouseevent.status.testFlag(MOUSE_ENTER);
    bool curEnter = (enter == GL_TRUE) ? true : false;
    if (preEnter == curEnter) {
        return;
    }
    s_mouseevent.status.setFlag(MOUSE_ENTER, curEnter);

    if (s_mouseevent.status.testFlag(MOUSE_ENTER)) {
        return;
    }

    if (!s_mouseevent.status.testFlag(MOUSE_LEFT)
        && !s_mouseevent.status.testFlag(MOUSE_RIGHT)
        && !s_mouseevent.status.testFlag(MOUSE_MIDDLE)) {
        return;
    }

    s_mouseevent.event = MOUSE_UP_ALL;
    s_mouseevent.status.setFlag(MOUSE_LEFT, false);
    s_mouseevent.status.setFlag(MOUSE_RIGHT, false);
    s_mouseevent.status.setFlag(MOUSE_MIDDLE, false);
    onMouseEvent(s_mouseevent);

    printf("\n cursorenter_callback :%p --- \n", window);
}

void cursorpos_callback(GLFWwindow* window, double x, double y)
{
    s_mouseevent.x = (int)x;
    s_mouseevent.y = (int)y;
    if (!s_mouseevent.status.testFlag(MOUSE_ENTER)) {
        return;
    }

    if (!s_mouseevent.status.testFlag(MOUSE_LEFT)
        && !s_mouseevent.status.testFlag(MOUSE_RIGHT)
        && !s_mouseevent.status.testFlag(MOUSE_MIDDLE)) {
        return;
    }
    printf("\n cursorpos_callback :%p x:%d y:%d \n", window, s_mouseevent.x, s_mouseevent.y);
    s_mouseevent.event = MOUSE_MOVE;
    onMouseEvent(s_mouseevent);
}

void dropfile_callback(GLFWwindow* window, int count, const char** path) {
    if (count <= 0 || path == NULL) {
        return;
    }
    printf("\n dropfile_callback :%p count:%d +++ \n", window, count);
    for (int i = 0; i < count; ++i) {
        printf("\n [%d][%s] \n", i, path[i]);
    }
    printf("\n dropfile_callback :%p --- \n", window);
}

void close_callback(GLFWwindow* window)
{
     printf("\n close_callback :%p \n", window);
}

//void scroll_callback(GLFWwindow* window, double x, double y)
//{
//    printf("\n scroll_callback :%p x:%.1lf y:%.1lf +++ \n", window, x, y);
//    printf("\n scroll_callback --- \n");
//}


void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    LOG_BASE_TRACE("process_input_callabck:%p  key:%d scancode:%d  action:%d mods:%d", window, key, scancode, action, mods);
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

GLFWwindow* initGLWindow(const char* title, int w, int h)
{

    GLFWwindow* window = glfwCreateWindow(w, h, title ? title : " ", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_callback);
    glfwSetCursorPosCallback(window, cursorpos_callback);
    glfwSetCursorEnterCallback(window, cursorenter_callback);
    // glfwSetScrollCallback(window, scroll_callback);

    glfwSetFramebufferSizeCallback(window, framebuffsize_callback);
    glfwSetWindowCloseCallback(window, close_callback);
    glfwSetDropCallback(window, dropfile_callback);
    return window;
}

#define DEFAULT_SIZE_WIDTH (2560/4)
#define DEFAULT_SIZE_HEIGHT (1600/4)
#define DEFAULT_PIXEL_FACTOR (1.0)
#define TITLE_NAME ("Tools")

//void loadBmp(int txtid, const char* filepath, int w, int h)
//{
//    char* data = NULL;
//    unsigned long size = w*h*4+54;
////    unsigned long size = 2560*1440*3+54;
//    FILE * file = fopen(filepath, "rb");
//    do {
//        if (!file) {
//            break;
//        }
//        data = new char[size];
//        unsigned long readsize = fread(data, 1, size, file);
//        if (size != readsize) {
//            break;
//        }
////        {
////             LOG_BASE_DUMP(data, 54);
////             data[28] = 32;
////             unsigned long size2 = 2560*1440*4+54;
////             char* data2 = new char[size2];
////             memcpy(data2, data, 54);
////            for (int i = 0; i < (size-54)/3; ++i) {
////                data2[54 + i*4] = data[54 + i*3];
////                data2[54 + i*4 + 1] = data[54 + i*3 + 1];
////                data2[54 + i*4 + 2] = data[54 + i*3 + 2];
////                data2[54 + i*4 + 3] = 0xff;
////            }
////            FILE * fileOut = fopen("/home/colazhu/takoyaki/takoyaki/TestProj/out/TestImage2.bmp", "wb+");
////            fwrite(data2, 1, size2, fileOut);
////            fclose(fileOut);
////        }
//        Director::instance()->addTexture(txtid, data + 54, 150, 150);
//    } while (0);
//    delete [] data;
//    if (file) {
//        fclose(file);
//        file = NULL;
//    }
//}

class SocketListener : public SocketCallback
{
public:
    SocketListener(const std::string& name) : m_name(name){}
    virtual ~SocketListener() {}

    // void setDst(SocketServer* dest) { m_dest = dest; }
    virtual void onReceiveData(const std::string& data, int clientid = 0) {
        LOG_BASE("[%s]onRceiveData:%s", m_name.c_str(), data.c_str());
    }

    std::string m_name;
};

int main(int argc, char** argv)
{  
//    GestureManager::instance()->setFoucsSurfaceRegion(new GestureRegion(0, 0, DEFAULT_SIZE_WIDTH, DEFAULT_SIZE_HEIGHT));
//    GestureManager::instance()->setNotifyFunc(onGestureEvent);

    char* address = "192.168.5.112";
    s_clientSocket = new SocketClient("PCClient", address, 2000);
    s_clientSocket->setListener(new SocketListener("PCClient"));
    s_clientSocket->start();

    glfwInit();
//    CEGuiRender render;

    do {
        GLFWwindow* window = initGLWindow(TITLE_NAME, DEFAULT_SIZE_WIDTH, DEFAULT_SIZE_HEIGHT);
        if (NULL == window) {
            break;
        }
//        GestureManager::instance()->enableSystemGesture(WL_SYSTEM_GESTURE_TYPE_FLAG_ALL);
//        GestureManager::instance()->enableCommonGesture(window, WL_COMMON_GESTURE_TYPE_FLAG_ALL);

        glViewport(0, 0, DEFAULT_SIZE_WIDTH, DEFAULT_SIZE_HEIGHT);
//        render.initRender();
//        render.createRootWindow();

//        Director::instance()->init();
//        Director::instance()->setWindowSize(DEFAULT_SIZE_WIDTH, DEFAULT_SIZE_HEIGHT);
//        loadBmp(0, "/xxxx/Cube.bmp", 150, 150);
//        for (int i = 0; i < 6; ++i)
//        Director::instance()->setPlaneTexture(i, 0);


        while (!glfwWindowShouldClose(window)) {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glfwSwapBuffers(window);

//            if (render.loop()) {
//                glfwSwapBuffers(window);
//            }

//            Director::instance()->loop();
//            glfwSwapBuffers(window);

            glfwPollEvents();
//            GestureManager::instance()->update();
        }
    } while (0);

//    Director::instance()->deinit();
//    Director::destroy();
//    render.deinitRender();
    glfwTerminate();
    return 0;

}

#endif
