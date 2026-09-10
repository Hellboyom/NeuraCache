#include "server/server.h"

#include <exception>
#include <iostream>

int main()
{
  try
  {
    Server server(6379);

    server.start();
  }
  catch (const std::exception &exception)
  {
    std::cerr
        << "Fatal error: "
        << exception.what()
        << std::endl;

    return 1;
  }

  return 0;
}