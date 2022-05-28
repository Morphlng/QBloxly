#include "Cxx/Cxx_Core.h"
#include "Cxx/Runner.h"
#include "Cxx_Engine/CxxCompleter.h"
#include <QStringListModel>

CxxCompleter::CxxCompleter() :insert_pos{ 0 }
{
    lox_keywords << "and";
    lox_keywords << "as";
    lox_keywords << "break";
    lox_keywords << "continue";
    lox_keywords << "class";
    lox_keywords << "else";
    lox_keywords << "for";
    lox_keywords << "func";
    lox_keywords << "from";
    lox_keywords << "if";
    lox_keywords << "import";
    lox_keywords << "or";
    lox_keywords << "return";
    lox_keywords << "super";
    lox_keywords << "this";
    lox_keywords << "var";
    lox_keywords << "while";

    lox_keywords << "true";
    lox_keywords << "false";
    lox_keywords << "nil";
}

int CxxCompleter::updateCompletionModel(const QString &code)
{
    this->setModel(0);

    // Don't try to complete the empty string
    if (code.isEmpty()) {
        return 0;
    }

    // Search backward through the string for usable characters
    QString textToComplete;
    for (int i = code.length() - 1; i >= 0; i--) {
        QChar c = code.at(i);
        if (c.isLetterOrNumber() || c == '.' || c == '_') {
            textToComplete.prepend(c);
            insert_pos = i;
        } else {
            break;
        }
    }

    // Split the string at the last dot, if one exists
    QString lookup;
    QString compareText = textToComplete;
    int dot = compareText.lastIndexOf('.');
    if (dot != -1) {
        lookup = compareText.mid(0, dot);
        compareText = compareText.mid(dot + 1);
        insert_pos += (dot + 1);
    }

    // Lookup Interpreter Context names
    QStringList found;
    if (!lookup.isEmpty() || !compareText.isEmpty()) {
        compareText = compareText.toLower();
        QStringList l = introspection(lookup);
        for (QString &n : l) {
            if (n.toLower().startsWith(compareText))
                found << n;
        }
    }

    // Initialize the completion model
    if (!found.isEmpty()) {
        setCompletionMode(QCompleter::PopupCompletion);
        setModel(new QStringListModel(found, this));
        setCaseSensitivity(Qt::CaseInsensitive);
        setCompletionPrefix(compareText.toLower());
    }

    return found.size();
}

QStringList CxxCompleter::introspection(const QString &lookup)
{
    using namespace CXX;

    // list of found tokens
    QStringList properties, functions, children;

    ContextPtr c_env = Runner::interpreter.currContext();
    
    Object obj;
    if (lookup.isEmpty()) {
        properties = lox_keywords;

        // add all defined variable name
        Context* ptr = c_env.get();
        do {
            for (auto& [name, object] : ptr->variables) {
                children << QString::fromStdString(name);
            }

            ptr = ptr->parent.get();
        } while (ptr);

    }
    else {
        obj = c_env->get(lookup.toStdString());
        if (obj.isInstance()) {
            InstancePtr instance = obj.getInstance();
            Class* belonging = instance->belonging.get();

            for (auto& [name, prop] : instance->fields) {
                QString prop_name = QString::fromStdString(name);
                if (prop_name.contains('@'))
                    continue;

                properties << prop_name;
            }

            for (auto& [name, method] : belonging->methods) {
                QString func_name = QString::fromStdString(name);

                if (func_name.startsWith("__") || func_name == "init")
                    continue;
                else
                    functions << func_name;
            }
        }
    }

    children.removeDuplicates();
    children.sort(Qt::CaseInsensitive);

    functions.removeDuplicates();
    functions.sort(Qt::CaseInsensitive);

    properties.removeDuplicates();
    properties.sort(Qt::CaseInsensitive);

    children << properties << functions;

    return children;
}
