#include "Turtle.h"
#include "Screen.h"

MyTurtle::MyTurtle() : NativeClass("Turtle")
{
	allowedFields.emplace("@turtle", CXX::ObjectType::CONTAINER);

	methods.emplace("init", std::make_shared<NativeMethod>([&](Interpreter& interpreter, const std::vector<Object>& args)
		{
			if (!args[0].isInstance() || args[0].getInstance()->belonging->name() != "Screen") {
				throw std::runtime_error("Turtle needs to bind to a Screen instance");
			}

			auto screen = getScreenPtr(args[0].getInstance()->get("@screen"));
			Object& instance = interpreter.currContext()->get("this");
			instance.getInstance()->set("@turtle", Object(std::make_shared<TurtleWrapper>(screen->getScreen())));

			return Object();
		},
		1));

	methods.emplace("home", std::make_shared<NativeMethod>([&](Interpreter& interpreter, const std::vector<Object>& args)
		{
			Object& instance = interpreter.currContext()->get("this");
			TurtlePtr turtle = getTurtlePtr(instance.getInstance()->get("@turtle"));
			turtle->home();
			return Object();
		},
		0));

	methods.emplace("forward", std::make_shared<NativeMethod>([&](Interpreter& interpreter, const std::vector<Object>& args)
		{
			int step = 0;
			if (args[0].isNumber())
				step = args[0].getNumber();

			Object& instance = interpreter.currContext()->get("this");
			TurtlePtr turtle = getTurtlePtr(instance.getInstance()->get("@turtle"));
			turtle->forward(step);
			return Object();
		},
		1));

	methods.emplace("backward", std::make_shared<NativeMethod>([&](Interpreter& interpreter, const std::vector<Object>& args)
		{
			int step = 0;
			if (args[0].isNumber())
				step = args[0].getNumber();

			Object& instance = interpreter.currContext()->get("this");
			TurtlePtr turtle = getTurtlePtr(instance.getInstance()->get("@turtle"));
			turtle->backward(step);
			return Object();
		},
		1));

	methods.emplace("right", std::make_shared<NativeMethod>([&](Interpreter& interpreter, const std::vector<Object>& args)
		{
			double angle = 0;
			if (args[0].isNumber())
				angle = args[0].getNumber();

			Object& instance = interpreter.currContext()->get("this");
			TurtlePtr turtle = getTurtlePtr(instance.getInstance()->get("@turtle"));
			turtle->right(angle);
			return Object();
		},
		1));

	methods.emplace("left", std::make_shared<NativeMethod>([&](Interpreter& interpreter, const std::vector<Object>& args)
		{
			double angle = 0;
			if (args[0].isNumber())
				angle = args[0].getNumber();

			Object& instance = interpreter.currContext()->get("this");
			TurtlePtr turtle = getTurtlePtr(instance.getInstance()->get("@turtle"));
			turtle->left(angle);
			return Object();
		},
		1));

	methods.emplace("setheading", std::make_shared<NativeMethod>([&](Interpreter& interpreter, const std::vector<Object>& args)
		{
			double angle = 0;
			if (args[0].isNumber())
				angle = args[0].getNumber();

			Object& instance = interpreter.currContext()->get("this");
			TurtlePtr turtle = getTurtlePtr(instance.getInstance()->get("@turtle"));
			turtle->setheading(angle);
			return Object();
		},
		1));

	methods.emplace("goto", std::make_shared<NativeMethod>([&](Interpreter& interpreter, const std::vector<Object>& args)
		{
			if (!args[0].isNumber() || !args[1].isNumber())
				throw std::runtime_error("must goto a Position(number: x, number: y)");

			Object& instance = interpreter.currContext()->get("this");
			TurtlePtr turtle = getTurtlePtr(instance.getInstance()->get("@turtle"));
			turtle->goTo(args[0].getNumber(), args[1].getNumber());
			return Object();
		},
		2));

	methods.emplace("speed", std::make_shared<NativeMethod>([&](Interpreter& interpreter, const std::vector<Object>& args)
		{
			double speed = 0;
			if (args[0].isNumber())
				speed = args[0].getNumber();

			Object& instance = interpreter.currContext()->get("this");
			TurtlePtr turtle = getTurtlePtr(instance.getInstance()->get("@turtle"));
			turtle->speed(speed);
			return Object();
		},
		1));

	methods.emplace("circle", std::make_shared<NativeMethod>([&](Interpreter& interpreter, const std::vector<Object>& args)
		{
			int radius = 30, step = 15;
			if (args[0].isNumber())
				radius = args[0].getNumber();
			if (args[1].isNumber())
				step = args[1].getNumber();

			Object& instance = interpreter.currContext()->get("this");
			TurtlePtr turtle = getTurtlePtr(instance.getInstance()->get("@turtle"));
			turtle->circle(radius, step);
			return Object();
		},
		2));

	methods.emplace("write", std::make_shared<NativeMethod>([&](Interpreter& interpreter, const std::vector<Object>& args)
		{
			// 2 argument: text, font(optional)
			if (!args[0].isString() || !args[1].isString())
				throw std::runtime_error("Text/Font should be a string");

			Object& instance = interpreter.currContext()->get("this");
			TurtlePtr turtle = getTurtlePtr(instance.getInstance()->get("@turtle"));
			if (args.size() == 1)
				turtle->write(args[0].getString());
			else
				turtle->write(args[0].getString(), args[1].getString());
			
			return Object();
		},
		2, 1));

	methods.emplace("penup", std::make_shared<NativeMethod>([&](Interpreter& interpreter, const std::vector<Object>& args)
		{
			Object& instance = interpreter.currContext()->get("this");
			TurtlePtr turtle = getTurtlePtr(instance.getInstance()->get("@turtle"));
			turtle->penup();
			return Object();
		},
		0));

	methods.emplace("pendown", std::make_shared<NativeMethod>([&](Interpreter& interpreter, const std::vector<Object>& args)
		{
			Object& instance = interpreter.currContext()->get("this");
			TurtlePtr turtle = getTurtlePtr(instance.getInstance()->get("@turtle"));
			turtle->pendown();
			return Object();
		},
		0));

	methods.emplace("pencolor", std::make_shared<NativeMethod>([&](Interpreter& interpreter, const std::vector<Object>& args)
		{
			Object& instance = interpreter.currContext()->get("this");
			TurtlePtr turtle = getTurtlePtr(instance.getInstance()->get("@turtle"));
			if (args.size() == 1)
				turtle->pencolor(args[0].to_string());
			else if (args.size() == 3)
				turtle->pencolor(args[0].getNumber(), args[1].getNumber(), args[2].getNumber());
			
			return Object();
		},
		3, 2));

	methods.emplace("pensize", std::make_shared<NativeMethod>([&](Interpreter& interpreter, const std::vector<Object>& args)
		{
			if (!args[0].isNumber())
				throw std::runtime_error("pensize should be a number");

			Object& instance = interpreter.currContext()->get("this");
			TurtlePtr turtle = getTurtlePtr(instance.getInstance()->get("@turtle"));
			turtle->pensize(args[0].getNumber());
			return Object();
		},
		1));

	methods.emplace("fillcolor", std::make_shared<NativeMethod>([&](Interpreter& interpreter, const std::vector<Object>& args)
		{
			Object& instance = interpreter.currContext()->get("this");
			TurtlePtr turtle = getTurtlePtr(instance.getInstance()->get("@turtle"));
			if (args.size() == 1)
				turtle->fillcolor(args[0].to_string());
			else if (args.size() == 3)
				turtle->fillcolor(args[0].getNumber(), args[1].getNumber(), args[2].getNumber());

			return Object();
		},
		3, 2));

	methods.emplace("begin_fill", std::make_shared<NativeMethod>([&](Interpreter& interpreter, const std::vector<Object>& args)
		{
			Object& instance = interpreter.currContext()->get("this");
			TurtlePtr turtle = getTurtlePtr(instance.getInstance()->get("@turtle"));
			turtle->begin_fill();
			return Object();
		},
		0));

	methods.emplace("end_fill", std::make_shared<NativeMethod>([&](Interpreter& interpreter, const std::vector<Object>& args)
		{
			Object& instance = interpreter.currContext()->get("this");
			TurtlePtr turtle = getTurtlePtr(instance.getInstance()->get("@turtle"));
			turtle->end_fill();
			return Object();
		},
		0));

	methods.emplace("showTurtle", std::make_shared<NativeMethod>([&](Interpreter& interpreter, const std::vector<Object>& args)
		{
			Object& instance = interpreter.currContext()->get("this");
			TurtlePtr turtle = getTurtlePtr(instance.getInstance()->get("@turtle"));
			turtle->showTurtle();
			return Object();
		},
		0));

	methods.emplace("hideTurtle", std::make_shared<NativeMethod>([&](Interpreter& interpreter, const std::vector<Object>& args)
		{
			Object& instance = interpreter.currContext()->get("this");
			TurtlePtr turtle = getTurtlePtr(instance.getInstance()->get("@turtle"));
			turtle->hideTurtle();
			return Object();
		},
		0));

	methods.emplace("heading", std::make_shared<NativeMethod>([&](Interpreter& interpreter, const std::vector<Object>& args)
		{
			Object& instance = interpreter.currContext()->get("this");
			TurtlePtr turtle = getTurtlePtr(instance.getInstance()->get("@turtle"));
			
			return Object(turtle->heading());
		},
		0));
}

TurtlePtr getTurtlePtr(const Object& obj)
{
	return std::dynamic_pointer_cast<TurtleWrapper>(obj.getContainer());
}
