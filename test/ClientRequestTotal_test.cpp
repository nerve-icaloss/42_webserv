/* #####################################################################################################################

               """          ClientRequestTotal_test.cpp
        -\-    _|__
         |\___/  . \        Created on 24 Jul. 2024 at 17:07
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

##################################################################################################################### */

#include "ClientRequest.hpp"
#include "HttpError.hpp"
#include "HttpMethods.hpp"
#include "HttpStatusCodes.hpp"
#include "Logger.hpp"
#include "ProcessState.hpp"
#include "ReadState.hpp"
#include "gtest/gtest.h"
#include <cassert>
#include <cstddef>
#include <exception>
#include <map>
#include <ostream>
#include <string>
#include <tuple>
#include <unistd.h>
#include <vector>

typedef enum e_total_index {
    tname = 0,
    tdata,
    ttarget,
    tmethod,
    thavebody,
    tbody,
    theaders,
    tstatus,
    tport,
    tqs,
    tbadbody,
} t_ti;

typedef std::tuple<std::string,             ///< Name
        std::string,                        ///< Request data (including body and all stuff)
        std::string,                        ///< Target
        HttpMethod,                         ///< Method
        bool,                               ///< HaveBody
        std::string,                        ///< Body after parsing (will be used to test Body objects)
        std::map<std::string, std::string>, ///< Headers
        HttpCode,                           ///< Status
        int,                                ///< port ???
        std::string,                        ///< QueryString (things after ?)
        bool                                ///< Should Body Bad Request ?
        > TotalRequest;

std::vector<TotalRequest> TotalRequestData = {
    {
        "Basic_GET", "GET /helloworld.html?hihi=ahah HTTP/1.1\r\nHost: 127.0.0.1\r\nName: fireTesting/1.0\r\n\r\n",
        "/helloworld.html", GET, false, "",{
            {
                "Host", "127.0.0.1"
            },{
                "Name", "fireTesting/1.0"
            }
        }, OK, 80, "hihi=ahah", false
    },      {
        "Port_parsing",
        "GET /helloworld.html?hihi=ahah HTTP/1.1\r\nHost: 127.0.0.1:42\r\nName: fireTesting/1.0\r\n\r\n",
        "/helloworld.html", GET, false, "",{
            {
                "Host", "127.0.0.1"
            },{
                "Name", "fireTesting/1.0"
            }
        }, OK, 42, "hihi=ahah", false
    },      {
        "Wrong_port_parsing",
        "GET /helloworld.html?hihi=ahah HTTP/1.1\r\nHost: 127.0.0.1:ab\r\nName: fireTesting/1.0\r\n\r\n",
        "/helloworld.html", GET, false, "",{
            {
                "Host", "127.0.0.1"
            },{
                "Name", "fireTesting/1.0"
            }
        }, BadRequest, 0, "hihi=ahah", false
    },      {
        "No_Host", "GET /helloworld.html?hihi=ahah HTTP/1.1\r\nName: fireTesting/1.0\r\n\r\n", "/helloworld.html", GET,
        false, "",{
            {
                "Name", "fireTesting/1.0"
            }
        }, BadRequest, 80, "hihi=ahah", false
    },      {
        "No_Headers", "GET /helloworld.html?hihi=ahah HTTP/1.1\r\n\r\n", "", none, false, "",{                                                                                  }, BadRequest,
        80,
        "", false
    },      {
        "No_Headers2", "GET /helloworld.html?hihi=ahah HTTP/1.1\r\n\r\n\r\n", "", none, false, "",{                                                                                  }, BadRequest,
        80,
        "", false
    },      {
        "Wrong_Method", "PET /helloworld.html?hihi=ahah HTTP/1.1\r\nHost: 127.0.0.1\r\nName: fireTesting/1.0\r\n\r\n",
        "", none, false, "",{                                                                                  },
        NotImplemented                                              ,     80,"", false
    },      {
        "PostLength",
        "POST /process.html?hihi=ahah HTTP/1.1\r\nHost: 127.0.0.1\r\nName: fireTesting/1.0\r\n"
        "Content-Length: 97\r\n\r\nCoucou je suis heureux et c'est le premier body que nous allons pouvoir trouver"
        " dans ces tests...NOTBODY", "/process.html", POST, true,
        "Coucou je suis heureux et c'est le premier body que nous allons pouvoir trouver"" dans ces tests...",{
            {
                "Host", "127.0.0.1"
            },{
                "Name", "fireTesting/1.0"
            },{
                "Content-Length", "97"
            }
        }, OK, 80, "hihi=ahah", false
    },      {
        "PostChunk",
        "POST /process.html?hihi=ahah HTTP/1.1\r\nHost: 127.0.0.1\r\nName: fireTesting/1.0\r\n"
        "Transfer-Encoding: chunk\r\n\r\na\r\nCoucou je \r\n32\r\nsuis heureux et c'est le premier body que nous all\r\n13\r\nons pouvoir trouver"
        "\r\n12\r\n dans ces tests...\r\n0\r\n\r\nNOTBODY", "/process.html", POST, true,
        "Coucou je suis heureux et c'est le premier body que nous allons pouvoir trouver"" dans ces tests...",{
            {
                "Host", "127.0.0.1"
            },{
                "Name", "fireTesting/1.0"
            },{
                "Transfer-Encoding", "chunk"
            }
        }, OK, 80, "hihi=ahah", false
    },      {
        "PostChunkTrailing",
        "POST /process.html?hihi=ahah HTTP/1.1\r\nHost: 127.0.0.1\r\nName: fireTesting/1.0\r\n"
        "Transfer-Encoding: chunk\r\n\r\na\r\nCoucou je \r\n32\r\nsuis heureux et c'est le premier body que nous all\r\n13\r\nons pouvoir trouver"
        "\r\n12\r\n dans ces tests...\r\n0\r\nTrailingSecretData\r\nNOTBODY", "/process.html", POST, true,
        "Coucou je suis heureux et c'est le premier body que nous allons pouvoir trouver"" dans ces tests...",{
            {
                "Host", "127.0.0.1"
            },{
                "Name", "fireTesting/1.0"
            },{
                "Transfer-Encoding", "chunk"
            }
        }, OK, 80, "hihi=ahah", false
    },
};

