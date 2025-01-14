#include "Server.hpp"
#include "HttpMethods.hpp"
#include "HttpStatusCodes.hpp"
#include "Logger.hpp"
#include "Route.hpp"
#include "ServerConfFields.hpp"
#include "ServerConfTokenize.hpp"
#include "ServerConfValidate.hpp"
#include "StringTokenizer.hpp"
#include <algorithm>
#include <cstddef>
#include <map>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

Server::Server(const std::string &serverContent) :
    _serverName(1, "localhost"),
    _port(8080),
    _maxBodySize(1000000),
    _autoindex(true),
    _methods(1, GET),
    _rootDir("./"),
    _indexPage(1, "index.html"),
    _routes(),
    _errorPages(),
    _serverNameSet(false),
    _portSet(false),
    _maxBodySizeSet(false),
    _autoindexSet(false),
    _indexPageSet(false),
    _methodsSet(false),
    _rootDirSet(false) {
    StringTokenizer        tokenizedServer(serverContent, '|');
    std::vector<Field>     tokenizedLocations;
    std::vector<ValueList> tokenizedErrorPages;

    std::string infoStr = serverContent.substr(0, 30);

    std::replace(infoStr.begin(), infoStr.end(), '|', ' ');
    info.log() << "server: " << infoStr << " ... : parsing started..." << std::endl;
    while (tokenizedServer.hasMoreTokens()) {
        try {
            Field fieldContent;

            if (tokenizedServer.peakToken() == ConfFieldString(location))
                fieldContent = tokenizeLocation(tokenizedServer);
            else
                fieldContent = tokenizeField(tokenizedServer);

            int       fieldCode    = findFieldCode(fieldContent.first);
            ValueList valueContent = tokenizeValue(fieldContent.second);

            switch (fieldCode) {
                case 2:
                    tokenizedLocations.push_back(fieldContent);
                    break;
                case 3:
                    tokenizedErrorPages.push_back(valueContent);
                    break;
                case 4:
                    if (_serverNameSet) {
                        warn.log() << "server_name: ";
                        for (size_t i = 0; i < valueContent.size(); ++i)
                            warn.log() << valueContent[i] << " ";
                        warn.log() << ", is a redifinition of the current value, field ignored." << std::endl;
                        break;
                    }
                    this->setServerName(valueContent);
                    break;
                case 5:
                    if (_portSet) {
                        warn.log() << "listen: " << valueContent[0]
                                   << ", is a redifinition of the current value, field ignored." << std::endl;
                        break;
                    }
                    this->setPort(valueContent);
                    break;
                case 6:
                    if (_rootDirSet) {
                        warn.log() << "root: " << valueContent[0]
                                   << ", is a redifinition of the current value, field ignored." << std::endl;
                        break;
                    }
                    this->setRootDir(valueContent);
                    break;
                case 7:
                    if (_indexPageSet) {
                        warn.log() << "index: ";
                        for (size_t i = 0; i < valueContent.size(); ++i)
                            warn.log() << valueContent[i] << " ";
                        warn.log() << ", is a redifinition of the current value, field ignored." << std::endl;
                        break;
                    }
                    this->setIndexPage(valueContent);
                    break;
                case 8:
                    if (_autoindexSet) {
                        warn.log() << "autoindex: " << valueContent[0]
                                   << ",  is a redifinition of the current value, field ignored." << std::endl;
                        break;
                    }
                    this->setAutoindex(valueContent);
                    break;
                case 9:
                    if (_methodsSet) {
                        warn.log() << "methods: ";
                        for (size_t i = 0; i < valueContent.size(); ++i)
                            warn.log() << valueContent[i] << " ";
                        warn.log() << ", is a redifinition of the current value, field ignored." << std::endl;
                        break;
                    }
                    this->setMethods(valueContent);
                    break;
                case 10:
                    if (_maxBodySizeSet) {
                        warn.log() << "max_body_size: " << valueContent[0]
                                   << ",  is a redifinition of the current value, field ignored." << std::endl;
                        break;
                    }
                    this->setMaxBodySize(valueContent);
                    break;
                default:
                    warn.log() << fieldContent.first << ": is not a valide Server Conf Field" << std::endl;
                    throw ServerConfWarn();
            }
        } catch (ServerConfWarn &e) {
            continue;
        }
    }
    for (std::vector<Field>::iterator it = tokenizedLocations.begin(); it < tokenizedLocations.end(); ++it) {
        try {
            this->addRoute(*it);
        } catch (ServerConfWarn &e) {
            continue;
        }
    }
    for (std::vector<ValueList>::iterator it = tokenizedErrorPages.begin(); it < tokenizedErrorPages.end(); ++it) {
        try {
            this->addErrorPage(*it);
        } catch (ServerConfWarn &e) {
            continue;
        }
    }
    if (!hasRoute("/"))
        _routes["/"] = (*this);
}

