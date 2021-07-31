#include <drogon/drogon.h>
#include <drogon/HttpClient.h>
#include <drogon/WebSocketClient.h>
#include <list>

using namespace drogon;
std::list<WebSocketClientPtr> clientHolder;

void runWsOnAllThread()
{
    size_t numClient = 900;
    for(size_t i=0;i<numClient;i++)
    {
        auto loop = app().getLoop();
        auto wsClient = WebSocketClient::newWebSocketClient("ws://127.0.0.1:8848", loop);
        auto req = HttpRequest::newHttpRequest();
        req->setPath("/benchmark");
        wsClient->setMessageHandler([](const std::string &message,
                                const WebSocketClientPtr &wsPtr,
                                const WebSocketMessageType &type) {
            if(type == WebSocketMessageType::Text)
            {
                wsPtr->getConnection()->send("Hello from client!");
            }
        });
        wsClient->connectToServer(req, [](ReqResult r,
            const HttpResponsePtr &,
            const WebSocketClientPtr &wsPtr) {

            wsPtr->getConnection()->send("Hello from client!");
        });
        clientHolder.push_back(wsClient);
    }
}

int main()
{

    app().getLoop()->queueInLoop(runWsOnAllThread);
    app()
        .setThreadNum(0)
        .run();
    return 0;
}
