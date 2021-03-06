#include "Interpreter/Interpreter.h"
#include "Common/utils.h"
#include "Interpreter/Function.h"
#include "Interpreter/MetaList.h"
#include "Interpreter/loxlib/NativeClass.h"
#include "Interpreter/loxlib/StandardFunctions.h"
#include "Lexer/Lexer.h"
#include "Parser/Parser.h"
#include "Resolver/Resolver.h" // above headers are used in loadModule
#include "Runner.h"
#include <algorithm>
#include <iostream>
#include <QThread>

namespace CXX {

	Interpreter::Interpreter()
	{
		presetContext = std::make_shared<Context>();
		context = std::make_shared<Context>(presetContext);
		globalContext = context;
		globalContext->set("__name__", Object(std::string("__main__")));
		loadPresetEnvironment();
	}

	void Interpreter::interpret(const std::vector<StmtPtr>& statements)
	{
		for (auto& stmt : statements)
		{
			execute(stmt.get());
		}
	}

	void Interpreter::interpret(std::vector<StmtPtr>&& statements)
	{
		for (auto& stmt : statements)
		{
			execute(stmt.get());
			stmt.reset();
		}
	}

	void Interpreter::visit(const ExpressionStmt* expressionStmt)
	{
		Object result = interpret(expressionStmt->expr.get());

		if (replEcho && !result.isNil())
		{
			std::cout << result.to_string() << "\n";
		}
	}

	void Interpreter::visit(const VarDeclarationStmt* varStmt)
	{
		if (varStmt->expr)
		{
			Object initializer = interpret(varStmt->expr.value().get());
			context->set(varStmt->identifier, initializer);
		}
		else
		{
			context->set(varStmt->identifier, Object::Nil());
		}
	}

	void Interpreter::visit(std::shared_ptr<FuncDeclarationStmt> funcDeclarationStmt)
	{
		std::shared_ptr<Function> function = std::make_shared<Function>(Object::Nil(), funcDeclarationStmt, context);
		context->set(funcDeclarationStmt->name, Object(std::move(function)));
	}

	void Interpreter::visit(const ClassDeclarationStmt* classDeclStmt)
	{
		// 在定义类时，我们将分两步构造，先声明，再赋值
		// 这样可以允许类内函数调用该类
		context->set(classDeclStmt->name, Object::Nil());

		std::optional<std::shared_ptr<Class>> superClass;
		if (classDeclStmt->superClass)
		{
			Object superclassObject = interpret(classDeclStmt->superClass.value().get());
			if (!superclassObject.isCallable() || superclassObject.getCallable()->type != Callable::CallableType::CLASS)
			{
				throw RuntimeError(classDeclStmt->superClass.value()->pos_start, classDeclStmt->superClass.value()->pos_end,
					"SuperClass must be a Class");
			}

			superClass = std::static_pointer_cast<Class>(superclassObject.getCallable());
		}

		std::unordered_map<std::string, CallablePtr> methods;
		std::shared_ptr<Class> classPtr = std::make_shared<Class>(classDeclStmt->name.lexeme, methods, std::move(superClass));
		context->change(classDeclStmt->name, Object(classPtr));

		// 因为我们需要给类成员函数绑定所处类，因此我们只能先定义类，再添加函数
		if (!classDeclStmt->methods.empty())
		{
			Object& classObject = context->get(classDeclStmt->name);
			for (auto& method : classDeclStmt->methods)
			{
				std::string func_name = method->name.lexeme;
				methods[func_name] = std::make_shared<Function>(classObject, method, context);
			}
			classPtr->methods = std::move(methods);
		}
	}

	void Interpreter::visit(const BlockStmt* blockStmt)
	{
		// 循环体，判断语句的输出控制
		auto task = toggleRepl();

		ScopedContext scope(context, std::make_shared<Context>(context));

		for (auto& stmt : blockStmt->statements)
		{
			execute(stmt.get());

			if (m_returns)
				return;
		}
	}

	void Interpreter::visit(const IfStmt* ifStmt)
	{
		if (interpret(ifStmt->condition.get()).is_true())
		{
			execute(ifStmt->thenBranch.get());
		}
		else if (ifStmt->elseBranch)
		{
			execute(ifStmt->elseBranch.value().get());
		}
	}

	void Interpreter::visit(const WhileStmt* whileStmt)
	{
		while (interpret(whileStmt->condition.get()).is_true())
		{
			try
			{
				execute(whileStmt->body.get());
			}
			catch (const BreakFlag& e)
			{
				return;
			}
			catch (const ContinueFlag& e)
			{
				// continue;
			}

			if (m_returns)
				return;
		}
	}