Server::~Server() {}

std::vector<std::string> Server::getServerName() const {
    return _serverName;
}

unsigned int Server::getPort() const {
    return _port;
}

bool Server::getAutoindex() const {
    return _autoindex;
}

std::vector<HttpMethod> Server::getMethods() const {
    return _methods;
}

std::string Server::getRootDir() const {
    return _rootDir;
}

std::vector<std::string> Server::getIndexPage() const {
    return _indexPage;
}

const std::map<HttpCode, std::string> &Server::getErrorPages() const {
    return _errorPages;
}

inline bool Server::hasRoute(const std::string &path) const {
    return _routes.find(path) == _routes.end() ? false : true;
}

// TODO test this
Route &Server::getRoute(const std::string &path) {
    if (path[0] != '/') {
        warn.log() << "target '" << path << "' is not starting with '/', wrong target. Choosing default route '/'."
                   << std::endl;
        if (hasRoute("/"))
            return _routes["/"];
        error.log() << "No default route with wrong target (not starting with /): '" << path << "'" << std::endl;
        throw RouteNotFoundWarn(path);
    }

    size_t      i          = path.find('/');
    std::string last_found = "/";
    std::string testing    = path.substr(0, i + 1);

    while (hasRoute(testing)) {
        last_found = testing;
        i          = path.find('/', i + 1);
        if (i == path.npos)
            break;
        testing = path.substr(0, i + 1);
    }
    return _routes[last_found]; // will return "/" route if default
}

bool Server::hasServeName(const std::string &serverName) const {
    std::vector<std::string>::const_iterator name = _serverName.end();

    name = std::find(_serverName.begin(), _serverName.end(), serverName);
    if (name == _serverName.end())
        return false;
    else
        return true;
}

void Server::addRoute(const Field &locationContent) {
    if (!isValidUrl(locationContent.first)) {
        warn.log() << "location: " << locationContent.first << ": is not a valid url path." << std::endl;
        throw ServerConfWarn();
    }
    _routes[locationContent.first] = Route(locationContent.first, locationContent.second, *this);
}

void Server::addErrorPage(const ValueList &valueContent) {
    if (valueContent.size() != 2) {
        warn.log() << "error_page: ";
        for (size_t i = 0; i < valueContent.size(); ++i)
            warn.log() << valueContent[i] << " ";
        warn.log() << ", fail to parse field, it accept only a error code and a path value." << std::endl;
        throw ServerConfWarn();
    }

    std::stringstream errorStr(valueContent[0]);
    int               errorCode;

    if (errorStr >> errorCode) {
        warn.log() << "error_page: " << valueContent[0] << ": is not a valid int." << std::endl;
        throw ServerConfWarn();
    }
    if (!isError(errorCode)) {
        warn.log() << "error_page: " << valueContent[0] << ": is not a valid Http error." << std::endl;
        throw ServerConfWarn();
    }
    if (errorCode > 599) {
        info.log() << "error_page: " << valueContent[0] << ": this error code is not standard because above 599."
                   << std::endl;
    }
    if (!isValidUrl(valueContent[1])) {
        warn.log() << "error_page: " << valueContent[0] << " " << valueContent[1] << "is not a valid path."
                   << std::endl;
        throw ServerConfWarn();
    }
    _errorPages[HttpCode(errorCode)] = valueContent[1];
}

void Server::setServerName(const ValueList &valueContent) {
    _serverNameSet = true;
    for (ValueList::const_iterator it = valueContent.begin(); it < valueContent.end(); ++it) {
        if (!isValidIPAddress(*it)) {
            if (!isValidHostname(*it))
                warn.log() << "server_name: " << *it << ": is not a valid Hostname." << std::endl;
            warn.log() << "server_name: " << *it << ": is not a a valid IP address." << std::endl;
        }
        _serverName.push_back(*it);
    }
}

