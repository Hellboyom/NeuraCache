#pragma once

#include "../commands/command_handler.h"
#include "../storage/database.h"

class Server
{
private:
  int serverSocket;
  int port;

  Database database;
  CommandHandler commandHandler;

  void handleClient(
      int clientSocket);

public:
  explicit Server(int port);

  ~Server();

  void start();
};