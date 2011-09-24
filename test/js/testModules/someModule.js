const Assert = require ("common/assert");

// 'exports' exists and is an object
Assert.ok (exports);
Assert.equal (typeof (exports), "object");
Assert.equal (exports.constructor, Object);

// 'exports' is non-deletable
delete (this.exports);
Assert.ok (this.exports);

// 'module' exists and is an object
Assert.ok (module);
Assert.equal (typeof (module), "object");
Assert.equal (module.constructor, Object);

// 'module' is non-deletable
delete (this.module);
Assert.ok (this.module);

// 'module' must have a read-only 'id' property
Assert.ok (module["id"]);
Assert.equal (typeof (module.id), "string");

// module.id is read-only
// @TODO: this case is commented because GJS shows an ugly warning message
//let tmp = module.id;
//module.id = null;
//Assert.equal (module.id, tmp);

// module.id is don't-delete
delete (module.id);
Assert.ok (module.id);

let privateMsg1 = "Hi, I'm a private variable";
this.privateMsg2 = "Hi, I'm also private variable";

exports.msg = "Hello, I'm an exported variable";
