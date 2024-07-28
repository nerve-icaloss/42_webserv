/* #############################################################################

               """          ResponseBuildState.cpp
        -\-    _|__
         |\___/  . \        Created on 06 Jun. 2024 at 13:12
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

############################################################################# */

#include "BodyWriter.hpp"
#include "CGIStrategy.hpp"
#include "ClientRequest.hpp"
#include "DeleteStrategy.hpp"
#include "ErrorStrategy.hpp"
#include "GetFileStrategy.hpp"
#include "GetIndexStrategy.hpp"
#include "HttpError.hpp"
#include "HttpMethods.hpp"
#include "HttpStatusCodes.hpp"
#include "Location.hpp"
#include "Logger.hpp"
#include "MimeTypes.hpp"
#include "ProcessState.hpp"
#include "RedirectStrategy.hpp"
#include "ResponseBuildState.hpp"
#include "ResponseBuildingStrategy.hpp"
#include "Server.hpp"
#include "UploadStrategy.hpp"
#include <exception>
#include <map>
#include <ostream>
#include <pthread.h>
#include <string>
#include <sys/wait.h>

/**
 This class is in charge of choosing the right strategy depending on the request.
 */
template <class ServerClass, class RouteClass>
ResponseBuildState<ServerClass, RouteClass>::ResponseBuildState(int fd, ClientRequest *request, ServerClass &server):
    ProcessState(fd),
    _request    (request),
    _server     (server),
    _strategy   (0),
    _recovery   (false),
    _code       (OK) {
    if (!_request) {
        error.log() << "Trying to build response without request." << std::endl;
        throw HttpError(InternalServerError);
    }
}           // TODO:check where to free this once it is allocated

template <class ServerClass, class RouteClass>
ResponseBuildState<ServerClass, RouteClass>::~ResponseBuildState() {}

template <class ServerClass, class RouteClass>
t_state ResponseBuildState<ServerClass, RouteClass>::process() {
    if (!_recovery) {
        if (!_strategy)
            init_strategy();
    } else {
        if (_strategy)
            delete _strategy;
        init_strategy(_code);   // recovery
        _recovery = false;      // because it is init so just normal operation now
    }
    return _strategy->build_response() ? (_state = ready) : (_state = waiting);
}

template <class ServerClass, class RouteClass>
ClientRequest *ResponseBuildState<ServerClass, RouteClass>::get_request() {
    return _request;
}

template <class ServerClass, class RouteClass>
ResponseBuildingStrategy *ResponseBuildState<ServerClass, RouteClass>::get_response_strategy() {
    return _strategy;
}

/**
  This function build the right strategy for a given ClientRequest.
  */
template <class ServerClass, class RouteClass>
void ResponseBuildState<ServerClass, RouteClass>::init_strategy() {
    if (isError(_request->get_status())) {
        const std::map<HttpCode, std::string> error_pages   = _server.getErrorPages();

        std::map<HttpCode, std::string>::const_iterator it  = error_pages.find(_request->get_status());

        if (it == error_pages.end())
            _strategy = new ErrorStrategy(_request->get_status());  // page not found
        else                                                        // page found
            _strategy = new GetFileStrategy(mime_types, it->second, _request->get_status());
        return;
    }

    Location<ServerClass, RouteClass> location(*_request, _server);

    if (location.is_redirect())
        _strategy = new RedirectStrategy(location.get_path(), location.get_status_code());
    else if (location.is_cgi())
        _strategy = new CGIStrategy(location.get_path(), _request);
    else if (_request->get_method() == GET) {
        if (!location.is_get())
            throw HttpError(MethodNotAllowed);
        if (location.is_file())
            _strategy = new GetFileStrategy(mime_types, location.get_path());
        else if (location.has_autoindex())
            _strategy = new GetIndexStrategy(location.get_path());
        else
            throw HttpError(Forbidden);
    } else if (_request->get_method() == POST) {
        if (!location.is_post())
            throw HttpError(MethodNotAllowed);
        _strategy = new UploadStrategy(*_request, location.get_path());
    } else if (_request->get_method() == DELETE) {
        if (!location.is_delete())
            throw HttpError(MethodNotAllowed);
        _strategy = new DeleteStrategy(location.get_path());
    }
    if (!_strategy) {
        error.log() << "No strategy selected for a given request. Throwing " << InternalServerError << std::endl;
        throw HttpError(InternalServerError);
    }
} // 27 not that bad

/**
  Build error strategy for a given http code.
  */
template <class ServerClass, class RouteClass>
void ResponseBuildState<ServerClass, RouteClass>::init_strategy(HttpCode code) {
    if (!isError(code))
        warn.log() << "Generating error for non error code " << code << std::endl;
    try {
        _strategy = new ErrorStrategy(code);
    } catch (std::exception &e) {
        throw e; // do not try to send response after this, only close connection.
        // WARN: check not to loop in recovery there.
    }
}
