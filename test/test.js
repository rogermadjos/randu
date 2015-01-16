var expect = require('chai').expect;
var _ = require('lodash');
var random = require('../main');

describe('Random', function() {
	describe('random', function() {
		it('should generate random values greater than or equal to zero but less than 1', function() {
			var num = 1000;
			var values = {};
			var index;
			for(index = 0; index < num; index++) {
				var rand = random();
				if(rand < 0 || rand >= 1) break;
				if(values[rand+''] >= 1) break;
				values[rand+''] = 1;
			}
			expect(index).to.equal(num);
		});

		it('should generate random integer values', function() {
			var num = 1000;
			var values = {};
			var index;
			for(index = 0; index < num; index++) {
				var rand = random.randInt();
				if(rand !== Math.floor(rand)) break;
				if(values[rand+''] >= 1) break;
				values[rand+''] = 1;
			}
			expect(index).to.equal(num);
		});

		it('should generate random values greater than or equal to zero but less than max value', function() {
			var num = 1000;
			var values = {};
			var max = 100;
			var index;
			for(index = 0; index < num; index++) {
				var rand = random(max);
				if(rand < 0 || rand >= max) break;
				if(values[rand+''] >= 1) break;
				values[rand+''] = 1;
			}
			expect(index).to.equal(num);
		});

		it('should generate random integer values greater than or equal to zero but less than max value', function() {
			var num = 100;
			var values = {};
			var max = 1000000;
			var index;
			for(index = 0; index < num; index++) {
				var rand = random.randInt(max);
				if(rand !== Math.floor(rand)) break;
				if(rand < 0 || rand >= max) break;
				if(values[rand+''] >= 1) break;
				values[rand+''] = 1;
			}
			expect(index).to.equal(num);
		});

		it('should generate random values greater than or equal to min value but less than max value', function() {
			var num = 1000;
			var values = {};
			var min = 50;
			var max = 100;
			var index;
			for(index = 0; index < num; index++) {
				var rand = random(min, max);
				if(rand < min || rand >= max) break;
				if(values[rand+''] >= 1) break;
				values[rand+''] = 1;
			}
			expect(index).to.equal(num);
		});

		it('should generate random integer values greater than or equal to min value but less than max value', function() {
			var num = 100;
			var values = {};
			var min = 50000;
			var max = 1000000;
			var index;
			for(index = 0; index < num; index++) {
				var rand = random.randInt(min, max);
				if(rand !== Math.floor(rand)) break;
				if(rand < min || rand >= max) break;
				if(values[rand+''] >= 1) break;
				values[rand+''] = 1;
			}
			expect(index).to.equal(num);
		});

		it('should select random index based on weights', function() {
			var num = 100000;
			var weights = [20, 20 ,10, 50];
			var distribution = [0, 0, 0, 0];
			_.times(num, function() {
				var index = random.selectRandomIndex(weights);
				distribution[index]++;
			});
			var prob = _.map(distribution, function(val) {
				return Math.round((val / num) * 100);
			});
			expect(prob).to.deep.equal(weights);
		});

		it('should randomly shuffle an array', function() {
			var range = _.range(20);
			var arr = random.shuffle(range);
			expect(range.length).to.equal(arr.length);
			expect(_.isEqual(range, arr)).to.be.false();
		});
	});
});