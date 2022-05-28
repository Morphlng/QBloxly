#include "Screen.h"

MyScreen::MyScreen() : NativeClass("Screen")
{
	allowedFields.emplace("@screen", CXX::ObjectType::CONTAINER);

	methods.emplace("init", std::make_shared<NativeMethod>([&](Interpreter& interpreter, const std::vector<Object>& args)
		{
			Object& instance = interpreter.currContext()->get("this");
			switch (args.size()) {
			case 0: 
			{
				instance.getInstance()->set("@screen", Object(std::make_shared<ScreenWrapper>()));
				break; 
			}

			case 1:
			{
				instance.getInstance()->set("@screen", Object(std::make_shared<ScreenWrapper>(args[0].to_string())));
				break;
			}

			case 2:
			{
				if (!args[0].isNumber() || !args[1].isNumber()) {
					throw std::runtime_error("Expecting width and height as Number");
				}

				instance.getInstance()->set("@screen", Object(std::make_shared<ScreenWrapper>(args[0].getNumber(), args[1].getNumber())));
				break;
			}

			case 3:
			{
				if (!args[0].isNumber() || !args[1].isNumber()) {
					throw std::runtime_error("Expecting width and height as Number");
				}

				instance.getInstance()->set("@screen", Object(std::make_shared<ScreenWrapper>(args[0].getNumber(), args[1].getNumber(), args[2].to_string())));
				break;
			}

			}
			
			return Object();
		},
		3, 3));

	methods.emplace("clear", std::make_shared<NativeMethod>([&](Interpreter& interpreter, const std::vector<Object>& args)
		{
			Object& instance = interpreter.currContext()->get("this");
			ScreenPtr screen = getScreenPtr(instance.getInstance()->get("@screen"));
			screen->clear();
			return Object();
		},
		0));

	methods.emplace("bgcolor", std::make_shared<NativeMethod>([&](Interpreter& interpreter, const std::vector<Object>& args)
		{
			Object& instance = interpreter.currContext()->get("this");
			ScreenPtr screen = getScreenPtr(instance.getInstance()->get("@screen"));
			screen->bgcolor(args[0].to_string());
			return Object();
		},
		1));

	methods.emplace("bgpic", std::make_shared<NativeMethod>([&](Interpreter& interpreter, const std::vector<Object>& args)
		{
			Object& instance = interpreter.currContext()->get("this");
			ScreenPtr screen = getScreenPtr(instance.getInstance()->get("@screen"));
			screen->bgpic(args[0].to_string());
			return Object();
		},
		1));

	methods.emplace("bye", std::make_shared<NativeMethod>([&](Interpreter& interpreter, const std::vector<Object>& args)
		{
			Object& instance = interpreter.currContext()->get("this");
			ScreenPtr screen = getScreenPtr(instance.getInstance()->get("@screen"));
			screen->bye();
			return Object(); 
		},
		0));

	methods.emplace("exitonclick", std::make_shared<NativeMethod>([&](Interpreter& interpreter, const std::vector<Object>& args)
		{
			Object& instance = interpreter.currContext()->get("this");
			ScreenPtr screen = getScreenPtr(instance.getInstance()->get("@screen"));
			screen->exitonclick();
			return Object();
		},
		0));

	methods.emplace("save", std::make_shared<NativeMethod>([&](Interpreter& interpreter, const std::vector<Object>& args)
		{
			Object& instance = interpreter.currContext()->get("this");
			ScreenPtr screen = getScreenPtr(instance.getInstance()->get("@screen"));
			
			std::string filename = args[0].to_string();
			if (filename.compare(filename.length() - 4, 4, ".bmp") != 0)
				filename += ".bmp";
			
			screen->save(filename);
			return Object();
		},
		1));

	methods.emplace("tracer", std::make_shared<NativeMethod>([&](Interpreter& interpreter, const std::vector<Object>& args)
		{
			if (!args[0].isNumber() || !args[1].isNumber()) {
				throw std::runtime_error("Expecting count and delay as Unsigned Number");
			}

			Object& instance = interpreter.currContext()->get("this");
			ScreenPtr screen = getScreenPtr(instance.getInstance()->get("@screen"));
			screen->tracer(args[0].getNumber(), args[1].getNumber());
			return Object();
		},
		2));

	methods.emplace("mode", std::make_shared<NativeMethod>([&](Interpreter& interpreter, const std::vector<Object>& args)
		{
			std::string input = args[0].to_string();
			std::transform(input.begin(), input.end(), input.begin(), std::toupper);
			
			cturtle::ScreenMode mode = cturtle::SM_STANDARD;
			if (input == "LOGO")
				mode == cturtle::SM_LOGO;

			Object& instance = interpreter.currContext()->get("this");
			ScreenPtr screen = getScreenPtr(instance.getInstance()->get("@screen"));
			screen->mode(mode);
			return Object();
		},
		1));
}

ScreenPtr getScreenPtr(const Object& obj)
{
	return std::dynamic_pointer_cast<ScreenWrapper>(obj.getContainer());
}
