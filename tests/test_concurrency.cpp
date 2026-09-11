#include <arpa/inet.h>
#include <cassert>
#include <chrono>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

int connectToServer()
{
  int socketFd =
      socket(
          AF_INET,
          SOCK_STREAM,
          0);

  assert(socketFd >= 0);

  sockaddr_in serverAddress{};

  serverAddress.sin_family =
      AF_INET;

  serverAddress.sin_port =
      htons(6379);

  int result =
      inet_pton(
          AF_INET,
          "127.0.0.1",
          &serverAddress.sin_addr);

  assert(result == 1);

  for (int attempt = 0; attempt < 20; ++attempt)
  {
    result =
        connect(
            socketFd,
            reinterpret_cast<sockaddr *>(
                &serverAddress),
            sizeof(serverAddress));

    if (result == 0)
    {
      return socketFd;
    }

    std::this_thread::sleep_for(
        std::chrono::milliseconds(100));
  }

  close(socketFd);

  return -1;
}

void clientWorker(
    int clientNumber)
{
  int socketFd =
      connectToServer();

  assert(socketFd >= 0);

  std::string key =
      "CLIENT_" +
      std::to_string(clientNumber);

  std::string value =
      "VALUE_" +
      std::to_string(clientNumber);

  std::string command =
      "*3\r\n"
      "$3\r\n"
      "SET\r\n"
      "$" +
      std::to_string(key.size()) +
      "\r\n" +
      key +
      "\r\n"
      "$" +
      std::to_string(value.size()) +
      "\r\n" +
      value +
      "\r\n";

  ssize_t sent =
      send(
          socketFd,
          command.c_str(),
          command.size(),
          0);

  assert(
      sent ==
      static_cast<ssize_t>(
          command.size()));

  char buffer[4096];

  ssize_t received =
      recv(
          socketFd,
          buffer,
          sizeof(buffer),
          0);

  assert(received > 0);

  std::string response(
      buffer,
      static_cast<std::size_t>(
          received));

  assert(
      response ==
      "+OK\r\n");

  close(socketFd);
}

int main()
{
  const int clientCount = 10;

  std::thread clients[clientCount];

  for (int i = 0; i < clientCount; ++i)
  {
    clients[i] =
        std::thread(
            clientWorker,
            i);
  }

  for (int i = 0; i < clientCount; ++i)
  {
    clients[i].join();
  }

  std::cout
      << "PASS: "
      << clientCount
      << " concurrent clients"
      << std::endl;

  std::cout << std::endl;

  std::cout
      << "All concurrency tests passed!"
      << std::endl;

  return 0;
}