#pragma once
#include "Cxx_Core.h"
#include "CTurtle/CTurtle.hpp"

using CXX::Container;
using CXX::NativeClass;
using CXX::NativeMethod;
using CXX::Object;
using CXX::Interpreter;

class ScreenWrapper :public Container
{
public:
	ScreenWrapper(int width = cturtle::SCREEN_DEFAULT_WIDTH, int height = cturtle::SCREEN_DEFAULT_HEIGHT, const std::string& title = "CxxTurtle") :Container("ScreenWrapper"), m_scr{ width,height,title }{}
	ScreenWrapper(const std::string& title) :Container("ScreenWrapper"), m_scr{ title }{}
	cturtle::TurtleScreen& getScreen() { return m_scr; }

	void bgcolor(const std::string& color) { m_scr.bgcolor({color}); }
	void bgpic(const std::string& path) { cturtle::Image pic; pic.load(path.c_str()); m_scr.bgpic(pic); }
	void clear() { m_scr.clearscreen(); }
	void save(const std::string& filepath) { m_scr.save(filepath); }
	void bye() { m_scr.bye(); }
	void exitonclick() { m_scr.exitonclick(); }
	void tracer(int count, unsigned int delay) { m_scr.tracer(count, delay); }
	void mode(cturtle::ScreenMode mode) { m_scr.mode(mode); }

	std::string to_string() override { return "Screen"; }

private:
	cturtle::TurtleScreen m_scr;
};

using ScreenPtr = std::shared_ptr<ScreenWrapper>;

ScreenPtr getScreenPtr(const Object& obj);

class MyScreen :public NativeClass
{
public:
	MyScreen();
};