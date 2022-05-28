#pragma once
#include "Cxx_Core.h"
#include "CTurtle/CTurtle.hpp"

using CXX::Container;
using CXX::NativeClass;
using CXX::NativeMethod;
using CXX::Object;
using CXX::Interpreter;

class TurtleWrapper :public Container
{
public:
	TurtleWrapper(cturtle::TurtleScreen& screen) :Container("TurtleWrapper"), m_turtle(screen) {}
	
	// movements
	void forward(int step) { m_turtle.forward(step); }
	void backward(int step) {m_turtle.backward(step);}
	void left(double step) {m_turtle.left(step);}
	void right(double step) { m_turtle.right(step); }
	void setheading(double angle) { m_turtle.seth(angle); }
	void goTo(int x, int y) { m_turtle.goTo(x, y); }
	void speed(double val) { m_turtle.speed(val); cturtle::TS_FASTEST; }
	void circle(int radius, int step) { m_turtle.circle(radius, step, m_turtle.fillcolor()); }
	void write(const std::string& text, const std::string& font = cturtle::DEFAULT_FONT, const cturtle::Color& color = { "white" }, float scale = 1.0f, cturtle::TextAlign alignment = cturtle::TEXT_ALIGN_LEFT) { m_turtle.write(text, font, color, scale, alignment); }
	void home() { m_turtle.home(); }

	// penstate
	void penup() { m_turtle.penup(); }
	void pendown() { m_turtle.pendown(); }
	void pencolor(uint8_t r, uint8_t g, uint8_t b) { m_turtle.pencolor({ r,g,b }); }
	void pencolor(const std::string& name) { m_turtle.pencolor(name); }
	void pensize(int width) { m_turtle.width(width); }

	// color related
	void fillcolor(const std::string& color) { m_turtle.fillcolor({ color }); }
	void fillcolor(uint8_t r, uint8_t g, uint8_t b) { m_turtle.fillcolor({ r,g,b }); }
	void begin_fill() { m_turtle.begin_fill(); }
	void end_fill() { m_turtle.end_fill(); }

	// turtle itself
	void showTurtle() { m_turtle.showturtle(); }
	void hideTurtle() { m_turtle.hideturtle(); }
	double heading() { return m_turtle.heading(); }

	// override Container
	std::string to_string() override { return "Turtle"; }

private:
	cturtle::Turtle m_turtle;
};

using TurtlePtr = std::shared_ptr<TurtleWrapper>;

TurtlePtr getTurtlePtr(const Object& obj);

class MyTurtle :public NativeClass
{
public:
	MyTurtle();
};