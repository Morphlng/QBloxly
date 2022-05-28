/*
 * ATTENTION: The "eval" devtool has been used (maybe by default in mode: "development").
 * This devtool is neither made for production nor for readable output files.
 * It uses "eval()" calls to create a separate source file in the browser devtools.
 * If you are trying to read the output file, select a different devtool (https://webpack.js.org/configuration/devtool/)
 * or disable the default devtool with "devtool: false".
 * If you are looking for production-ready output files, see mode: "production" (https://webpack.js.org/configuration/mode/).
 */
/******/ (() => { // webpackBootstrap
/******/ 	var __webpack_modules__ = ({

/***/ "./src/toolbox_label.js":
/*!******************************!*\
  !*** ./src/toolbox_label.js ***!
  \******************************/
/***/ (() => {

eval("/**\n * @license\n * Copyright 2020 Google LLC\n * SPDX-License-Identifier: Apache-2.0\n */\n\n/**\n * @fileoverview The toolbox label built during the custom toolbox codelab, in es6.\n * @author aschmiedt@google.com (Abby Schmiedt)\n */\n\n// import * as Blockly from 'blockly';\n\nclass ToolboxLabel extends Blockly.ToolboxItem {\n\t/**\n\t * Constructor for a label in the toolbox.\n\t * @param {!Blockly.utils.toolbox.ToolboxItemInfo} toolboxItemDef The toolbox\n\t *    item definition. This comes directly from the toolbox definition.\n\t * @param {!Blockly.IToolbox} parentToolbox The toolbox that holds this\n\t *    toolbox item.\n\t * @override\n\t */\n\tconstructor(toolboxItemDef, parentToolbox) {\n\t\tsuper(toolboxItemDef, parentToolbox);\n\t\t/**\n\t\t * The button element.\n\t\t * @type {?HTMLLabelElement}\n\t\t */\n\t\tthis.label = null;\n\t}\n\n\t/**\n\t * Init method for the label.\n\t * @override\n\t */\n\tinit() {\n\t\t// Create the label.\n\t\tthis.label = document.createElement('label');\n\t\t// Set the name.\n\t\tthis.label.textContent = this.toolboxItemDef_['name'];\n\t\t// Set the color.\n\t\tthis.label.style.color = this.toolboxItemDef_['colour'];\n\t\t// Any attributes that begin with css- will get added to a cssconfig.\n\t\tconst cssConfig = this.toolboxItemDef_['cssconfig'];\n\t\t// Add the class.\n\t\tif (cssConfig) {\n\t\t\tthis.label.classList.add(cssConfig['label']);\n\t\t}\n\t}\n\n\t/**\n\t * Gets the div for the toolbox item.\n\t * @return {HTMLLabelElement} The label element.\n\t * @override\n\t */\n\tgetDiv() {\n\t\treturn this.label;\n\t}\n}\n\nBlockly.registry.register(\n\tBlockly.registry.Type.TOOLBOX_ITEM,\n\t'toolboxlabel',\n\tToolboxLabel);\n\n\n//# sourceURL=webpack://blockly_toolbox/./src/toolbox_label.js?");

/***/ })

/******/ 	});
/************************************************************************/
/******/ 	
/******/ 	// startup
/******/ 	// Load entry module and return exports
/******/ 	// This entry module can't be inlined because the eval devtool is used.
/******/ 	var __webpack_exports__ = {};
/******/ 	__webpack_modules__["./src/toolbox_label.js"]();
/******/ 	
/******/ })()
;