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

Server::Server(int port)
    : serverSocket(-1),
      port(port),
      database(),
      commandHandler(database)
{
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
    serverSocket = socket(
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

    serverAddress.sin_family = AF_INET;

    serverAddress.sin_addr.s_addr =
        INADDR_ANY;

    serverAddress.sin_port =
        htons(port);

    if (
        bind(
            serverSocket,
            reinterpret_cast<sockaddr *>(
                &serverAddress),
            sizeof(serverAddress)) < 0)
    {
        close(serverSocket);

        throw std::runtime_error(
            "Failed to bind socket");
    }

    if (
        listen(
            serverSocket,
            16) < 0)
    {
        close(serverSocket);

        throw std::runtime_error(
            "Failed to listen");
    }

    std::cout
        << "NeuraCache running on port "
        << port
        << std::endl;

    while (true)
    {
        sockaddr_in clientAddress{};
        socklen_t clientLength = sizeof(clientAddress);

        int clientSocket =
            accept(
                serverSocket,
                reinterpret_cast<sockaddr *>(&clientAddress),
                &clientLength);

        if (clientSocket < 0)
        {
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
}

void Server::handleClient(int clientSocket)
{
    char buffer[4096];

    std::string inputBuffer;

    while (true)
    {
        ssize_t bytesReceived = recv(
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
            static_cast<std::size_t>(bytesReceived));

        while (!inputBuffer.empty())
        {
            std::vector<std::string> command;

            std::size_t consumedBytes = 0;

            RESP::ParseResult result =
                RESP::parseCommand(
                    inputBuffer,
                    command,
                    consumedBytes);

            if (result == RESP::ParseResult::Incomplete)
            {
                break;
            }

            if (result == RESP::ParseResult::Invalid)
            {
                std::string response =
                    RESP::encodeError("invalid request");

                send(
                    clientSocket,
                    response.c_str(),
                    response.size(),
                    0);

                inputBuffer.clear();

                break;
            }

            std::string response =
                commandHandler.execute(command);

            send(
                clientSocket,
                response.c_str(),
                response.size(),
                0);

            inputBuffer.erase(
                0,
                consumedBytes);
        }
    }
}