	void Interpreter::visit(const ForStmt* forStmt)
	{
		// for循环内是一个新的变量环境
		// for语句的第一个变量声明应设为新变量
		ScopedContext scoped(context, std::make_shared<Context>(context));

		if (forStmt->initializer)
			execute(forStmt->initializer.value().get());

		bool hasCondition = forStmt->condition.has_value();

		while (!hasCondition || interpret(forStmt->condition.value().get()).is_true())
		{
			try
			{
				execute(forStmt->body.get());
			}
			catch (const BreakFlag& e)
			{
				return;
			}
			catch (const ContinueFlag& e)
			{
				// continue;
			}

			if (m_returns)
				return;

			if (forStmt->increment)
				interpret(forStmt->increment.value().get());
		}
	}

	void Interpreter::visit(const BreakStmt* breakStmt)
	{
		throw BreakFlag();
	}

	void Interpreter::visit(const ContinueStmt* continueStmt)
	{
		throw ContinueFlag();
	}

	void Interpreter::visit(const ReturnStmt* returnStmt)
	{
		if (returnStmt->expr)
			m_returns = interpret(returnStmt->expr.value().get());
		else
			m_returns = Object();
	}

	void Interpreter::visit(const ImportStmt* importStmt)
	{
		std::shared_ptr<Module> importModule;
		if (auto it = m_modules.find(importStmt->filepath.lexeme); it != m_modules.end())
		{
			importModule = it->second;
		}
		else
		{
			importModule = loadModule(importStmt->filepath);
			if (ErrorReporter::count())
			{
				throw RuntimeError(importStmt->pos_start, importStmt->pos_end, "Failed to import Module, error occured");
			}
			m_modules.emplace(importStmt->filepath.lexeme, importModule);
		}

		// import { * } from "module"
		if (importStmt->symbols.begin()->first.type == TokenType::MUL)
		{
			for (auto& [name, obj] : importModule->m_values)
			{
				context->set(name, obj);
			}
		}
		else
		{
			for (const auto& [symbol, alias] : importStmt->symbols)
			{
				if (auto& obj = importModule->get(symbol.lexeme); &obj != &Object::Nil())
				{
					const std::string& name = alias ? alias->lexeme : symbol.lexeme;
					context->set(name, obj);
				}
				else
				{
					throw RuntimeError(symbol.pos_start, symbol.pos_end, format("Can't find `%s` from module \"%s\".", symbol.lexeme.c_str(), importStmt->filepath.lexeme.c_str()));
				}
			}
		}
	}

	void Interpreter::visit(const PackStmt* packStmt)
	{
		for (auto const& stmt : packStmt->statements)
		{
			execute(stmt.get());
		}
	}

	Object Interpreter::visit(const BinaryExpr* binaryExpr)
	{
		Object left = interpret(binaryExpr->left.get()), right = interpret(binaryExpr->right.get());

		switch (binaryExpr->op.type)
		{
		case TokenType::PLUS:
			return left + right;

		case TokenType::MINUS:
			return left - right;

		case TokenType::MUL:
			return left * right;

		case TokenType::DIV:
			return left / right;

		case TokenType::MOD:
			return left % right;

		case TokenType::GT:
			return Object(left > right);

		case TokenType::GTE:
			return Object(left >= right);

		case TokenType::LT:
			return Object(left < right);

		case TokenType::LTE:
			return Object(left <= right);

		case TokenType::EQEQ:
			return Object(left == right);

		case TokenType::BANGEQ:
			return Object(left != right);

		default:
			throw RuntimeError(binaryExpr->pos_start, binaryExpr->pos_end, "Invalid Binary operand");
		}
	}

	Object Interpreter::visit(const UnaryExpr* unaryExpr)
	{
		Object expr = interpret(unaryExpr->expr.get());

		switch (unaryExpr->op.type)
		{
		case TokenType::MINUS:
			return -expr;
		case TokenType::BANG:
			return !expr;
		default:
			throw RuntimeError(unaryExpr->pos_start, unaryExpr->pos_end, "Invalid Binary operand");
		}
	}

	Object Interpreter::visit(const LiteralExpr* literalExpr)
	{
		return literalExpr->value;
	}