// TODO: analyser 411 No Length

class TotalRequestFixture: public ::testing::TestWithParam<TotalRequest> {
    public:
        TotalRequestFixture():
            _test   (0),
            _request(0),
            _fd     {0, 0} {}

        void SetUp() override {
            if (pipe(_fd))
                GTEST_FATAL_FAILURE_("Pipe error");

            const std::string &raw = std::get<tdata>(GetParam());

            size_t i = 0;

            if ((i = write(_fd[1], raw.c_str(), raw.length())) < 0)
                GTEST_FATAL_FAILURE_("Write in pipe failure");
            if (i != raw.length())
                GTEST_FATAL_FAILURE_("Partial write in pipe");
            close(_fd[1]);
            _test       = new ReadState(_fd[0]);
            _fd_check   = _fd[0];

            i           = 0;
            while ((_test->process() == waiting) && (i < 100))
                i++;
            if (i >= 100)
                GTEST_FATAL_FAILURE_("Infinite loop detected.");
            if (_test->get_state() == s_error)
                GTEST_SKIP() << "Really Bad request successfully ignored";
            // ASSERT_TRUE(_test->process());
            _request = _test->get_client_request();
            ASSERT_NE(_request, (void *) 0);
        };

        void TearDown() override {
            if (_test) {
                _test->done_client_request();
                delete _test;
            }
            // if (_request)
            //     delete _request;
            if (_fd[0] != 0)
                close(_fd[0]);
        };

    protected:
        ReadState       *_test;
        ClientRequest   *_request;
        int _fd_check;

    private:
        int         _fd[2];
        std::string _buffer;
};

INSTANTIATE_TEST_SUITE_P(TotalRequestSuite,
        TotalRequestFixture,
        ::testing::ValuesIn(TotalRequestData),
        [](const testing::TestParamInfo<TotalRequest> &info)
{
    // Can use info.param here to generate the test suffix
    std::string name = std::get<tname>(info.param);
    return name;
});

