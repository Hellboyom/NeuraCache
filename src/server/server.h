#pragma once

#include "../commands/command_handler.h"
#include "../storage/database.h"
#include "../metrics/metrics.h"

#include <string>

class Server
{
private:
  int serverSocket;
  int port;

  Metrics metrics;
  Database database;
  CommandHandler commandHandler;

  std::string snapshotFile;

  void handleClient(
      int clientSocket);

public:
  explicit Server(
      int port);

  ~Server();

  void start();
};