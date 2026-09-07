#pragma once

#include "../commands/command_handler.h"

class Server
{
private:
  int serverSocket;
  int port;

  Database database;
  CommandHandler commandHandler;

public:
  explicit Server(int port);

  ~Server();

  void start();

private:
  void handleClient(int clientSocket);
};