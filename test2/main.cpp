//
// Copyright (c) 2013-2016 Pavel Medvedev. All rights reserved.
//
// This file is part of v8pp (https://github.com/pmed/v8pp) project.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include <iostream>
#include <algorithm>
#include <memory>
#include <vector>
#include <exception>
#include <string>

#include <v8.h>
#include <libplatform/libplatform.h>

#include "v8pp/context.hpp"
#include "v8pp/version.hpp"

#include "tests.h"

void run_tests()
{
#define PCALL(func) std::cout << #func; func(); std::cout << " ok" << std::endl;
	try
	{
		PCALL(test_utility);
		PCALL(test_context);
		PCALL(test_convert);
		PCALL(test_throw_ex);
		PCALL(test_function);
		PCALL(test_ptr_traits);
		PCALL(test_call_v8);
		PCALL(test_call_from_v8);
		PCALL(test_factory);
		PCALL(test_module);
		PCALL(test_class);
		PCALL(test_property);
		PCALL(test_object);
		PCALL(test_json);
	}
	catch (std::exception const& ex)
	{
		std::cerr << " error: " << ex.what();
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char const* argv[])
{
	v8::V8::InitializeExternalStartupData(argv[0]);
	std::unique_ptr<v8::Platform> platform(v8::platform::NewDefaultPlatform());
	v8::V8::InitializePlatform(platform.get());
	v8::V8::Initialize();

	// your logics
	run_tests();

	v8::V8::Dispose();
	v8::V8::ShutdownPlatform();

	return 0;
}
