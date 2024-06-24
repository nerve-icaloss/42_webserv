/* #############################################################################

               """          UploadStrategy.hpp
        -\-    _|__
         |\___/  . \        Created on 24 Jun. 2024 at 11:40
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

############################################################################# */

#ifndef INCLUDE_SRC_UPLOADSTRATEGY_HPP_
#define INCLUDE_SRC_UPLOADSTRATEGY_HPP_

#include "ResponseBuildState.hpp"
#include "ResponseBuildingStrategy.hpp"
#include <string>

/**
  Strategy to upload file to server.

  run build_response until file is written.
  */
class UploadStrategy : public ResponseBuildingStrategy {
    public:
        UploadStrategy(ResponseBuildState &state, std::string location);
        ~UploadStrategy();

    private:
        std::string _location;
};

#endif  // INCLUDE_SRC_UPLOADSTRATEGY_HPP_
