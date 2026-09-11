#include "../src/protocol/resp.h"

#include <arpa/inet.h>
#include <cassert>
#include <chrono>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

namespace
{

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

  std::string sendCommand(
      int socketFd,
      const std::string &command)
  {
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

    return std::string(
        buffer,
        static_cast<std::size_t>(
            received));
  }

}

void testPing(int socketFd)
{
  std::string response =
      sendCommand(
          socketFd,
          "*1\r\n"
          "$4\r\n"
          "PING\r\n");

  assert(response == "+PONG\r\n");

  std::cout << "PASS: server PING" << std::endl;
}

void testSetAndGet(int socketFd)
{
  std::string response =
      sendCommand(
          socketFd,
          "*3\r\n"
          "$3\r\n"
          "SET\r\n"
          "$1\r\n"
          "A\r\n"
          "$5\r\n"
          "Hello\r\n");

  assert(response == "+OK\r\n");

  response =
      sendCommand(
          socketFd,
          "*2\r\n"
          "$3\r\n"
          "GET\r\n"
          "$1\r\n"
          "A\r\n");

  assert(
      response ==
      "$5\r\n"
      "Hello\r\n");

  std::cout << "PASS: server SET and GET" << std::endl;
}

void testExistsAndDelete(int socketFd)
{
  std::string response =
      sendCommand(
          socketFd,
          "*2\r\n"
          "$6\r\n"
          "EXISTS\r\n"
          "$1\r\n"
          "A\r\n");

  assert(response == ":1\r\n");

  response =
      sendCommand(
          socketFd,
          "*2\r\n"
          "$3\r\n"
          "DEL\r\n"
          "$1\r\n"
          "A\r\n");

  assert(response == ":1\r\n");

  response =
      sendCommand(
          socketFd,
          "*2\r\n"
          "$6\r\n"
          "EXISTS\r\n"
          "$1\r\n"
          "A\r\n");

  assert(response == ":0\r\n");

  std::cout
      << "PASS: server EXISTS and DEL"
      << std::endl;
}

void testSetWithTTL(int socketFd)
{
  std::string response =
      sendCommand(
          socketFd,
          "*5\r\n"
          "$3\r\n"
          "SET\r\n"
          "$1\r\n"
          "B\r\n"
          "$1\r\n"
          "2\r\n"
          "$2\r\n"
          "EX\r\n"
          "$2\r\n"
          "10\r\n");

  assert(response == "+OK\r\n");

  response =
      sendCommand(
          socketFd,
          "*2\r\n"
          "$3\r\n"
          "GET\r\n"
          "$1\r\n"
          "B\r\n");

  assert(
      response ==
      "$1\r\n"
      "2\r\n");

  response =
      sendCommand(
          socketFd,
          "*2\r\n"
          "$3\r\n"
          "TTL\r\n"
          "$1\r\n"
          "B\r\n");

  assert(
      response == ":10\r\n" ||
      response == ":9\r\n");

  std::cout
      << "PASS: server SET EX and TTL"
      << std::endl;
}

void testExpire(int socketFd)
{
  std::string response =
      sendCommand(
          socketFd,
          "*3\r\n"
          "$3\r\n"
          "SET\r\n"
          "$1\r\n"
          "C\r\n"
          "$5\r\n"
          "Hello\r\n");

  assert(response == "+OK\r\n");

  response =
      sendCommand(
          socketFd,
          "*3\r\n"
          "$6\r\n"
          "EXPIRE\r\n"
          "$1\r\n"
          "C\r\n"
          "$1\r\n"
          "5\r\n");

  assert(response == ":1\r\n");

  response =
      sendCommand(
          socketFd,
          "*2\r\n"
          "$3\r\n"
          "TTL\r\n"
          "$1\r\n"
          "C\r\n");

  assert(
      response == ":5\r\n" ||
      response == ":4\r\n");

  std::cout
      << "PASS: server EXPIRE"
      << std::endl;
}

void testPipelining(int socketFd)
{
  std::string pipeline =
      "*1\r\n"
      "$4\r\n"
      "PING\r\n"
      "*3\r\n"
      "$3\r\n"
      "SET\r\n"
      "$1\r\n"
      "D\r\n"
      "$1\r\n"
      "4\r\n"
      "*2\r\n"
      "$3\r\n"
      "GET\r\n"
      "$1\r\n"
      "D\r\n";

  ssize_t sent =
      send(
          socketFd,
          pipeline.c_str(),
          pipeline.size(),
          0);

  assert(
      sent ==
      static_cast<ssize_t>(
          pipeline.size()));

  std::string response;

  char buffer[4096];

  while (response.find("$1\r\n4\r\n") ==
         std::string::npos)
  {
    ssize_t received =
        recv(
            socketFd,
            buffer,
            sizeof(buffer),
            0);

    assert(received > 0);

    response.append(
        buffer,
        static_cast<std::size_t>(
            received));
  }

  assert(
      response.find("+PONG\r\n") !=
      std::string::npos);

  assert(
      response.find("+OK\r\n") !=
      std::string::npos);

  assert(
      response.find(
          "$1\r\n"
          "4\r\n") !=
      std::string::npos);

  std::cout
      << "PASS: server pipelining"
      << std::endl;
}

void testUnknownCommand(int socketFd)
{
  std::string response =
      sendCommand(
          socketFd,
          "*1\r\n"
          "$4\r\n"
          "NOPE\r\n");

  assert(
      response ==
      "-ERR unknown command\r\n");

  std::cout
      << "PASS: server unknown command"
      << std::endl;
}

int main()
{
  int socketFd =
      connectToServer();

  assert(
      socketFd >= 0);

  testPing(socketFd);
  testSetAndGet(socketFd);
  testExistsAndDelete(socketFd);
  testSetWithTTL(socketFd);
  testExpire(socketFd);
  testPipelining(socketFd);
  testUnknownCommand(socketFd);

  close(socketFd);

  std::cout << std::endl;
  std::cout
      << "All Server integration tests passed!"
      << std::endl;

  return 0;
}