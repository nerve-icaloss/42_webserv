/* #############################################################################

               """          Location.hpp
        -\-    _|__
         |\___/  . \        Created on 10 Jul. 2024 at 10:19
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

############################################################################# */

#ifndef INCLUDE_SRC_LOCATION_HPP_
#define INCLUDE_SRC_LOCATION_HPP_

/**
  This class is meant to interpret a target and convert it to a location, giving what is allowed to do or not.
  */

#include "ClientRequest.hpp"
#include "HttpStatusCodes.hpp"
#include "Route.hpp"
#include "Server.hpp"
#include <string>

class Location {
    public:
        Location(ClientRequest &request, Server &server);
        ~Location();

        bool        is_get() const;
        bool        is_post() const;
        bool        is_delete() const;
        bool        is_put() const;
        bool        has_autoindex() const;
        bool        is_file() const;
        bool        is_cgi() const;
        bool        is_redirect() const;
        bool        is_error() const;
        HttpCode    get_status_code() const;
        std::string get_path() const;

    private:
        bool        _is_get;
        bool        _is_post;
        bool        _is_delete;
        bool        _is_put;
        bool        _autoindex;
        bool        _is_file;
        bool        _is_cgi;
        bool        _is_redirect;
        bool        _is_error;
        HttpCode    _status_code;
        std::string _path;
};

#endif  // INCLUDE_SRC_LOCATION_HPP_