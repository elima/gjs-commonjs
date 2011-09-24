function _isFunc (value) {
    return (typeof (value) == "function");
}

function Promise () {
    let listeners = [];
    let value;
    let err;

    this.then = function (callback, errback, progback) {
        if (value != undefined) {
            // Promise has already being fullfilled
            if (_isFunc (callback))
                callback (value);
        }
        else if (err != undefined) {
            // Promise has already failed
            if (_isFunc (errback))
                errback (err);
        }
        else {
            // add as new listener
            let obj = {};
            if (_isFunc (callback))
                obj.cb = callback;
            if (_isFunc (errback))
                obj.eb = errback;
            if (_isFunc (progback))
                obj.pb = progback;

            listeners.push (obj);
        }
    };

    this.fullfill = function (_value) {
        if (value != undefined || err != undefined)
            throw (new Error ("Promise has already been resolved"));

        value = _value;

        for (let i in listeners)
            if (listeners[i].cb)
                listeners[i].cb (value);
    };

    this.fail = function (_err) {
        if (value != undefined || err != undefined)
            throw (new Error ("Promise has already been resolved"));

        err = _err;

        for (let i in listeners)
            if (listeners[i].eb)
                listeners[i].eb (err);
    };
}

Promise.prototype.fullfill = function (value) {
    this._value = value;
};

Promise.prototype.fail = function (error) {
    this._error = error;
};

function Deferred () {
    this.promise = new Promise ();

    let fullfillFunc = this.promise.fullfill;
    this.promise.fullfill = function () {
        throw (new Error ("Not authorized to fullfil this promise"));
    };

    let failFunc = this.promise.fail;
    this.promise.fail = function () {
        throw (new Error ("Not authorized to fail this promise"));
    };

    this.resolve = function (result, isError) {
        if (! isError)
            fullfillFunc.apply (this.promise, [result]);
        else
            failFunc.apply (this.promise, [result]);
    };
}

function defer () {
    return new Deferred ();
};

const JOIN_INDEX_KEY = "org.commonjs.Promise.join._index";

function join (promises) {
    let d = defer ();

    let resolved = 0;
    let total = promises.length;
    let result = [];

    function completed (promise, index, value) {
        result[index] = value;

        if (++resolved == total)
            d.resolve (result, false);
    }

    for (let i=0; i < promises.length; i++) {
        let p = promises[i];
        p[JOIN_INDEX_KEY] = i;

        p.then (function (value) {
                    completed (this,
                               p[JOIN_INDEX_KEY],
                               value);
                },
                function (err) {
                    completed (this,
                               p[JOIN_INDEX_KEY],
                               err);
                });
    }

    return d.promise;
}

if (exports) {
    exports.Promise = Promise;
    exports.Deferred = Deferred;
    exports.defer = defer;
    exports.join = join;
}
