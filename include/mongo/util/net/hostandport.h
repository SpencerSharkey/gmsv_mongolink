// hostandport.h

/*    Copyright 2009 10gen Inc.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#pragma once

#include "mongo/bson/util/builder.h"
#include "mongo/client/options.h"
#include "mongo/util/log.h"
#include "mongo/util/mongoutils/str.h"
#include "mongo/util/net/sock.h"

namespace mongo {

    using namespace mongoutils;

    /** helper for manipulating host:port connection endpoints.
      */
    struct HostAndPort {
        HostAndPort() : _port(-1) { }

        /** From a string hostname[:portnumber]
            Throws user assertion if bad config string or bad port #.
        */
        HostAndPort(const std::string& s);

        /** @param p port number. -1 is ok to use default. */
        HostAndPort(const std::string& h, int p /*= -1*/) : _host(h), _port(p) { 
            verify(!mongoutils::str::startsWith(h, '#'));
        }

        HostAndPort(const SockAddr& sock ) : _host( sock.getAddr() ) , _port( sock.getPort() ) { }

        bool operator<(const HostAndPort& r) const {
            const int cmp = host().compare(r.host());
            if (cmp)
                return cmp < 0;
            return port() < r.port();
        }

        bool operator==(const HostAndPort& r) const { 
            return host() == r.host() && port() == r.port();
        }

        bool operator!=(const HostAndPort& r) const { return !(*this == r); }

        /* returns true if the host/port combo identifies this process instance. */
        bool isSelf() const; // defined in isself.cpp

        bool isLocalHost() const;

        /**
         * @param includePort host:port if true, host otherwise
         */
        std::string toString( bool includePort=true ) const;

        operator std::string() const { return toString(); }

        void append( StringBuilder& ss ) const;

        bool empty() const {
            return _host.empty() && _port < 0;
        }
        const std::string& host() const {
            return _host;
        }
        int port() const {
            if (hasPort())
                return _port;
            return client::Options::kDbServer;
        }
        bool hasPort() const {
            return _port >= 0;
        }
        void setPort( int port ) {
            _port = port;
        }

    private:
        void init(const char *);
        std::string _host;
        int _port; // -1 indicates unspecified
    };

    inline std::string HostAndPort::toString( bool includePort ) const {
        if ( ! includePort )
            return host();

        StringBuilder ss;
        append( ss );
        return ss.str();
    }

    inline void HostAndPort::append( StringBuilder& ss ) const {
        ss << host();

        int p = port();

        if ( p != -1 ) {
            ss << ':';
#if defined(_DEBUG)
            if( p >= 44000 && p < 44100 ) {
                log() << "warning: special debug port 44xxx used" << std::endl;
                ss << p+1;
            }
            else
                ss << p;
#else
            ss << p;
#endif
        }

    }


    inline bool HostAndPort::isLocalHost() const {
        std::string _host = host();
        return (  _host == "localhost"
               || mongoutils::str::startsWith(_host.c_str(), "127.")
               || _host == "::1"
               || _host == "anonymous unix socket"
               || _host.c_str()[0] == '/' // unix socket
               );
    }

    inline void HostAndPort::init(const char *p) {
        massert(13110, "HostAndPort: host is empty", *p);
        const char *colon = strrchr(p, ':');
        if( colon ) {
            int port = atoi(colon+1);
            massert(13095, "HostAndPort: bad port #", port > 0);
            _host = std::string(p,colon-p);
            _port = port;
        }
        else {
            // no port specified.
            _host = p;
            _port = -1;
        }
    }

    inline HostAndPort::HostAndPort(const std::string& s) {
        init(s.c_str());
    }

}