	Object Interpreter::visit(const VariableExpr* variableExpr)
	{
		Object& var = lookupVariable(variableExpr->identifier, variableExpr->depth);
		if (&var == &Object::Nil())
		{
			throw RuntimeError(variableExpr->identifier.pos_start, variableExpr->identifier.pos_end,
				format("Undefined variable %s", variableExpr->identifier.lexeme.c_str()));
		}

		return var;
	}

	Object Interpreter::visit(const AssignmentExpr* assignmentExpr)
	{
		Object& var = lookupVariable(assignmentExpr->identifier, assignmentExpr->depth);
		if (&var == &Object::Nil())
		{
			// 注意这里是和静态成员Object::Nil()去比（地址）
			// 成员可以赋值为nil，那将进行一个拷贝
			throw RuntimeError(assignmentExpr->identifier.pos_start, assignmentExpr->value->pos_end,
				format("Undefined variable %s", assignmentExpr->identifier.lexeme.c_str()));
		}

		Object value = interpret(assignmentExpr->value.get());
		TokenType op_type = assignmentExpr->operation.type;
		value = handleAssign(var, value, op_type);

		// 因为var是一个引用，所以可以直接改
		var = value;
		return value;
	}

	Object Interpreter::visit(const TernaryExpr* ternaryExpr)
	{
		Object check = interpret(ternaryExpr->expr.get());

		if (check.is_true())
			return interpret(ternaryExpr->thenBranch.get());
		else
			return interpret(ternaryExpr->elseBranch.get());
		;
	}

	Object Interpreter::visit(std::shared_ptr<LambdaExpr> lambdaExpr)
	{
		std::shared_ptr<LambdaFunction> function = std::make_shared<LambdaFunction>(lambdaExpr, context);
		return Object(std::move(function));
	}

	Object Interpreter::visit(const OrExpr* orExpr)
	{
		// 或运算，一真则真
		Object lhs = interpret(orExpr->left.get());
		if (lhs.is_true())
		{
			return Object(true);
		}

		Object rhs = interpret(orExpr->right.get());
		return Object(rhs.is_true());
	}

	Object Interpreter::visit(const AndExpr* andExpr)
	{
		// 与运算，一假则假
		Object lhs = interpret(andExpr->left.get());
		if (!lhs.is_true())
		{
			return Object(false);
		}

		Object rhs = interpret(andExpr->right.get());
		return Object(rhs.is_true());
	}

	Object Interpreter::visit(const IncrementExpr* incrementExpr)
	{
		using OpType = RetrieveExpr::OpType;

		Object prev = interpret(incrementExpr->holder.get());
		if (!prev.isNumber())
			throw RuntimeError(incrementExpr->holder->pos_start, incrementExpr->holder->pos_end,
				format("Operator '++' does not support type(%s)", ObjectTypeName(prev.type)));

		Object result = prev + Object(1.0);

		if (incrementExpr->holder->exprType == ExprType::Variable)
		{
			auto var = static_cast<VariableExpr*>(incrementExpr->holder.get());
			context->change(var->identifier, result);
		}
		else
		{
			auto retrieve = static_cast<RetrieveExpr*>(incrementExpr->holder.get());
			Object holder = interpret(retrieve->holder.get());
			if (Classifier::belongClass(holder, "List") && retrieve->type == OpType::BRACKET)
			{
				Object index = interpret(retrieve->index.get());
				if (!index.isNumber())
					throw RuntimeError(retrieve->index->pos_start, retrieve->index->pos_end, "Index should be a number");

				Object& oldVal = listAt(holder, index);
				oldVal = result;
			}
			else if (holder.isInstance())
			{
				if (retrieve->type == OpType::DOT)
					holder.getInstance()->set(retrieve->identifier, result);
				else
				{
					Object attr = interpret(retrieve->index.get());
					if (!attr.isString())
						throw RuntimeError(retrieve->index->pos_start, retrieve->index->pos_end, "Attr should be a string");
					holder.getInstance()->set(attr.getString(), result);
				}
			}
		}

		if (incrementExpr->type == IncrementExpr::Type::POSTFIX)
		{
			return prev;
		}

		return result;
	}

