//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03

// <experimental/filesystem>

// bool is_empty(path const& p);
// bool is_empty(path const& p, std::error_code& ec) noexcept;

#include <experimental/filesystem>
#include <type_traits>
#include <cassert>

#include "test_macros.h"
#include "rapid-cxx-test.hpp"
#include "filesystem_test_helper.hpp"

using namespace std::experimental::filesystem;

TEST_SUITE(is_empty_test_suite)

TEST_CASE(signature_test)
{
    const path p; ((void)p);
    std::error_code ec; ((void)ec);
    ASSERT_NOEXCEPT(is_empty(p, ec));
    ASSERT_NOT_NOEXCEPT(is_empty(p));
}

TEST_CASE(test_exist_not_found)
{
    const path p = StaticEnv::DNE;
    std::error_code ec;
    TEST_CHECK(is_empty(p, ec) == false);
    TEST_CHECK(ec);
    TEST_CHECK_THROW(filesystem_error, is_empty(p));
}

TEST_CASE(test_is_empty_directory)
{
    TEST_CHECK(!is_empty(StaticEnv::Dir));
    TEST_CHECK(!is_empty(StaticEnv::SymlinkToDir));
}

TEST_CASE(test_is_empty_directory_dynamic)
{
    scoped_test_env env;
    TEST_CHECK(is_empty(env.test_root));
    env.create_file("foo", 42);
    TEST_CHECK(!is_empty(env.test_root));
}

TEST_CASE(test_is_empty_file)
{
    TEST_CHECK(is_empty(StaticEnv::EmptyFile));
    TEST_CHECK(!is_empty(StaticEnv::NonEmptyFile));
}

TEST_CASE(test_is_empty_fails)
{
    scoped_test_env env;
    const path dir = env.create_dir("dir");
    const path dir2 = env.create_dir("dir/dir2");
    permissions(dir, perms::none);

    std::error_code ec;
    TEST_CHECK(is_empty(dir2, ec) == false);
    TEST_CHECK(ec);

    TEST_CHECK_THROW(filesystem_error, is_empty(dir2));
}

TEST_SUITE_END()
