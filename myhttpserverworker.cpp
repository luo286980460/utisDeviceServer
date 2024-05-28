#include "myhttpserverworker.h"
#include "include/HttpServer.h"
#include "include/hthread.h"    // import hv_gettid
#include "include/hasync.h"     // import hv::async

#include <QSettings>
#include <QCoreApplication>
#include <QDebug>
#include <QJsonDocument>
#include <QUrlQuery>

using namespace hv;

#define TEST_HTTPS 0

MyHttpServerWorker::MyHttpServerWorker(QObject *parent)
    : QObject{parent}
{

}

void MyHttpServerWorker::initWeatherData()
{
    m_weatherData.insert("fengXiang","/");          // 风向
    m_weatherData.insert("fengSu","/");             // 风速
    m_weatherData.insert("jiangYuLiang","/");       // 降雨量
    m_weatherData.insert("nengJianDu","/");         // 能见度
    m_weatherData.insert("shiDu","/");              // 湿度
    m_weatherData.insert("qiYa","/");               // 气压
    m_weatherData.insert("huanJingWenDu","/");      // 环境温度
    m_weatherData.insert("fuBingHouDu","/");        // 覆冰厚度
    m_weatherData.insert("luMianWenDu","/");        // 路面温度
    m_weatherData.insert("shiHuaXiShu","/");        // 湿滑系数
    m_weatherData.insert("luMianZhuangTai","/");    // 路面状态
    m_weatherData.insert("fuXueHouDu","/");         // 覆雪厚度
    m_weatherData.insert("yingJianZhuangTai","/");  // 硬件状态
    m_weatherData.insert("suhiMoHouDu","/");        // 水膜厚度
}

void MyHttpServerWorker::slotStart()
{
    HV_MEMCHECK;

    HttpService router;

    router.Static("/", "./html");
    router.EnableForwardProxy();
    router.AddTrustProxy("*httpbin.org");
    router.Proxy("/httpbin/", "http://httpbin.org/");

    /*          POST            */

    // 气象
    router.POST("/Weather/GetInfo", [=](HttpRequest* req, HttpResponse* resp) {Q_UNUSED(req);
        qDebug() << "req: " << QJsonDocument::fromJson(QString::fromStdString(req->Body()).toLocal8Bit());



        Json ex3 = {
            {"Res", "/"},
            {"FengXiang", "/"},
            {"FengSu", "/"},
            {"JiangYuLiang", "/"},
            {"NengJianDu", "/"},
            {"ShiDu", "/"},
            {"QiYa", "/"},
            {"HuanJingWenDu", "/"},
            {"FuBingHouDu", "/"},
            {"LuMianWenDu", "/"},
            {"ShiHuaXiShu", "/"},
            {"LuMianZhuangTai", "/"},
            {"FuXueHouDu", "/"},
            {"YingJianZhuangTai", "/"},
            {"ShuiMoHouDu", "/"},
        };
        //return resp->Json(ex3);//m_weatherData
        return resp->String(QJsonDocument(m_weatherData).toJson().data());
    });




    // curl -v http://ip:port/DEVICE/DEBUG/reboot
    router.POST("/DEVICE/DEBUG/reboot", [=](HttpRequest* req, HttpResponse* resp) {Q_UNUSED(req);
        /*发出信号，2s以后重启*/
        return resp->String("reboot");
    });
    router.POST("/DEVICE/DEBUG/poweroff", [=](HttpRequest* req, HttpResponse* resp) {Q_UNUSED(req);
        /*发出信号，2s以后关机*/
        return resp->String("poweroff");
    });

    // curl -v http://ip:port/echo -d "hello,world!"
    router.POST("/echo", [](const HttpContextPtr& ctx) {
        return ctx->send(ctx->body(), ctx->type());
    });

//    router.POST("/echo", [](HttpRequest* req, HttpResponse* resp) {
//        resp->content_type = req->content_type;
//        resp->body = req->body;
//        return resp->String("poweroff");

//    });

    /*          GET            */
    /* API handlers */
    // curl -v http://ip:port/ping
    router.GET("/ping", [](HttpRequest* req, HttpResponse* resp) {
        Q_UNUSED(req);
        Json ex3 = {
                    {"happy", true},
                    {"pi", 3.141},
                    };
        return resp->Json(ex3);
        //return resp->String("connected............");
    });

    // curl -v http://ip:port/data
    router.GET("/data", [](HttpRequest* req, HttpResponse* resp) {Q_UNUSED(req);
        static char data[] = "0123456789";
        return resp->Data(data, 10 /*, false */);
    });

    // curl -v http://ip:port/paths
    router.GET("/paths", [&router](HttpRequest* req, HttpResponse* resp) {Q_UNUSED(req);
        return resp->Json(router.Paths());
    });

    // curl -v http://ip:port/get?env=1
    router.GET("/get", [](const HttpContextPtr& ctx) {
        hv::Json resp;
        resp["origin"] = ctx->ip();
        resp["url"] = ctx->url();
        resp["args"] = ctx->params();
        resp["headers"] = ctx->headers();
        return ctx->send(resp.dump(2));
    });

    // curl -v http://ip:port/user/123
    router.GET("/user/{id}", [](const HttpContextPtr& ctx) {
        hv::Json resp;
        resp["id"] = ctx->param("id");
        return ctx->send(resp.dump(2));
    });

    // curl -v http://ip:port/async
    router.GET("/async", [](const HttpRequestPtr& req, const HttpResponseWriterPtr& writer) {Q_UNUSED(req);
        writer->Begin();
        writer->WriteHeader("X-Response-tid", hv_gettid());
        writer->WriteHeader("Content-Type", "text/plain");
        writer->WriteBody("This is an async response.\n");
        writer->End();
    });

    // middleware
    router.AllowCORS();
    router.Use([](HttpRequest* req, HttpResponse* resp) {Q_UNUSED(req);
        resp->SetHeader("X-Request-tid", hv::to_string(hv_gettid()));
        return HTTP_STATUS_NEXT;
    });

    HttpServer server;
    server.service = &router;
    server.port = m_port;
#if TEST_HTTPS
    server.https_port = 8443;
    hssl_ctx_opt_t param;
    memset(&param, 0, sizeof(param));
    param.crt_file = "cert/server.crt";
    param.key_file = "cert/server.key";
    param.endpoint = HSSL_SERVER;
    if (server.newSslCtx(&param) != 0) {
        fprintf(stderr, "new SSL_CTX failed!\n");
        return -20;
    }
#endif

    // uncomment to test multi-processes
    // server.setProcessNum(4);
    // uncomment to test multi-threads
    // server.setThreadNum(4);

    server.start();

    // press Enter to stop
    while (getchar() != '\n');
    hv::async::cleanup();
}