	Object Interpreter::visit(const DecrementExpr* decrementExpr)
	{
		using OpType = RetrieveExpr::OpType;

		Object prev = interpret(decrementExpr->holder.get());
		if (!prev.isNumber())
		{
			throw RuntimeError(decrementExpr->holder->pos_start, decrementExpr->holder->pos_end,
				format("Operator '--' does not support type(%s)", ObjectTypeName(prev.type)));
		}

		Object result = prev - Object(1.0);

		if (decrementExpr->holder->exprType == ExprType::Variable)
		{
			auto var = static_cast<VariableExpr*>(decrementExpr->holder.get());
			context->change(var->identifier, result);
		}
		else
		{
			auto retrieve = static_cast<RetrieveExpr*>(decrementExpr->holder.get());
			Object holder = interpret(retrieve->holder.get());
			if (Classifier::belongClass(holder, "List") && retrieve->type == OpType::BRACKET)
			{
				Object index = interpret(retrieve->index.get());
				if (!index.isNumber())
					throw RuntimeError(retrieve->index->pos_start, retrieve->index->pos_end, "Index should be a number");

				Object& oldVal = listAt(holder, index);
				oldVal = result;
			}
			else if (holder.isInstance())
			{
				if (retrieve->type == OpType::DOT)
					holder.getInstance()->set(retrieve->identifier, result);
				else
				{
					Object attr = interpret(retrieve->index.get());
					if (!attr.isString())
						throw RuntimeError(retrieve->index->pos_start, retrieve->index->pos_end, "Attr should be a string");
					holder.getInstance()->set(attr.getString(), result);
				}
			}
		}

		if (decrementExpr->type == DecrementExpr::Type::POSTFIX)
		{
			return prev;
		}

		return result;
	}

	Object Interpreter::visit(const CallExpr* callExpr)
	{
		Object callee = interpret(callExpr->callee.get());

		if (!callee.isCallable())
		{
			throw RuntimeError(callExpr->callee->pos_start, callExpr->callee->pos_end, "Expression is not callable");
		}

		std::vector<Object> args;
		for (auto& arg : callExpr->arguments)
		{
			args.push_back(interpret(arg.get()));
		}

		CallablePtr callable = callee.getCallable();
		size_t arg_size = args.size();

		// 当函数参数元数为-1时，表示接收不限量参数，仅内置函数支持
		// 否则实参个数应在范围：必须参数 <= 实参个数 <= 形参个数
		if (callable->arity() != -1 && (arg_size < callable->required_params() || arg_size > callable->arity()))
		{
			throw RuntimeError(callExpr->pos_start, callExpr->pos_end,
				format("Function expected %d argument(s), %d is required, only got %d", callable->arity(), callable->required_params(), arg_size));
		}

		Callable* prev = currentFunction;
		currentFunction = callable.get();

		auto task = toggleRepl();

		Object result = callable->call(*this, args);
		currentFunction = prev;

		return result;
	}

	Object Interpreter::visit(const RetrieveExpr* retrieveExpr)
	{
		using OpType = RetrieveExpr::OpType;

		Object holder = interpret(retrieveExpr->holder.get());
		if (Classifier::belongClass(holder, "List") && retrieveExpr->type == OpType::BRACKET)
		{
			Object index = interpret(retrieveExpr->index.get());
			if (!index.isNumber())
			{
				throw RuntimeError(retrieveExpr->index->pos_start, retrieveExpr->index->pos_end, "Index should be a number");
			}

			return listAt(holder, index);
		}
		else if (holder.isInstance())
		{
			// 目前的设计是，如果对象没有索取的属性，则返回Nil
			// 这种设计和JavaScript一致，但容易造成bug
			if (retrieveExpr->type == OpType::DOT)
				return holder.getInstance()->get(retrieveExpr->identifier);
			else
			{
				Object attr = interpret(retrieveExpr->index.get());
				if (!attr.isString())
				{
					throw RuntimeError(retrieveExpr->index->pos_start, retrieveExpr->index->pos_end, "attribute should be a string");
				}

				return holder.getInstance()->get(attr.getString());
			}
		}

		const char* op = retrieveExpr->type == OpType::DOT ? "." : "[]";
		throw RuntimeError(retrieveExpr->pos_start, retrieveExpr->pos_end,
			format("Cannot apply %s to object type(%s)", op, ObjectTypeName(holder.type)));
	}

