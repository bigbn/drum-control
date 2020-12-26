/**
 * @file index.js
 * @project Web-panel
 * @author Pavel Shabardin (<bigbn@mail.ru>) Saturday, 26th December 2020 12:00:33 pm
 * @copyright 2015 - 2020 SKAT LLC, Delive LLC
 */
const gypBuild = require('node-gyp-build')
const bindings = gypBuild(__dirname)

console.log(bindings.init())