/* #####################################################################################################################

               """          MockServer.hpp
        -\-    _|__
         |\___/  . \        Created on 28 Jul. 2024 at 16:51
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

##################################################################################################################### */

#ifndef INCLUDE_TEST_MOCKSERVER_HPP_
#define INCLUDE_TEST_MOCKSERVER_HPP_

#include "FakeRoute.hpp"
#include "Logger.hpp"
#include "Logger.hpp"
#include <cstddef>
#include <exception>
#include <map>
#include <ostream>
#include <string>

class ResponseBuildStateFixture;

class FakeServer {
    public:
        FakeServer() {}

        FakeRoute &getRoute(const std::string &path) {
            if (path[0] != '/') {
                warn.log()  << "target '" << path
                            << "' is not starting with '/', wrong target. Choosing default route '/'." << std::endl;
                if (hasRoute("/"))
                    return _routes["/"];
                error.log() << "No default route with wrong target (not starting with /): '" << path << "'"
                            << std::endl;
                throw std::exception();
            }

            size_t      i           = path.find('/');
            std::string last_found  = "/";
            std::string testing     = path.substr(0, i + 1);

            while (hasRoute(testing)) {
                last_found = testing;
                i = path.find('/', i + 1);
                if (i == path.npos)
                    break;
                testing = path.substr(0, i + 1);
            }
            return _routes[last_found]; // will return "/" route if default
        }

        inline bool hasRoute(const std::string &path) const {
            return _routes.find(path) == _routes.end() ? false : true;
        }

    private:
        std::map<std::string, FakeRoute> _routes;
    friend ResponseBuildStateFixture;
};

#endif  // INCLUDE_TEST_MOCKSERVER_HPP_