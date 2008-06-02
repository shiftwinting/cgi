//                    -- main.hpp --
//
//           Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
//
//[fcgi_hello_world
//
// The simplest FastCGI program, outputs only "Hello there, universe."
//

#include <iomanip>
#include <fstream>
#include <boost/cgi/fcgi.hpp>
#include <boost/cgi.hpp>
#include <boost/asio.hpp>
#include <sys/socket.h>

using namespace std;
using namespace boost::fcgi;

template<typename Request, typename Response>
int handle_request(Request& req, Response& resp)
{
  // This is a minimal response. The content_type(...) may go before or after
  // the response text.
  resp<< content_type("text/plain")
      << "Hello there, universe.";

  return_(resp, req, 0);
}

/// Handle a vanilla CGI request
int handle_request()
{
  boost::cgi::request req;
  response resp;
  return handle_request(req,resp);
}

/// Handle a FastCGI request
template<typename Acceptor>
int handle_request(Acceptor& a)
{
  request req(a.protocol_service());   // Our request.

  int ret = 0;
  for (;;) // Handle requests until something goes wrong
           // (an exception will be thrown).
  {
    a.accept(req);
    response resp;    // A response object to make our lives easier.
    ret = handle_request(req, resp);
  }

  return ret;
}

int main()
{
try
{
  service s;        // This becomes useful with async operations.
  acceptor a(s);
  ofstream file("/var/www/log/is_cgi.log", std::ios::app);

  if (a.is_cgi())
  {
    file<< "Is CGI" << endl;
    file.flush();
    handle_request();
  }
  else
  {
    file<< "Is FastCGI (probably)";
    file.flush();
    handle_request(a);
  }

  file.flush();
  
  return 0;
}
catch(boost::system::system_error& err)
{
  std::cerr<< "System error " << err.code() << ": "
           << err.what() << std::endl;
  return 1;
}
catch(...)
{
  std::cerr<< "Unknown error!" << std::endl;
  return 2;
}
}
//]

