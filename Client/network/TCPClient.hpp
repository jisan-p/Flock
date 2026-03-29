#pragma once

#include <string>
#include <mutex>
#include <thread>
#include <atomic>
#include <functional>
#include <vector>

class TCPClient {
private:
    int sock;                     
    std::string serverIP;        
    int serverPort;
    
    std::atomic<bool> connected; 

    //listener thread for incoming messages
    std::thread listenerThread;
    std::atomic<bool> listening;

    //callback
    std::function<void(const std::string&)> messageCallback;

    //thread for sends
    std::mutex sendMutex;

    //listener loop on its own threads
    void listenerLoop();

public:
    //stored after login
    std::string myUserID;
    std::string myUsername;

    TCPClient(const std::string& ip = "127.0.0.1", int port = 4444);

    ~TCPClient();

    //connection
    bool connectToServer();
    void disconnect();
    bool isConnected() const;

    //send raw protocol command and get response
    std::string sendCommand(const std::string& command);

    bool login(const std::string& username, const std::string& password);
    bool registerUser(const std::string& username, const std::string& password);
    std::vector<std::string> listUsers();
    bool sendMessage(const std::string& targetUser, const std::string& message);
    std::vector<std::string> getChatHistory(const std::string& targetUser);

    void sendOnly(const std::string& command);

    void setMessageCallback(std::function<void(const std::string&)> callback);

    void startListening();
    void stopListening();
};