TEST_P(TotalRequestFixture, TargetTest) {
    const std::string &correct = std::get<ttarget>(GetParam());

    EXPECT_EQ(_request->get_target(), correct);
}

TEST_P(TotalRequestFixture, MethodTest) {
    HttpMethod correct = std::get<tmethod>(GetParam());

    EXPECT_EQ(_request->get_method(), correct);
}

TEST_P(TotalRequestFixture, BodyTest) {
    typedef std::map<std::string, std::string> map;

    const std::string &correct = std::get<tbody>(GetParam());

    if (!std::get<thavebody>(GetParam())) {
        info.log() << "If this message is displayed, warning is normal for this test." << std::endl;
        EXPECT_EQ(_request->get_body(), (void *) 0);
        return;
    }
    ASSERT_TRUE(_request->have_body());

    const map &headers = _request->get_header();

    if (headers.find("Content-Length") != headers.end()) {
        SUCCEED();
    } else if (headers.find("Transfer-Encoding") != headers.end()) {
        SUCCEED();
    } else {
        FAIL() << "All body headers are missing.";
    }

    auto    body    = _request->get_body();
    size_t  i       = 0;

    if (std::get<tbadbody>(GetParam())) {
        EXPECT_THROW({
            while (!body->is_done() && i++ < 100)
                body->get();
        }, HttpError);
        if (i >= 100)
            FAIL() << "Infinite loop in body getter, Body tests should fail too.";
        return;
    }
    while (!body->is_done() && i++ < 100)
        body->get();
    if (i >= 100)
        FAIL() << "Infinite loop in body getter, Body tests should fail too.";
    info.log() << "i: " << i << std::endl;

    auto body_content = body->get();

    EXPECT_EQ(body_content, correct);
}

TEST_P(TotalRequestFixture, HeadersTest) {
    typedef std::map<std::string, std::string> map;

    const map   &correct        = std::get<theaders>(GetParam());
    const map   &test_headers   = _request->get_header();

    if (isRedirection(std::get<tstatus>(GetParam()))) {
        ASSERT_NE(correct.find("Location"),
                correct.end()) << "pas de location pour vérifier la redirection dans le test.";

        map::const_iterator loc = test_headers.find("Location");

        ASSERT_NE(loc, test_headers.end());
        EXPECT_EQ(correct.find("Location")->second, loc->second);
        return;
    }

    map::const_iterator test_item;
    map::const_iterator correct_item;

    EXPECT_EQ(correct.size(), test_headers.size());
    for (correct_item = correct.begin(); correct_item != correct.end(); correct_item++) {
        test_item = test_headers.find(correct_item->first);
        EXPECT_NE(test_item, test_headers.end());
        if (test_item != test_headers.end())
            EXPECT_EQ(test_item->second, correct_item->second);
    }
    for (test_item = correct.begin(); test_item != correct.end(); test_item++) {
        correct_item = correct.find(test_item->first);
        EXPECT_NE(correct_item, correct.end());
        if (correct_item != test_headers.end())
            EXPECT_EQ(correct_item->second, test_item->second);
    } // double verification is to display clearly which one is missing.
}

TEST_P(TotalRequestFixture, HaveBodyTest) {
    const bool correct = std::get<thavebody>(GetParam());

    EXPECT_EQ(correct, _request->have_body());
}

TEST_P(TotalRequestFixture, StatusTest) {
    const HttpCode correct = std::get<tstatus>(GetParam());

    EXPECT_EQ(correct, _request->get_status());
}

TEST_P(TotalRequestFixture, FdRequestTest) {
    const int correct = _fd_check;

    EXPECT_EQ(correct, _request->get_fd());
}

TEST_P(TotalRequestFixture, FdStateTest) {
    const int correct = _fd_check;

    EXPECT_EQ(correct, _test->_fd);
}

TEST_P(TotalRequestFixture, PortTest) {
    const int correct = std::get<tport>(GetParam());

    EXPECT_EQ(correct, _request->get_port());
}

TEST_P(TotalRequestFixture, QueryStringTest) {
    const std::string &correct = std::get<tqs>(GetParam());

    EXPECT_EQ(correct, _request->get_query_string());
}