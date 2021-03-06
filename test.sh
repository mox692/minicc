#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./minicc "$input" > tmp.s
  cc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 0 '0;'
assert 42 "42;"
assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'
assert 10 '-10+20;'
assert 10 '((-3)+4)*10;'
assert 10 '((-5)*4+25)*2;'
assert 10 '((-5)*(+4)+25)*(+2);'
assert 15 '-3*(-5);'

assert 0 '0==1;'
assert 1 '42==42;'
assert 1 '0!=1;'
assert 0 '42!=42;'

assert 1 '0<1;'
assert 0 '1<1;'
assert 0 '2<1;'
assert 1 '0<=1;'
assert 1 '1<=1;'
assert 0 '2<=1;'

assert 1 '1>0;'
assert 0 '1>1;'
assert 0 '1>2;'
assert 1 '1>=0;'
assert 1 '1>=1;'
assert 0 '1>=2;'

# multiple statements
assert 3 '0; 1; 3;'
assert 0 '1>=1; 1>=0; 1>=2;'
assert 46 '42==42; 46;'

# add single assignment
assert 3 'a=3; a;'
assert 8 'a=3; z=5; a+z;'
assert 6 'a=b=3; a+b;'

# add multi letter variable 
assert 3 'foo=3; foo;'
assert 8 'foo123=3; bar=5; foo123+bar;'

echo OK