void Server::setPort(const ValueList &valueContent) {
    _portSet = true;
    if (valueContent.size() != 1) {
        warn.log() << "listen: ";
        for (size_t i = 0; i < valueContent.size(); ++i)
            warn.log() << valueContent[i] << " ";
        warn.log() << ", fail to parse field, it accept only one value." << std::endl;
        throw ServerConfWarn();
    }

    std::stringstream port(valueContent[0]);
    int               portNumber;

    if (port >> portNumber) {
        warn.log() << "listen: " << valueContent[0] << ": is not a valid int." << std::endl;
        throw ServerConfWarn();
    }
    if (!isValidPort(portNumber)) {
        warn.log() << "listen: " << valueContent[0] << ": is not a valid port number." << std::endl;
        throw ServerConfWarn();
    }
    _port = portNumber;
}

void Server::setRootDir(const ValueList &valueContent) {
    _rootDirSet = true;
    if (valueContent.size() != 1) {
        warn.log() << "root: ";
        for (size_t i = 0; i < valueContent.size(); ++i)
            warn.log() << valueContent[i] << " ";
        warn.log() << ", fail to parse field, it accept only one value." << std::endl;
        throw ServerConfWarn();
    }
    if (!isValidRelativePath(valueContent[0])) {
        warn.log() << "root: " << valueContent[0] << ": is not a valid root." << std::endl;
        throw ServerConfWarn();
    }
    _rootDir = valueContent[0];
}

void Server::setIndexPage(const ValueList &valueContent) {
    _indexPageSet = true;
    for (ValueList::const_iterator it = valueContent.begin(); it < valueContent.end(); ++it) {
        if (!isValidIndexFile(*it)) {
            warn.log() << "index: " << *it << ": is not a valid index page." << std::endl;
            continue;
        }
        _indexPage.push_back(*it);
    }
}

void Server::setAutoindex(const ValueList &valueContent) {
    _autoindexSet = true;
    if (valueContent.size() != 1) {
        warn.log() << "autoindex: ";
        for (size_t i = 0; i < valueContent.size(); ++i)
            warn.log() << valueContent[i] << " ";
        warn.log() << ", fail to parse field, it accept only one value." << std::endl;
        throw ServerConfWarn();
    }
    if (!isValidAutoindex(valueContent[0])) {
        warn.log() << "autoindex: " << valueContent[0] << ": is not a valid bool." << std::endl;
        throw ServerConfWarn();
    }
    if (valueContent[0] == "true")
        _autoindex = true;
    else
        _autoindex = false;
}

void Server::setMethods(const ValueList &valueContent) {
    _methodsSet = true;
    for (ValueList::const_iterator it = valueContent.begin(); it < valueContent.end(); ++it) {
        if (!isValidMethods(*it)) {
            warn.log() << "methods: " << *it << ": is not a valid Http method." << std::endl;
            continue;
        }
        if (*it == "GET")
            _methods.push_back(GET);
        if (*it == "POST")
            _methods.push_back(POST);
        if (*it == "DELETE")
            _methods.push_back(DELETE);
    }
}

void Server::setMaxBodySize(const ValueList &valueContent) {
    _maxBodySizeSet = true;
    if (valueContent.size() != 1) {
        warn.log() << "max_body_size: ";
        for (size_t i = 0; i < valueContent.size(); ++i)
            warn.log() << valueContent[i] << " ";
        warn.log() << ", fail to parse field, it accept only one value." << std::endl;
        throw ServerConfWarn();
    }

    std::stringstream maxBodySizeStr(valueContent[0]);
    int               maxBodySizeNumber;

    if (maxBodySizeStr >> maxBodySizeNumber) {
        warn.log() << "max_body_size: " << valueContent[0] << ": is not a valid int." << std::endl;
        throw ServerConfWarn();
    }
    if (!isValidPort(maxBodySizeNumber)) {
        warn.log() << "max_body_size: " << valueContent[0] << ": is not a valid size number." << std::endl;
        throw ServerConfWarn();
    }
    _maxBodySize = maxBodySizeNumber;
}
