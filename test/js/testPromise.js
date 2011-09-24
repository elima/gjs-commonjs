//
// testPromise.js
//
// This file is part of gjs-commonjs.
//
// Eduardo Lima Mitev <elima@igalia.com>
//

const Assert = require ("common/assert");
const Test = require ("common/test");
const Promise = require ("promise");

function testModuleExports () {
    Assert.ok (Promise.Deferred);
    Assert.ok (Promise.Promise);
    Assert.ok (Promise.defer);
    Assert.ok (Promise.join);
}

function testDefer () {
    // the 'defer' function is defined
    Assert.ok (Promise.defer);
    Assert.equal (typeof (Promise.defer), "function");

    let d = Promise.defer ();

    // defer() returns an object of type Deferred
    Assert.ok (d);
    Assert.equal (typeof (d), "object");
    Assert.strictEqual (d.constructor, Promise.Deferred);

    // a Deferred has a 'promise' object, that is a Promise
    Assert.ok (d.promise);
    Assert.equal (typeof (d.promise), "object");
    Assert.strictEqual (d.promise.constructor, Promise.Promise);

    // a Deferred has a 'resolve' method
    Assert.ok (d.resolve);
    Assert.strictEqual (typeof (d.resolve), "function");

    // resolve() can only be called once
    d.resolve (true, false);
    Assert.throws (function () { d.resolve (true, false); });
}

function testPromise () {
    let p = new Promise.Promise ();
    Assert.equal (typeof (p), "object");
    Assert.strictEqual (p.constructor, Promise.Promise);

    // a Promise has a 'fullfill' method
    Assert.ok (p.fullfill);
    Assert.equal (typeof (p.fullfill), "function");

    // a Promise has a 'fail' method
    Assert.ok (p.fail);
    Assert.equal (typeof (p.fail), "function");

    // a Promise has a 'then' method
    Assert.ok (p.then);
    Assert.strictEqual (typeof (p.then), "function");
}

function testPromiseFullfill () {
    let p = new Promise.Promise ();

    let someValue = [ "someValue" ];
    let callbackCalled = false;

    p.fullfill (someValue);

    // a Promise can only be fullfilled or failed once
    Assert.throws (function () { p.fullfill (someValue); });
    Assert.throws (function () { p.fail (new Error ()); });

    p.then (function (value) {
                Assert.ok (value);
                Assert.strictEqual (value, someValue);
                callbackCalled = true;
            },
            function (error) {
                Assert.fail ("Error callback should not be called here");
            },
            function () {
                Assert.fail ("Progress callback should not be called here");
            });

    Assert.equal (callbackCalled, true);
}

function testPromiseFail () {
    let p = new Promise.Promise ();

    let someError = new Error ();
    let errbackCalled = false;

    p.fail (someError);

    // a Promise can only be fullfilled or failed once
    Assert.throws (function () { p.fullfill ("blah"); });
    Assert.throws (function () { p.fail (someError); });

    p.then (function (value) {
                Assert.fail ("Promise callback should not be called here");
            },
            function (error) {
                Assert.ok (error);
                Assert.strictEqual (error, someError);
                errbackCalled = true;
            },
            function () {
                Assert.fail ("Progress callback should not be called here");
            });

    Assert.equal (errbackCalled, true);
}

function testJoin () {
    // the 'join' function is defined
    Assert.ok (Promise.join);
    Assert.equal (typeof (Promise.join), "function");

    let callbackCalled = false;

    let d1 = Promise.defer ();
    let d2 = Promise.defer ();

    let p = Promise.join ([d1.promise, d2.promise]);

    d2.resolve (2);
    d1.resolve (1);

    p.then (function (value) {
                Assert.ok (value);
                Assert.equal (typeof (value), "object");
                Assert.equal (value.constructor, Array);

                Assert.equal (value[0], 1);
                Assert.equal (value[1], 2);

                callbackCalled = true;
            },
            function (value) {
                Assert.fail ("Error callback should not be called here");
            },
            function () {
                Assert.fail ("Progress callback should not be called here");
            });

    Assert.ok (callbackCalled);
}

Test.run (this);