	Object Interpreter::visit(const SetExpr* setExpr)
	{
		using OpType = RetrieveExpr::OpType;

		Object holder = interpret(setExpr->holder.get());

		if (holder.isInstance())
		{
			if (setExpr->type == OpType::BRACKET)
			{
				Object attr = interpret(setExpr->index.get());
				if (!attr.isString())
				{
					throw RuntimeError(setExpr->index->pos_start, setExpr->index->pos_end, "Attribute should be a string");
				}
				const_cast<SetExpr*>(setExpr)->identifier.lexeme = attr.getString();
			}

			Object prev = holder.getInstance()->get(setExpr->identifier);
			// 要赋予或改变的新value
			Object value = interpret(setExpr->value.get());
			value = handleAssign(prev, value, setExpr->operation.type);
			holder.getInstance()->set(setExpr->identifier, value);
			return value;
		}
		else if (Classifier::belongClass(holder, "List") && setExpr->type == OpType::BRACKET)
		{
			// 这里为了拿到引用而不是复制，所以重复了Retrieve中的代码
			Object index = interpret(setExpr->index.get());
			if (!index.isNumber())
			{
				throw RuntimeError(setExpr->index->pos_start, setExpr->index->pos_end, "Index should be a number");
			}

			Object& prev = listAt(holder, index);

			// 要赋予或改变的新value
			Object value = interpret(setExpr->value.get());
			value = handleAssign(prev, value, setExpr->operation.type);

			prev = value;
			return value;
		}

		const char* op = setExpr->type == OpType::DOT ? "." : "[]";
		throw RuntimeError(setExpr->pos_start, setExpr->pos_end,
			format("Cannot apply %s to object type(%s)", op, ObjectTypeName(holder.type)));

		return Object();
	}

	Object Interpreter::visit(const ThisExpr* thisExpr)
	{
		return lookupVariable(thisExpr->keyword, thisExpr->depth);
	}

	Object Interpreter::visit(const SuperExpr* superExpr)
	{
		// Resolver中应当检查过super使用的合法性
		// 因此不应出现在非子类中调用，在非类函数中调用的情况
		std::shared_ptr<Class> superClass;
		if (currentFunction->type == Callable::CallableType::FUNCTION)
		{
			Object& belonging = static_cast<Function*>(currentFunction)->belonging;
			superClass = std::static_pointer_cast<Class>(belonging.getCallable())->superClass.value();
		}
		else if (currentFunction->type == Callable::CallableType::CLASS)
		{
			superClass = static_cast<Class*>(currentFunction)->superClass.value();
		}

		CallablePtr method = superClass->findMethods(superExpr->identifier.lexeme);
		if (!method)
			throw RuntimeError(superExpr->pos_start, superExpr->pos_end,
				format("Undefined method %s", superExpr->identifier.lexeme.c_str()));

		int distance = superExpr->depth;
		Object& instance = context->getAt("this", distance);
		CallablePtr bindMethod = method->bindThis(instance.getInstance());

		return Object(bindMethod);
	}

	Object Interpreter::visit(const ListExpr* listExpr)
	{
		std::vector<Object> items;
		for (auto& expr : listExpr->items)
			items.push_back(interpret(expr.get()));

		return Object(List::instantiate(std::move(items)));
	}

	Object Interpreter::visit(const PackExpr* packExpr)
	{
		Object ret;
		for (auto const& expr : packExpr->expressions)
		{
			ret = interpret(expr.get());
		}

		// 对于用','分隔的一整句，返回最后一个值
		return ret;
	}

	Object Interpreter::interpret(Expr* expr)
	{
        if (QThread::currentThread()->isInterruptionRequested()) {
            throw KeyBoardInterruption(expr->pos_start, expr->pos_end, "Keyboard interrupt by user");
        }

        // Runner会跟踪当前执行位置
        Runner::pos_start = &expr->pos_start;
		Runner::pos_end = &expr->pos_end;
		return expr->accept(*this);
	}

	void Interpreter::execute(Stmt* pStmt)
	{
        if (QThread::currentThread()->isInterruptionRequested()) {
            throw KeyBoardInterruption(pStmt->pos_start,
                                       pStmt->pos_end,
                                       "Keyboard interrupt by user");
        }

        // 实际上从执行角度，最终报错一定聚焦于Expr
		// 但是这个操作代价不大，我们可以做
		Runner::pos_start = &pStmt->pos_start;
		Runner::pos_end = &pStmt->pos_end;

		pStmt->accept(*this);
	}

