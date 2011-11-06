/*!
 * @file ws-transport.cpp
 * @brief Simple program that pipes standard streams to a websocket connection.
 *
 * @author Andre Caron (andre.louis.caron@usherbrooke.ca)
 */

#include "ws.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <stdint.h>

#include <string.h>
#include <stddef.h>
#include <stdlib.h>

#include <iostream>
#include <sstream>
#include "Response.hpp"

#define MAX(a,b) (((a)<(b))?(a):(b))

namespace net {

    class Waitables
    {
      ::fd_set myData;
      int myNfds;
    public:
      Waitables ()
	: myNfds(0)
      {
	clear();
      }

      Waitables ( const Waitables& waitables )
      {
	::memcpy(&myData, &waitables.myData, sizeof(::fd_set));
	myNfds = waitables.myNfds;
      }

      ~Waitables ()
      {
	clear();
      }

      int capacity () const
      {
	return (FD_SETSIZE);
      }

      void clear ()
      {
	FD_ZERO(&myData);
      }

      bool contains ( int handle ) const
      {
	return (FD_ISSET(handle, &myData));
      }

      Waitables& add ( int handle )
      {
	FD_SET(handle, &myData);
	myNfds = MAX(myNfds, handle);
	return (*this);
      }

      Waitables& del ( int handle )
      {
	FD_CLR(handle, &myData); return (*this);
      }

      int nfds () const
      {
	return (myNfds+1);
      }

      ::fd_set& data ()
      {
	return (myData);
      }

      const ::fd_set& data () const
      {
	return (myData);
      }
    };

  bool iready ( Waitables& pull )
  {
    const int status = ::select(pull.nfds(), &pull.data(), 0, 0, 0);
    if ( status == -1 ) {
      std::cerr << "Select!" << std::endl;
    }
    return (status > 0);
  }

  bool oready ( Waitables& push )
  {
    const int status = ::select(push.nfds(), 0, &push.data(), 0, 0);
    if ( status == -1 ) {
      std::cerr << "Select!" << std::endl;
    }
    return (status > 0);
  }

  bool eready ( Waitables& fail )
  {
    const int status = ::select(fail.nfds(), 0, 0, &fail.data(), 0);
    if ( status == -1 ) {
      std::cerr << "Select!" << std::endl;
    }
    return (status > 0);
  }

  class Endpoint
  {
    ::sockaddr_in myData;
  public:
    Endpoint ( uint16_t port )
    {
      myData.sin_family = AF_INET;
      myData.sin_addr.s_addr = INADDR_ANY;
      myData.sin_port = ::htons(port);
    }

    Endpoint ( uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint16_t port )
    {
      myData.sin_family = AF_INET;
      myData.sin_addr.s_addr =
	(uint32_t(a) << 24)|
	(uint32_t(b) << 16)|
	(uint32_t(c) <<  8)|
	(uint32_t(d) <<  0);
      myData.sin_port = ::htons(port);
    }

    ::sockaddr_in& data ()
    {
      return (myData);
    }

    const ::sockaddr_in& data () const
    {
      return (myData);
    }
  };

  class Stream
  {
    int myHandle;
    Stream ( const Stream& );
    Stream& operator= ( const Stream& );
  public:
    Stream ( Endpoint endpoint )
      : myHandle(::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))
    {
      if ( myHandle < 0 ) {
	std::cerr << "socket()" << std::endl;
      }
      ::sockaddr * address = reinterpret_cast< ::sockaddr* >(&endpoint.data());
#if 0
      const int status = ::connect(
        myHandle, address, sizeof(endpoint.data()));
      if ( status < 0 ) {
	std::cerr << "connect()" << std::endl;
      }
#endif
    }

    ~Stream ()
    {
      ::close(myHandle);
    }

    int handle () const
    {
      return (myHandle);
    }

    ssize_t get ( void * data, size_t size )
    {
      const ssize_t status = ::recv(myHandle, data, size, 0);
      if ( status < 0 ) {
	std::cerr << "recv()" << std::endl;
      }
      return (status);
    }

    ssize_t put ( const void * data, size_t size )
    {
      const ssize_t status = ::send(myHandle, data, size, 0);
      if ( status < 0 ) {
	std::cerr << "send()" << std::endl;
      }
      return (status);
    }

    void putall ( const void * data, size_t size )
    {
      putall(static_cast<const char*>(data), size);
    }

    void putall ( const std::string& message )
    {
      putall(message.data(), message.size());
    }

