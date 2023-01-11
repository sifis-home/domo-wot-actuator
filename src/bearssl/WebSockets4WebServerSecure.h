/**
 * @file WebSocketsServer.cpp
 * @date 28.10.2020
 * @author Markus Sattler & esp8266/arduino community
 *
 * Copyright (c) 2020 Markus Sattler. All rights reserved.
 * This file is part of the WebSockets for Arduino.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef __WEBSOCKETS4WEBSERVERSECURE_H
#define __WEBSOCKETS4WEBSERVERSECURE_H

#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>

#if WEBSOCKETS_NETWORK_TYPE == NETWORK_ESP8266 && WEBSERVER_HAS_HOOK

class WebSockets4WebServerSecure : public WebSocketsServerCore {
public:
    WebSockets4WebServerSecure(const String & origin = "", const String & protocol = "arduino")
            : WebSocketsServerCore(origin, protocol) {
        begin();
    }

    BearSSL::ESP8266WebServerSecure::HookFunction hookForWebserver(const String & wsRootDir, WebSocketServerEvent event) {
        onEvent(event);

        return [&, wsRootDir](const String & method, const String & url, WiFiClient * tcpClient1, BearSSL::ESP8266WebServerSecure::ContentTypeFunction contentType) {
            (void)contentType;
            WiFiClientSecure* tcpClient = (WiFiClientSecure*)tcpClient1;

            if(!(method == "GET" && url.indexOf(wsRootDir) == 0)) {
                return BearSSL::ESP8266WebServerSecure::CLIENT_REQUEST_CAN_CONTINUE;
            }

            // allocate a WiFiClient copy (like in WebSocketsServer::handleNewClients())
            WEBSOCKETS_NETWORK_SSL_CLASS * newTcpClient = new WEBSOCKETS_NETWORK_SSL_CLASS(*tcpClient);

            // Then initialize a new WSclient_t (like in WebSocketsServer::handleNewClient())
            WSclient_t * client = handleNewClient(newTcpClient);

            if(client) {
                // give "GET <url>"
                String headerLine;
                headerLine.reserve(url.length() + 5);
                headerLine = "GET ";
                headerLine += url;
                handleHeader(client, &headerLine);
            }

            // tell webserver to not close but forget about this client
            return BearSSL::ESP8266WebServerSecure::CLIENT_IS_GIVEN;
        };
    }
};
#else    // WEBSOCKETS_NETWORK_TYPE == NETWORK_ESP8266 && WEBSERVER_HAS_HOOK

#ifndef WEBSERVER_HAS_HOOK
#error Your current Framework / Arduino core version does not support Webserver Hook Functions
#else
#error Your Hardware Platform does not support Webserver Hook Functions
#endif

#endif    // WEBSOCKETS_NETWORK_TYPE == NETWORK_ESP8266 && WEBSERVER_HAS_HOOK

#endif
