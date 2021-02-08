//
// Copyright (c) 2013-2016 Pavel Medvedev. All rights reserved.
//
// This file is part of v8pp (https://github.com/pmed/v8pp) project.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include "v8pp/function.hpp"
#include "v8pp/context.hpp"

#include "test.hpp"

#include <iostream>

static int f(int const& x) { return x; }
static std::string g(char const* s) { return s? s : ""; }
static int h(v8::Isolate*, int x, int y) { return x + y; }
static int testAdd(v8::Isolate*, int x, int y) { return x + y; }

static void __uniqslog(const char* s)
{
	if (s)
	{
		std::cout << "__uniqslog " << s << std::endl;
	}
}

const char* ToCString(const v8::String::Utf8Value& value) {
	return *value ? *value : "<string conversion failed>";
}

void Print(const v8::FunctionCallbackInfo<v8::Value>& args) {
	bool first = true;
	for (int i = 0; i < args.Length(); i++) {
		v8::HandleScope handle_scope(args.GetIsolate());
		if (first) {
			first = false;
		}
		else {
			printf(" ");
		}
		v8::String::Utf8Value str(args.GetIsolate(), args[i]);
		const char* cstr = ToCString(str);
		printf("%s", cstr);
	}
	printf("\n");
	fflush(stdout);
}

struct X
{
	int operator()(int x) const { return -x; }
};

void test_setPreFunctions(v8pp::context& context)
{
	v8::Isolate* isolate = context.isolate();
	v8::HandleScope scope(isolate);

	context.set("__uniqslog", v8pp::wrap_function(isolate, "__uniqslog", &__uniqslog));
	//run_script(context, "__uniqslog(1234);");
	run_script(context, "__uniqslog('asdf');");
	run_script(context, "print('print function. string: hello world');");
	run_script(context, "print('print function. int:', 1234);");
}

// 据谣传说多写注释可以让程序员长的更帅
#define GetIsolateWithScope(context) context.isolate();v8::HandleScope scope(isolate);

void test_function_passContext(v8pp::context& context) {
	{
		v8::Isolate* isolate = context.isolate();

		{
			v8::HandleScope scope(isolate);

			context.set("f", v8pp::wrap_function(isolate, "f", &f));
			check_eq("f", run_script<int>(context, "f(1)"), 1);
		}
		{
			v8::HandleScope scope(isolate);
			check_eq("f", run_script<int>(context, "f(1024)"), 1024);
		}
	}
	{
		v8::Isolate* isolate = GetIsolateWithScope(context);
		{
			context.set("f", v8pp::wrap_function(isolate, "f", &f));
			check_eq("f", run_script<int>(context, "f(1)"), 1);
		}
		{
			context.set("f", v8pp::wrap_function(isolate, "f", &f));
			check_eq("f", run_script<int>(context, "f(1)"), 1);
		}
	}
}

void test_function()
{
	// original tests.
	{
		v8pp::context context;
		v8::Isolate* isolate = context.isolate();
		v8::HandleScope scope(isolate);

		context.set("f", v8pp::wrap_function(isolate, "f", &f));
		check_eq("f", run_script<int>(context, "f(1)"), 1);

		context.set("g", v8pp::wrap_function(isolate, "g", &g));
		check_eq("g", run_script<std::string>(context, "g('abc')"), "abc");

		context.set("h", v8pp::wrap_function(isolate, "h", &h));
		check_eq("h", run_script<int>(context, "h(1, 2)"), 3);

		int x = 1, y = 2;
		auto lambda = [x, y](int z) { return x + y + z; };
		context.set("lambda", v8pp::wrap_function(isolate, "lambda", lambda));
		check_eq("lambda", run_script<int>(context, "lambda(3)"), 6);

		auto lambda2 = []() { return 99; };
		context.set("lambda2", v8pp::wrap_function(isolate, "lambda2", lambda2));
		check_eq("lambda2", run_script<int>(context, "lambda2()"), 99);

		X xfun;
		context.set("xfun", v8pp::wrap_function(isolate, "xfun", xfun));
		check_eq("xfun", run_script<int>(context, "xfun(5)"), -5);

		std::function<int(int)> fun = f;
		context.set("fun", v8pp::wrap_function(isolate, "fun", fun));
		check_eq("fun", run_script<int>(context, "fun(42)"), 42);
	}


	{
		v8pp::context context;
		{
			v8::Isolate* isolate = context.isolate();

			{
				v8::HandleScope scope(isolate);

				context.set("f", v8pp::wrap_function(isolate, "f", &f));
				check_eq("f", run_script<int>(context, "f(1)"), 1);
			}
			{
				v8::HandleScope scope(isolate);
				check_eq("f", run_script<int>(context, "f(1024)"), 1024);
			}
		}
		{
			v8::Isolate* isolate = context.isolate();

			{
				v8::HandleScope scope(isolate);

				context.set("f", v8pp::wrap_function(isolate, "f", &f));
				check_eq("f", run_script<int>(context, "f(1)"), 1);
			}
			{
				v8::HandleScope scope(isolate);
				check_eq("f", run_script<int>(context, "f(1024)"), 1024);
			}
		}
		test_function_passContext(context);
	}
	{
		v8pp::context context;
		test_setPreFunctions(context);
		{
			v8::Isolate* isolate = context.isolate();

			{
				v8::HandleScope scope(isolate);

				context.set("f", v8pp::wrap_function(isolate, "f", &f));
				check_eq("f", run_script<int>(context, "f(1)"), 1);
			}
			{
				v8::HandleScope scope(isolate);
				check_eq("f", run_script<int>(context, "f(1024)"), 1024);
			}
		}
	}
}
