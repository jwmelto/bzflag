/* bzflag
 * Copyright (c) 1993-2019 Tim Riker
 *
 * This package is free software;  you can redistribute it and/or
 * modify it under the terms of the license found in the file
 * named COPYING that should have accompanied this file.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/*
 * includes platform specific network files and adds missing stuff
 *
 * unfortunately this can include far more than necessary
 */

#ifndef BZF_NETWORK_H
#define BZF_NETWORK_H

#include "common.h"

#ifdef _WIN32
# include <winsock2.h>
# include <ws2tcpip.h>

using AddrLen = int;

# ifndef EINPROGRESS
#  define EINPROGRESS WSAEWOULDBLOCK
# endif

# ifndef EWOULDBLOCK
#  define EWOULDBLOCK WSAEWOULDBLOCK
# endif

# ifndef ECONNRESET
#  define ECONNRESET  WSAECONNRESET
# endif

# ifndef EBADMSG
#  define EBADMSG     WSAECONNRESET   /* not defined by windows */
# endif


/* setsockopt prototypes the 4th arg as const char*. */
using SSOType = char const*;

//moved to multicast.cxx
//inline int close(SOCKET s)
//{
//  return closesocket(s);
//}
# define ioctl(__fd, __req, __arg) \
ioctlsocket(__fd, __req, (u_long*)__arg)
# define gethostbyaddr(__addr, __len, __type) \
gethostbyaddr((const char*)__addr, __len, __type)

extern "C" {
  
  int           inet_aton(const char* cp, struct in_addr* pin);
  void          herror(const char* msg);
  
}

#else   //if !defined(_WIN32)

# include <sys/socket.h>
# include <netinet/in.h>
# include <netinet/tcp.h>// TCP_NO_DELAY in bzfs.cxx
# include <arpa/inet.h> // inet_ntoa in AccessControlList.cxx
# include <netdb.h> // herror, hostent in <ares.h>
# include <sys/select.h> // select in Ping.cxx

// Posix defines the socket API to use socklen_t
using AddrLen = socklen_t;

# if defined(sun)
/* setsockopt prototypes the 4th arg as const char*. */
using SSOType = char const*;
/* connect prototypes the 2nd arg without const */
//using CNCTType = sockaddr;
# else
using SSOType  = void const*;
//using CNCTType = sockaddr const;
# endif

// This is extremely questionable. herror() is defined in netdb.h for Linux
// and in winsock2.h for Windows. Of course, it's deprecated (at least for Linux)
// so remapping it like this doesn't sound good. Better to refactor the code that feeds it
#define herror(x_)  bzfherror(x_)

extern "C" {
  void          bzfherror(const char* msg);
}

#endif /* defined(_WIN32) */

// Can this happen?
#if !defined(INADDR_NONE)
#  define INADDR_NONE   ((in_addr_t)0xffffffff)
#endif

// for all platforms
extern "C" {
  void          nerror(const char* msg);
  int           getErrno();
}

#include <string>


class BzfNetwork
{
public:
    static int        setNonBlocking(int fd);
    static int        setBlocking(int fd);
    static bool   parseURL(const std::string& url,
                           std::string& protocol,
                           std::string& hostname,
                           int& port,
                           std::string& pathname);
};

#endif // BZF_NETWORK_H


// Local Variables: ***
// mode: C++ ***
// tab-width: 4 ***
// c-basic-offset: 4 ***
// indent-tabs-mode: nil ***
// End: ***
// ex: shiftwidth=4 tabstop=4
