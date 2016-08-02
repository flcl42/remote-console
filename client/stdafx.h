// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

//#include "../cpp-netlib-0.11.2-final/libs/"

#include <iostream>



#include <cpprest\http_client.h>
#include <cpprest\filestream.h>
//#include <cpprest/http_listener.h>              // HTTP server
#include <cpprest\json.h>                       // JSON library
#include <cpprest\uri.h>                        // URI library
#include <cpprest\asyncrt_utils.h>
#include <cpprest\base_uri.h>
#include <cpprest\asyncrt_utils.h>

//#include <cpprest/ws_client>                    // WebSocket client
//#include <cpprest/containerstream.h>            // Async streams backed by STL containers
//#include <cpprest/interopstream.h>              // Bridges for integrating Async streams with STL and WinRT streams
//#include <cpprest/rawptrstream.h>               // Async streams backed by raw pointer to memory
//#include <cpprest/producerconsumerstream.h>     // Async streams for producer consumer scenarios