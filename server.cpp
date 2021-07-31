#include <drogon/drogon.h>
#include <drogon/WebSocketController.h>
#include <chrono>
#include <atomic>
#include <thread>

using namespace drogon;

std::atomic<uint64_t> numMessages;
std::atomic<uint64_t> numConnections;
std::chrono::system_clock::time_point startTime;

class BenchmarkWsController : public drogon::WebSocketController<BenchmarkWsController>
{
public:
    BenchmarkWsController()
    {
        numMessages = 0;
        numConnections = 0;
        startTime = std::chrono::system_clock::now();
    }
    virtual void handleNewMessage(const WebSocketConnectionPtr &conn,
                                  std::string &&,
                                  const WebSocketMessageType &type) override
    {
        numMessages++;
        if(type == WebSocketMessageType::Text)
        {
            conn->send("Hello from drogon!");
        }
    }
    virtual void handleConnectionClosed(
        const WebSocketConnectionPtr &) override
    {
        numConnections--;
    }
    virtual void handleNewConnection(const HttpRequestPtr &,
                                     const WebSocketConnectionPtr &) override
    {
        numConnections++;
    }

    WS_PATH_LIST_BEGIN
        WS_PATH_ADD("/benchmark", {}, Get);
    WS_PATH_LIST_END
};

int main()
{
    std::thread msgThr([](){
        while(1)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            auto now = std::chrono::system_clock::now();
            size_t nMessages = numMessages;
            size_t nConnections = numConnections;
            double timeSpent = std::chrono::duration_cast<std::chrono::duration<double>>(now - startTime).count();
            
            std::cout << "================================\n"
                << "connections: " << nConnections << "\n"
                << "total messages: " << nMessages << "\n"
                << "messages/min: " << 60.0*numMessages/timeSpent << std::endl;
        }
    });
    app().setThreadNum(0)
        // Must use IP since the event loop isn't up yet.
        //            vvvv
        .addListener("127.0.0.1", 8848)
        .run();
    msgThr.join();
    return 0;
}
