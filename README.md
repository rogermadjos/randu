# randu

[![Build Status](https://travis-ci.org/rogermadjos/randu.svg?branch=master)](https://travis-ci.org/rogermadjos/randu)
[![npm version](https://badge.fury.io/js/randu.svg)](http://badge.fury.io/js/randu)

## Randomization utility built around the Mersenne Twister Pseudo-Random Number Generator.

## How to install

```
npm install node-mutex
```

## API
```
var randu = require('randu');
```
This will import `randu` into your project


```
randu();
```
Generates a random floating point number in the range `[0, 1)`.


```
randu(max);
```
Generates a random floating point number in the range `[0, max)`.


```
randu(min, max);
```
Generates a random floating point number in the range `[min, max)`.


```
randu.randInt();
```
Generates a random integer in the range `[0, 4294967296)`.


```
randu(max);
```
Generates a random integer in the range `[0, max)`.


```
randu(min, max);
```
Generates a random integer in the range `[min, max)`.


```
randu.randString(len, charset);
```
Generates a random string with the length `len`. The result contains only the characters found in `charset`. The default value of `charset` is 
`ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789`


```
randu.selectRandomIndex(weights);
```
Generates a random integer based on `weights`. e.g. If `weights = [1, 2, 1]`, `0` has `0.25` probability of being selected, `1` has `0.5` and so on.


```
randu.shuffle(input)
```
Generates a random permutation of the array `input`. `input` is immutable.

## License

MIT