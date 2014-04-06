var thread = require('./build/release/main.node');

function time() { return process.hrtime(); }
function printTime(str) { console.log(str + ': [' + time() + ' ]'); }


printTime('pre');

thread.addListener(function() {
  printTime('inside');
});

printTime('post');
