var thread = require('./build/release/main.node');

function time() {
	return process.hrtime();
}

function printTime(str, data) {
	console.log(str + ': [' + time() + ' ]' + ((data) ? ', data: [ ' + JSON.stringify(data) + ' ]' : ''));
}


printTime('pre');

thread.addListener(function(data) {
	printTime('inside', data);
});

printTime('post');