	void Interpreter::loadPresetEnvironment()
	{
		// 内置函数
		auto clock = std::make_shared<standardFunctions::Clock>();
		auto str = std::make_shared<standardFunctions::Str>();
		auto chr = std::make_shared<standardFunctions::Chr>();
		auto getc = std::make_shared<standardFunctions::GetC>();
		auto exit = std::make_shared<standardFunctions::Exit>();
		auto typo = std::make_shared<standardFunctions::TypeOf>();
		auto print = std::make_shared<standardFunctions::Print>();
		auto getattr = std::make_shared<standardFunctions::GetAttr>();
		auto loadlib = std::make_shared<standardFunctions::Loadlib>();

		// 内置类
		auto StringClass = String::getSingleton();
		auto ListClass = List::getSingleton();

		std::vector<Object> built_in_functions = {
			Object(std::move(clock)), Object(std::move(str)), Object(std::move(typo)),
			Object(std::move(chr)), Object(std::move(getc)), Object(std::move(exit)),
			Object(std::move(print)), Object(std::move(getattr)), Object(std::move(loadlib)),
			Object(std::move(StringClass)), Object(std::move(ListClass)) };

		for (auto const& func : built_in_functions)
		{
			presetContext->set(func.getCallable()->name(), func);
		}

		// 内置变量
		presetContext->set("Math", Object(Mathematics::instantiate()));
	}

	Object& Interpreter::lookupVariable(const Token& identifier, int depth)
	{
		if (depth != -1)
		{
			return context->getAt(identifier, depth);
		}

		return globalContext->get(identifier);
	}

	Object Interpreter::handleAssign(const Object& prev, const Object& value, TokenType op)
	{
		switch (op)
		{
		case TokenType::PLUS_EQUAL:
			return prev + value; // prev += value => prev = prev + value
		case TokenType::MINUS_EQUAL:
			return prev - value; // prev -= value => prev = prev - value
		case TokenType::MUL_EQUAL:
			return prev * value; // prev *= value => prev = prev * value
		case TokenType::DIV_EQUAL:
			return prev / value; // prev /= value => prev = prev / value
		case TokenType::EQ:
			return value;
		default:
			// impossible
			return Object();
		}
	}

	Object& Interpreter::listAt(const Object& holder, const Object& index)
	{
		// 内部类List的唯一成员items是一个MetaList
		auto list = holder.getInstance()->get("@items");

		// index必须是Number，在调用该函数前应检查
		return getMetaList(list)->at((int)index.getNumber());
	}

	std::shared_ptr<Module> Interpreter::loadModule(const Token& filepath)
	{
		std::optional<std::string> fileContent = readfile(filepath.lexeme);
		if (!fileContent)
		{
			throw RuntimeError(filepath.pos_start, filepath.pos_end, "Error in loading Module from file:" + filepath.lexeme);
		}

#ifdef USE_STRING_VIEW
		static std::vector<std::string> TEXT;
		TEXT.push_back(std::move(fileContent.value()));
		Lexer lexer(filepath.lexeme, TEXT.back());
#else
		Lexer lexer(filepath.lexeme, *fileContent);
#endif
		std::vector<Token> tokens;
		try
		{
			tokens = std::move(lexer.tokenize());
		}
		catch (const std::exception& e)
		{
			// lexing error
			ErrorReporter::report(e);
			return nullptr;
		}

		Parser parser(std::move(tokens));
		std::vector<StmtPtr> stmts = parser.parse();
		if (ErrorReporter::errorCount != 0)
		{
			// parsing error
			return nullptr;
		}

		// 这里包起来主要是为了让Resolver的scopes层级+1，以符合import的语境
		std::shared_ptr<BlockStmt> blockStmt = std::make_shared<BlockStmt>(std::move(stmts));
		Resolver resolver;
		resolver.resolve(blockStmt.get());
		if (ErrorReporter::errorCount != 0)
		{
			// resolving error
			return nullptr;
		}

		// backup current context
		ContextPtr context_bak = context, global_bak = globalContext;
		// create new module global context
		ContextPtr moduleEnv = std::make_shared<Context>(presetContext);
		moduleEnv->set("__name__", Object(filepath.lexeme));

		globalContext = moduleEnv;
		context = globalContext;

		Finally task([&]()
			{
				globalContext.swap(global_bak);
				context.swap(context_bak); });

		interpret(blockStmt->statements);

		moduleEnv->variables.erase("__name__");

		return std::make_shared<Module>(moduleEnv->variables);
	}

	std::unique_ptr<Finally> Interpreter::toggleRepl()
	{
		if (!replEcho)
			return nullptr;

		replEcho = false;

		return std::make_unique<Finally>([&]()
			{ replEcho = true; });
	}

	ContextPtr Interpreter::currContext()
	{
		return context;
	}

	ContextPtr Interpreter::globalEnv()
	{
		return globalContext;
	}

	Object Interpreter::getReturn()
	{
		Object value = *m_returns;
		m_returns.reset();
		return value;
	}

}
