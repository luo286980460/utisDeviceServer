QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    myhttpserver.cpp \
    myhttpserverworker.cpp \
    widget.cpp

HEADERS += \
    include/AsyncHttpClient.h \
    include/Buffer.h \
    include/Channel.h \
    include/Event.h \
    include/EventLoop.h \
    include/EventLoopThread.h \
    include/EventLoopThreadPool.h \
    include/HttpClient.h \
    include/HttpContext.h \
    include/HttpMessage.h \
    include/HttpParser.h \
    include/HttpResponseWriter.h \
    include/HttpServer.h \
    include/HttpService.h \
    include/Status.h \
    include/TcpClient.h \
    include/TcpServer.h \
    include/ThreadLocalStorage.h \
    include/UdpClient.h \
    include/UdpServer.h \
    include/WebSocketChannel.h \
    include/WebSocketClient.h \
    include/WebSocketParser.h \
    include/WebSocketServer.h \
    include/axios.h \
    include/base64.h \
    include/hasync.h \
    include/hatomic.h \
    include/hbase.h \
    include/hbuf.h \
    include/hconfig.h \
    include/hdef.h \
    include/hdir.h \
    include/hendian.h \
    include/herr.h \
    include/hexport.h \
    include/hfile.h \
    include/hlog.h \
    include/hloop.h \
    include/hmain.h \
    include/hmap.h \
    include/hmath.h \
    include/hmutex.h \
    include/hobjectpool.h \
    include/hpath.h \
    include/hplatform.h \
    include/hproc.h \
    include/hscope.h \
    include/hsocket.h \
    include/hssl.h \
    include/hstring.h \
    include/hsysinfo.h \
    include/hthread.h \
    include/hthreadpool.h \
    include/htime.h \
    include/http_content.h \
    include/httpdef.h \
    include/hurl.h \
    include/hv.h \
    include/hversion.h \
    include/ifconfig.h \
    include/iniparser.h \
    include/json.hpp \
    include/md5.h \
    include/nlog.h \
    include/requests.h \
    include/sha1.h \
    include/singleton.h \
    include/wsdef.h \
    myhttpserver.h \
    myhttpserverworker.h \
    widget.h

FORMS += \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32: LIBS += -L$$PWD/lib/ -lhv

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include
