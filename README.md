# QBloxly

A custom Lox interpreter with drag-and-drop Ui interface

## Description

[Cploxplox](https://github.com/Morphlng/cploxplox) is a C++17 implementation of the lovely [Lox](http://www.craftinginterpreters.com/contents.html) language, designed by Robert Nystrom. 

There are quite a few differences between cploxplox and the original lox, such as we use `func` instead of `fun` for function, and the support of several new features, lambda expression for example. For more info about cploxplox itself, please goto the [interpreter repo](https://github.com/Morphlng/cploxplox)

I customized [google/blockly](https://github.com/google/blockly) library to support cploxplox code generator. In this repo, I used Qt to set up a bridge between the `blockly interface` and `cploxplox backend`, so you can adopt this drag-and-drop interaction for cploxplox. There's also an implementation of a `console`, you can open it through the menu and execute text-based code in repl mode.

This is my final project in CUC, thesis about the design and implementation stores in school library, so I and [CUC](https://en.cuc.edu.cn/) has all rights reserved for the idea. Code in the repo is free to share under MIT license.