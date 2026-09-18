#include "server.h"

#include "../protocol/resp.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <fcntl.h>
#include <chrono>
#include <csignal>
namespace
{
    volatile std::sig_atomic_t shutdownRequested = 0;

    void handleShutdownSignal(int)
    {
        shutdownRequested = 1;
    }

    bool sendAll(
        int socket,
        const std::string &response)
    {
        std::size_t totalSent = 0;

        while (totalSent < response.size())
        {
            ssize_t bytesSent =
                send(
                    socket,
                    response.data() + totalSent,
                    response.size() - totalSent,
                    0);

            if (bytesSent <= 0)
            {
                return false;
            }

            totalSent +=
                static_cast<std::size_t>(
                    bytesSent);
        }

        return true;
    }
}
Server::Server(int port)
    : serverSocket(-1),
      port(port),
      database(),
      metrics(),
      commandHandler(database, metrics),
      snapshotFile("neuracache.snapshot")
{
    database.setMetrics(&metrics);
}

Server::~Server()
{
    if (serverSocket != -1)
    {
        close(serverSocket);
    }
}

void Server::start()
{
    std::signal(
        SIGINT,
        handleShutdownSignal);

    if (database.loadSnapshot(snapshotFile))
    {
        std::cout
            << "Loaded snapshot from "
            << snapshotFile
            << std::endl;
    }
    else
    {
        std::cout
            << "No snapshot loaded"
            << std::endl;
    }

    serverSocket =
        socket(
            AF_INET,
            SOCK_STREAM,
            0);

    if (serverSocket < 0)
    {
        throw std::runtime_error(
            "Failed to create socket");
    }

    int option = 1;

    setsockopt(
        serverSocket,
        SOL_SOCKET,
        SO_REUSEADDR,
        &option,
        sizeof(option));

    sockaddr_in serverAddress{};

    serverAddress.sin_family =
        AF_INET;

    serverAddress.sin_addr.s_addr =
        INADDR_ANY;

    serverAddress.sin_port =
        htons(port);

    if (bind(
            serverSocket,
            reinterpret_cast<sockaddr *>(
                &serverAddress),
            sizeof(serverAddress)) < 0)
    {
        close(serverSocket);

        serverSocket = -1;

        throw std::runtime_error(
            "Failed to bind socket");
    }

    if (listen(
            serverSocket,
            16) < 0)
    {
        close(serverSocket);

        serverSocket = -1;

        throw std::runtime_error(
            "Failed to listen");
    }

    int flags =
        fcntl(
            serverSocket,
            F_GETFL,
            0);

    if (flags < 0 ||
        fcntl(
            serverSocket,
            F_SETFL,
            flags | O_NONBLOCK) < 0)
    {
        close(serverSocket);

        serverSocket = -1;

        throw std::runtime_error(
            "Failed to set non-blocking mode");
    }

    std::cout
        << "NeuraCache running on port "
        << port
        << std::endl;

    while (!shutdownRequested)
    {
        sockaddr_in clientAddress{};

        socklen_t clientLength =
            sizeof(clientAddress);

        int clientSocket =
            accept(
                serverSocket,
                reinterpret_cast<sockaddr *>(
                    &clientAddress),
                &clientLength);

        if (clientSocket < 0)
        {
            if (errno == EAGAIN ||
                errno == EWOULDBLOCK)
            {
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(10));

                continue;
            }

            if (errno == EINTR)
            {
                continue;
            }

            std::cerr
                << "Failed to accept client"
                << std::endl;

            continue;
        }

        std::cout
            << "Client connected"
            << std::endl;

        std::thread clientThread(
            &Server::handleClient,
            this,
            clientSocket);

        clientThread.detach();
    }

    std::cout
        << "Shutting down NeuraCache..."
        << std::endl;

    if (database.saveSnapshot(
            snapshotFile))
    {
        std::cout
            << "Snapshot saved to "
            << snapshotFile
            << std::endl;
    }
    else
    {
        std::cerr
            << "Failed to save snapshot"
            << std::endl;
    }

    close(serverSocket);

    serverSocket = -1;
}
void Server::handleClient(
    int clientSocket)
{
    char buffer[4096];

    std::string inputBuffer;

    while (true)
    {
        ssize_t bytesReceived =
            recv(
                clientSocket,
                buffer,
                sizeof(buffer),
                0);

        if (bytesReceived <= 0)
        {
            break;
        }

        inputBuffer.append(
            buffer,
            static_cast<std::size_t>(
                bytesReceived));

        while (!inputBuffer.empty())
        {
            std::vector<std::string> command;

            std::size_t consumedBytes = 0;

            RESP::ParseResult result =
                RESP::parseCommand(
                    inputBuffer,
                    command,
                    consumedBytes);

            if (result ==
                RESP::ParseResult::Incomplete)
            {
                break;
            }

            if (result ==
                RESP::ParseResult::Invalid)
            {
                std::string response =
                    RESP::encodeError(
                        "invalid request");

                if (!sendAll(
                        clientSocket,
                        response))
                {
                    break;
                }

                inputBuffer.clear();

                break;
            }

            std::string response =
                commandHandler.execute(
                    command);

            if (!sendAll(
                    clientSocket,
                    response))
            {
                break;
            }

            inputBuffer.erase(
                0,
                consumedBytes);
        }
    }

    close(clientSocket);
}