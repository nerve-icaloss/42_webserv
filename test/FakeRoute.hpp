/* #####################################################################################################################

               """          FakeRoute.hpp
        -\-    _|__
         |\___/  . \        Created on 28 Jul. 2024 at 17:12
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

##################################################################################################################### */

#ifndef INCLUDE_TEST_FAKEROUTE_HPP_
#define INCLUDE_TEST_FAKEROUTE_HPP_

#include "HttpMethods.hpp"
#include "HttpStatusCodes.hpp"
#include <string>
#include <vector>

class ResponseBuildStateFixture;

class FakeRoute {
    public:
        FakeRoute() {};

        FakeRoute(HttpCode _getRedirCode_,
            bool                            _hasAutoindex_,
            bool                            _hasCgiExtension_,
            bool                            _hasCgiPath_,
            bool                            _hasRedir_,
            bool                            _hasUpload_,
            const std::string               &_getCgiExtension_,
            const std::string               &_getCgiPath_,
            const std::string               &_getLocation_,
            const std::string               &_getRedirPage_,
            const std::string               &_getRootDir_,
            const std::string               &_getUploadPath_,
            const std::vector<HttpMethod>   &_getMethods_,
            const std::vector<std::string>  &_getIndexPage_):
            _getRedirCode   (_getRedirCode_),
            _hasAutoindex   (_hasAutoindex_),
            _hasCgiExtension(_hasCgiExtension_),
            _hasCgiPath     (_hasCgiPath_),
            _hasRedir       (_hasRedir_),
            _hasUpload      (_hasUpload_),
            _getCgiExtension(_getCgiExtension_),
            _getCgiPath     (_getCgiPath_),
            _getLocation    (_getLocation_),
            _getRedirPage   (_getRedirPage_),
            _getRootDir     (_getRootDir_),
            _getUploadPath  (_getUploadPath_),
            _getMethods     (_getMethods_),
            _getIndexPage   (_getIndexPage_) {};

        HttpCode getRedirCode() const {
            return _getRedirCode;
        }

        bool hasAutoindex() const {
            return _hasAutoindex;
        }

        bool hasCgiExtension() const {
            return _hasCgiExtension;
        }

        bool hasCgiPath() const {
            return _hasCgiPath;
        }

        bool hasRedir() const {
            return _hasRedir;
        }

        bool hasUpload() const {
            return _hasUpload;
        }

        std::string getCgiExtension() const {
            return _getCgiExtension;
        }

        std::string getCgiPath() const {
            return _getCgiPath;
        }

        std::string getLocation() const {
            return _getLocation;
        }

        std::string getRedirPage() const {
            return _getRedirPage;
        }

        std::string getRootDir() const {
            return _getRootDir;
        }

        std::string getUploadPath() const {
            return _getUploadPath;
        }

        const std::vector<HttpMethod> &getMethods() const {
            return _getMethods;
        }

        std::vector<std::string> getIndexPage() const {
            return _getIndexPage;
        }

    private:
        HttpCode    _getRedirCode;
        bool        _hasAutoindex;
        bool        _hasCgiExtension;
        bool        _hasCgiPath;
        bool        _hasRedir;
        bool        _hasUpload;
        std::string _getCgiExtension;
        std::string _getCgiPath;
        std::string _getLocation;
        std::string _getRedirPage;
        std::string _getRootDir;
        std::string _getUploadPath;
        std::vector<HttpMethod>     _getMethods;
        std::vector<std::string>    _getIndexPage;

        friend ResponseBuildStateFixture;
};

#endif  // INCLUDE_TEST_FAKEROUTE_HPP_