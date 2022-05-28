/*
 * ATTENTION: The "eval" devtool has been used (maybe by default in mode: "development").
 * This devtool is neither made for production nor for readable output files.
 * It uses "eval()" calls to create a separate source file in the browser devtools.
 * If you are trying to read the output file, select a different devtool (https://webpack.js.org/configuration/devtool/)
 * or disable the default devtool with "devtool: false".
 * If you are looking for production-ready output files, see mode: "production" (https://webpack.js.org/configuration/mode/).
 */
/******/ (() => { // webpackBootstrap
/******/ 	"use strict";
/******/ 	var __webpack_modules__ = ({

/***/ "./src/index.js":
/*!**********************!*\
  !*** ./src/index.js ***!
  \**********************/
/***/ (() => {

eval("\r\n\r\nlet workspace = null;\r\nlet textCode = null;\r\n\r\ndocument.addEventListener(\"DOMContentLoaded\", function () {\r\n\t// 将Blockly工作区插入div\r\n\tworkspace = Blockly.inject('blocklyDiv',\r\n\t\t{\r\n\t\t\ttoolbox: document.getElementById('toolbox-categories'),\r\n\t\t\tmedia: \"media/\"\r\n\t\t});\r\n\r\n\t// 定义部分Utility\r\n\tfunction block2Code(language) {\r\n\t\treturn Blockly[language].workspaceToCode(workspace);\r\n\t}\r\n\r\n\tfunction saveBlock() {\r\n\t\t// 使用var将变量声明于整个函数作用域\r\n\t\t// 否则catch中无法使用\r\n\t\ttry {\r\n\t\t\tvar xml = Blockly.Xml.workspaceToDom(workspace);\r\n\t\t\tvar xml_text = Blockly.Xml.domToText(xml);\r\n\t\t\tchannel.objects.Messenger.receiveXML(xml_text);\r\n\t\t} catch (e) {\r\n\t\t\talert(\"debug mode, see xml in console\");\r\n\t\t\tconsole.log(xml_text);\r\n\t\t}\r\n\t}\r\n\r\n\t// 弹出对话框，接受用户输入XML\r\n\tfunction loadBlock() {\r\n\t\ttry {\r\n\t\t\tlet xml_text = prompt(\"Please enter XML code\", \"\");\r\n\t\t\tif (!xml_text)\r\n\t\t\t\treturn;\r\n\r\n\t\t\tlet xml = Blockly.Xml.textToDom(xml_text);\r\n\t\t\tworkspace.clear();\r\n\t\t\tBlockly.Xml.domToWorkspace(xml, workspace);\r\n\t\t} catch (e) {\r\n\t\t\talert(e);\r\n\t\t}\r\n\t}\r\n\r\n\t// 用于接受Qt传来的文件内容\r\n\tfunction loadXML(xml_text) {\r\n\t\ttry {\r\n\t\t\tif (!xml_text)\r\n\t\t\t\treturn;\r\n\r\n\t\t\tlet xml = Blockly.Xml.textToDom(xml_text);\r\n\t\t\tworkspace.clear();\r\n\t\t\tBlockly.Xml.domToWorkspace(xml, workspace);\r\n\t\t} catch (e) {\r\n\t\t\talert(e);\r\n\t\t}\r\n\t}\r\n\r\n\tfunction clearTrashCan() {\r\n\t\tworkspace.trashcan.emptyContents();\r\n\t}\r\n\r\n\t// 为方便测试，定义几个hidden button\r\n\tconst lang = 'Lox';\r\n\tconst buttonConvert = document.getElementById('blocklyConvert');\r\n\tbuttonConvert.addEventListener('click', function () {\r\n\t\talert(\"Check the console for the generated output.\");\r\n\t\ttextCode = block2Code(lang);\r\n\t\tconsole.log(textCode);\r\n\t})\r\n\r\n\tconst buttonSave = document.getElementById('blocklySerialize');\r\n\tbuttonSave.addEventListener('click', saveBlock)\r\n\r\n\tconst buttonLoad = document.getElementById('blocklyDeserialize');\r\n\tbuttonLoad.addEventListener('click', loadBlock)\r\n\r\n\tconst buttonClearTrash = document.getElementById('blocklyClearTrash');\r\n\tbuttonClearTrash.addEventListener('click', clearTrashCan)\r\n\r\n\t// 初始化QWebChannel（全局）\r\n\t// delete try block when move to Qt\r\n\ttry {\r\n\t\twindow.channel = new QWebChannel(qt.webChannelTransport,\r\n\t\t\tfunction (channel) {\r\n\t\t\t\t// you could connect signal and slot here\r\n\t\t\t\tlet messenger = channel.objects.Messenger;\r\n\t\t\t\tmessenger.triggerSave.connect(saveBlock);\r\n\t\t\t\tmessenger.triggerLoadInput.connect(loadBlock);\r\n\t\t\t\tmessenger.triggerLoadFile.connect(loadXML);\r\n\t\t\t\tmessenger.clearTrash.connect(clearTrashCan);\r\n\t\t\t});\r\n\t}\r\n\tcatch (e) { }\r\n\r\n\tworkspace.addChangeListener((event) => {\r\n\t\tif (event.type == Blockly.Events.BLOCK_MOVE || event.type == Blockly.Events.BLOCK_CHANGE) {\r\n\t\t\tlet currentCode = block2Code(lang);\r\n\t\t\tif (currentCode != textCode) {\r\n\t\t\t\ttextCode = currentCode;\r\n\t\t\t\t// send code to Qt\r\n\t\t\t\tchannel.objects.Messenger.receiveCode(textCode);\r\n\t\t\t}\r\n\t\t}\r\n\t});\r\n});\n\n//# sourceURL=webpack://blockly_toolbox/./src/index.js?");

/***/ })

/******/ 	});
/************************************************************************/
/******/ 	
/******/ 	// startup
/******/ 	// Load entry module and return exports
/******/ 	// This entry module can't be inlined because the eval devtool is used.
/******/ 	var __webpack_exports__ = {};
/******/ 	__webpack_modules__["./src/index.js"]();
/******/ 	
/******/ })()
;