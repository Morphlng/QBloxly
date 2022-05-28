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

/***/ "./src/custom_category.js":
/*!********************************!*\
  !*** ./src/custom_category.js ***!
  \********************************/
/***/ (() => {

eval("/**\n * @license\n * Copyright 2020 Google LLC\n * SPDX-License-Identifier: Apache-2.0\n */\n\n/**\n * @fileoverview The toolbox category built during the custom toolbox codelab, in es6.\n * @author aschmiedt@google.com (Abby Schmiedt)\n */\n\n// import * as Blockly from 'blockly';\n\nclass CustomCategory extends Blockly.ToolboxCategory {\n\t/**\n\t * Constructor for a custom category.\n\t * @override\n\t */\n\tconstructor(categoryDef, toolbox, opt_parent) {\n\t\tsuper(categoryDef, toolbox, opt_parent);\n\t}\n\n\t/**\n\t * Adds the colour to the toolbox.\n\t * This is called on category creation and whenever the theme changes.\n\t * @override\n\t */\n\taddColourBorder_(colour) {\n\t\tthis.rowDiv_.style.backgroundColor = colour;\n\t}\n\n\t/**\n\t * Sets the style for the category when it is selected or deselected.\n\t * @param {boolean} isSelected True if the category has been selected,\n\t *     false otherwise.\n\t * @override\n\t */\n\tsetSelected(isSelected) {\n\t\t// We do not store the label span on the category, so use getElementsByClassName.\n\t\tvar labelDom = this.rowDiv_.getElementsByClassName('blocklyTreeLabel')[0];\n\t\tif (isSelected) {\n\t\t\t// Change the background color of the div to white.\n\t\t\tthis.rowDiv_.style.backgroundColor = 'white';\n\t\t\t// Set the colour of the text to the colour of the category.\n\t\t\tlabelDom.style.color = this.colour_;\n\t\t\tthis.iconDom_.style.color = this.colour_;\n\t\t} else {\n\t\t\t// Set the background back to the original colour.\n\t\t\tthis.rowDiv_.style.backgroundColor = this.colour_;\n\t\t\t// Set the text back to white.\n\t\t\tlabelDom.style.color = 'white';\n\t\t\tthis.iconDom_.style.color = 'white';\n\t\t}\n\t\t// This is used for accessibility purposes.\n\t\tBlockly.utils.aria.setState(/** @type {!Element} */(this.htmlDiv_),\n\t\t\tBlockly.utils.aria.State.SELECTED, isSelected);\n\t}\n\n\t/**\n\t * Creates the dom used for the icon.\n\t * @return {HTMLElement} The element for the icon.\n\t * @override\n\t */\n\tcreateIconDom_() {\n\t\t// use this.toolBoxItemDef_.name to recognize kind\n\t\tconst iconImg = document.createElement('img');\n\t\ticonImg.src = './logo_only.svg';\n\t\ticonImg.alt = 'Blockly Logo';\n\t\ticonImg.width = '25';\n\t\ticonImg.height = '25';\n\t\treturn iconImg;\n\t}\n}\n\nBlockly.registry.register(\n\tBlockly.registry.Type.TOOLBOX_ITEM,\n\tBlockly.ToolboxCategory.registrationName,\n\tCustomCategory, true);\n\n\n//# sourceURL=webpack://blockly_toolbox/./src/custom_category.js?");

/***/ })

/******/ 	});
/************************************************************************/
/******/ 	
/******/ 	// startup
/******/ 	// Load entry module and return exports
/******/ 	// This entry module can't be inlined because the eval devtool is used.
/******/ 	var __webpack_exports__ = {};
/******/ 	__webpack_modules__["./src/custom_category.js"]();
/******/ 	
/******/ })()
;