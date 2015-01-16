'use strict';

var mtrand = require('./build/Release/mtrand');
var _ = require('lodash');

var exports = function(min, max) {
	if(!min)
		return mtrand.randF();
	if(!max)
		return mtrand.randF(min);
	return mtrand.randF(min, max);
};

exports.rand = exports;

exports.randInt = function(min, max) {
	if(!min)
		return mtrand.rand();
	if(!max)
		return mtrand.rand(min);
	return mtrand.rand(min, max);
}

exports.randString = function(len, charset) {
	charset = charset || 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
	var randomString = '';
	_.times(len, function() {
		var index = exports.randInt(charset.length);
		randomString += charset.substring(index, index + 1);
	});
	return randomString;
}

exports.selectRandomIndex = function(weights) {
	if(!_.isArray(weights) || weights.length === 0) {
		return -1;
	}
	var total = _.reduce(weights, function(accum, val) {
		if(val < 0) throw new Error('Weight cannot be less than 0');
		return accum + val;
	}, 0);
	if(total === 0) {
		var weight = 1 / weights.length;
		_.times(weights.length, function(index) {
			weights[index] = weight;
		});
	}
	var rand = exports(total);
	var low = 0;
	var index;
	var accum = 0;
	for(index = 0; index < weights.length; index++) {
		if(index === 0 && rand < weights[0])
			break;
		if(index === 0)
			continue;
		accum += weights[index - 1];
		if(rand >= accum && rand < accum + weights[index]) {
			break;
		}
	}
	return index;
}

exports.shuffle = function(input) {
	var arr = input.slice(0);
	for(var i=0;i<arr.length;i++) {
		var j = exports.randInt(arr.length);
		var temp = arr[i];
		arr[i] = arr[j];
		arr[j] = temp;
	}
	return arr;
}

module.exports = exports;