    void putall ( const char * data, size_t size )
    {
      ssize_t used = 0;
      ssize_t pass = 0;
      do {
	pass = put(data+used, size-used);
      }
      while ((pass > 0) && ((used+=pass) < size));
    }
  };

  class File
  {
    int myHandle;
    File ( const File& );
    File& operator= ( const File& );
  public:
    File ( const char * path )
      : myHandle(::open(path, O_RDONLY))
    {
      if ( myHandle < 0 ) {
	std::cerr << "open()" << std::endl;
      }
    }

    explicit File ( int handle )
      : myHandle(handle)
    {
    }

    ~File ()
    {
      ::close(myHandle);
    }

    int handle () const
    {
      return (myHandle);
    }

    ssize_t get ( void * data, size_t size )
    {
      const ssize_t status = ::read(myHandle, data, size);
      if ( status < 0 ) {
	std::cerr << "read()" << std::endl;
      }
      return (status);
    }

    ssize_t put ( const void * data, size_t size )
    {
      const ssize_t status = ::write(myHandle, data, size);
      if ( status < 0 ) {
	std::cerr << "write()" << std::endl;
      }
      return (status);
    }

    void putall ( const void * data, size_t size )
    {
      putall(static_cast<const char*>(data), size);
    }

    void putall ( const std::string& message )
    {
      putall(message.data(), message.size());
    }

    void putall ( const char * data, size_t size )
    {
      ssize_t used = 0;
      ssize_t pass = 0;
      do {
	pass = put(data+used, size-used);
      }
      while ((pass > 0) && ((used+=pass) < size));
    }
  };

}

namespace {

  void tohost ( ::ws_iwire * stream, const void * data, uint64 size )
  {
    static_cast<net::File*>(stream->baton)->putall(data, size);
  }

  void topeer ( ::ws_owire * stream, const void * data, uint64 size )
  {
    static_cast<net::Stream*>(stream->baton)->putall(data, size);
  }

  std::size_t handshake ( net::Stream& peer, char * data, std::size_t size )
  {
    // Send HTTP upgrade request.
    std::ostringstream request;
    request
      << "GET / HTTP/1.1"            << "\r\n"
      << "Host: ..."                 << "\r\n"
      << "Upgrade: WebSocket"        << "\r\n"
      << "Conection: Upgrade"        << "\r\n"
      << "Sec-WebSocket-Key: ..."    << "\r\n"
      << "Sec-WebSocket-Version: 13" << "\r\n"
      << "\r\n";
    peer.putall(request.str());
    
    // Wait for upgrade approval.
    http::Response response;
    ::size_t used = 0;
    do {
      used = peer.get(data, sizeof(data));
      if ( used == 0 ) {
        std::cout << "Peer has finished." << std::endl;
        break;
      }
      used -= response.feed(data, used);
    }
    while ( !response.complete() );
    
    // Make sure we succeeded.
    if (!response.complete()) {
      std::cout << "Did not finish HTTP response." << std::endl;
    }
    
    // Confirm handshake.
    if ((response.header("Connection") != "upgrade"  )||
        (response.header("Upgrade"   ) != "websocket"))
    {
      std::cout << "Upgrade request denied." << std::endl;
    }
    const std::string version = response.header("Sec-WebSocket-Version");
    const std::string key = response.header("Sec-WebSocket-Accept");
    
    // Keep any leftovers for the wire protocol.
    return (size);
   }

}

int main ( int argc, char ** argv )
{
  std::cout << "Connecting..." << std::endl;
  const net::Endpoint endpoint(127,0,0,1,8000);
  net::Stream peer(endpoint);
  net::File host(STDIN_FILENO);

  std::cout << "Setting up wires..." << std::endl;
  ::ws_owire owire;
  ::ws_owire_init(&owire);
  owire.baton          = &peer;
  owire.accept_content = &::topeer;
  ::ws_iwire iwire;
  ::ws_iwire_init(&iwire);
  iwire.baton          = &host;
  iwire.accept_content = &::tohost;

  char data[1024];
  
  // Perform WebSocket handshake.
  ::ws_iwire_feed(&iwire, data,
      handshake(peer, data, sizeof(data)));
  
  std::cout << "Handshake completed..." << std::endl;
  
  net::Waitables streams;
  streams.add(host.handle());
  streams.add(peer.handle());
  if (net::iready(streams))
  {
    if (streams.contains(host.handle()))
    {
      const ssize_t size = host.get(data, sizeof(data));
      if ( size == 0 ) {
	std::cout << "Host has finished." << std::endl;
      }
      ::ws_owire_feed(&owire, data, size);
    }
    if (streams.contains(peer.handle()))
    {
      const ssize_t size = peer.get(data, sizeof(data));
      if ( size == 0 ) {
	std::cout << "Peer has finished." << std::endl;
      }
      ::ws_iwire_feed(&iwire, data, size);
   }
  }
}
