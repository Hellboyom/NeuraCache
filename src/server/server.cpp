#include "server.h"

#include "../protocol/resp.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

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

    socklen_t clientLength =
        sizeof(clientAddress);

    int clientSocket = accept(
        serverSocket,
        reinterpret_cast<sockaddr *>(
            &clientAddress),
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

    handleClient(clientSocket);

    close(clientSocket);

    std::cout
        << "Client disconnected"
        << std::endl;
  }
}

void Server::handleClient(
    int clientSocket)
{
  char receiveBuffer[4096];

  // --------------------------------------------------
  // Persistent TCP receive buffer
  // --------------------------------------------------
  //
  // Data can arrive in pieces.
  //
  // Example:
  //
  // recv #1:
  // "*3\r\n$3\r\nSE"
  //
  // recv #2:
  // "T\r\n$3\r\nfoo\r\n$3\r\nbar\r\n"
  //
  // We keep both pieces here until a complete
  // RESP command exists.
  //

  std::string inputBuffer;

  while (true)
  {
    ssize_t bytesReceived = recv(
        clientSocket,
        receiveBuffer,
        sizeof(receiveBuffer),
        0);

    // Client disconnected.
    if (bytesReceived == 0)
    {
      break;
    }

    // Socket error.
    if (bytesReceived < 0)
    {
      if (errno == EINTR)
      {
        continue;
      }

      std::cerr
          << "Receive error: "
          << std::strerror(errno)
          << std::endl;

      break;
    }

    // Add newly received bytes to our
    // persistent TCP buffer.
    inputBuffer.append(
        receiveBuffer,
        static_cast<std::size_t>(
            bytesReceived));

    // --------------------------------------------------
    // Process every complete command currently
    // available in the buffer.
    // --------------------------------------------------

    while (true)
    {
      std::vector<std::string> command;

      std::size_t consumedBytes = 0;

      bool parsed = RESP::parseCommand(
          inputBuffer,
          command,
          consumedBytes);

      if (!parsed)
      {
        // Command is incomplete.
        //
        // Keep the bytes and wait for
        // another recv().
        break;
      }

      // --------------------------------------------------
      // Remove the command we just processed.
      //
      // Anything after consumedBytes belongs to
      // the next command.
      // --------------------------------------------------

      inputBuffer.erase(
          0,
          consumedBytes);

      // --------------------------------------------------
      // Execute command
      // --------------------------------------------------

      std::string response =
          commandHandler.execute(command);

      // --------------------------------------------------
      // Send response
      // --------------------------------------------------

      std::size_t totalSent = 0;

      while (
          totalSent < response.size())
      {
        ssize_t bytesSent = send(
            clientSocket,
            response.data() + totalSent,
            response.size() - totalSent,
            0);

        if (bytesSent <= 0)
        {
          return;
        }

        totalSent +=
            static_cast<std::size_t>(
                bytesSent);
      }
    }
  }
}