/* #############################################################################

               """          Message.hpp
        -\-    _|__
         |\___/  . \        Created on 19 Apr. 2024 at 17:51
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

############################################################################# */

#ifndef INCLUDE_SRC_MESSAGE_HPP_
#define INCLUDE_SRC_MESSAGE_HPP_

#include <map>
#include <sstream>
#include <string>
#include "HttpMethods.hpp"
#include "HttpStatusCodes.hpp"
#ifdef TESTING
#include "gtest/gtest.h"
#endif

class Message {
    public:
        Message();
        Message(const Message&);
        ~Message();

        Message                             &operator=(const Message&);
        void                                parse(const std::string &in);
    private:
        HttpMethod                          parse_method(const std::string &method, const size_t &end);
        void                                parse_target(const std::string &in, const size_t &pos);
        void                                init_header(const std::string &in);
        void                                parse_header_line(const std::string &in,
            size_t                                                              begin,
            size_t                                                              end);
        std::string                         _method;
        std::string                         _target;
        std::map<std::string, std::string>  _header;
        std::map<std::string, std::string>  _response_header;
        std::map<std::string, std::string>  _parameters;
        std::string                         _body;
        HttpCode                            _status;
        bool                                _absolute_form;

#ifdef TESTING
        FRIEND_TEST(MessageTestSuite,   ParseMethodTestExpectedOK);
        FRIEND_TEST(MessageTestSuite,   ParseMethodTestExpectedFail);
        FRIEND_TEST(MessageTest, ParseTargetTest);
        FRIEND_TEST(MessageTestSuite,   ParseHeaderLineTestHost);
        FRIEND_TEST(MessageTestParseHeader, ParseHeaderLineTest);
#endif
};

#endif  // INCLUDE_SRC_MESSAGE_HPP_
