//
// testRequire.js
//
// This file is part of gjs-commonjs.
//
// Eduardo Lima Mitev <elima@igalia.com>
//

const Assert = require ("common/assert");
const Test = require ("common/test");

function testRequireIsDefined () {
    // 'require' exists and is a function
    Assert.ok (require);
    Assert.equal (typeof (require), "function");

    // 'require.paths' exists an is an array
    Assert.ok (require["paths"]);
    Assert.equal (typeof (require.paths), "object");
    Assert.equal (require.paths.constructor, Array);

    // 'require.paths' is non-deletable
    delete (require.paths);
    Assert.ok (require.paths);
}

function testRequireModuleOk () {
    let someModule = require ("testModules/someModule");

    // Symbols added to 'exports' are exported
    Assert.ok (someModule.msg);
    Assert.equal (someModule.msg, "Hello, I'm an exported variable");

    // Private symbols are not exported
    Assert.equal (someModule["privateMsg1"], undefined);
    Assert.equal (someModule["privateMsg2"], undefined);
}

function testRequireModuleNonExistent () {
    // Requiring a nonexistent module throws an error
    Assert.throws (function () { let someModule = require ("thisModuleDoesntExist"); });
}

Test.run (this);
