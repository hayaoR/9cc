#!/bin/bash
try() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
	gcc -c foo.c

    gcc -o tmp tmp.s foo.o
	
    ./tmp
    actual="$?"

    if [ "$actual" == "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual" 
        exit 1
    fi
}
try 0 "main() {return 0;}" 
try 21 "main() {return 5+20-4;}"
try 47 "main() {return 5+6*7;}"
try 15 "main() {return 5*(9-6);}"
try 4 "main() {return (3+5)/2;}"
try 9 "main() {return -3*-3;}"
try 1 "main() {return 1 < 3;}"
try 0 "main() {return 1 > 3;}"
try 1 "main() {return 1 <= 4;}"
try 4 "main() {return (1 >= 5) + 4 * 1;}"
try 1 "main() {return 1 == 1;}"
try 0 "main() {return 1 != 1;}"
try 3 "main() {a = 5; b=-2; return a+b;}"
try 3 "main() {foo= 5; bar=-2; ans=foo+bar; return ans;}"
try 14 "main() {
	a = 3;
    b = 5 * 6 - 8;
    return a + b / 2;}"
try 4 "main() {
	i = 1; num = 3; if (i == 1) 
						num = 4;
						return num;
						}"
try 3 "main() {
	i = 0; num = 3; if (i == 1) 
						num = 4;
						return num;
						}"
try 5 "main() {
	i = 0; num = 3; if (i == 1) 
						num = 4;
						else
						num=5;
						return num;
						}"
try 3 "main() {i = 1; n=3; while (i < n)
							i = i+1;
							return i;
							}"
try 100 "main() {i = 1; n=100; while (i < n)
							i = i+1;
							return i;
							}"
try 4 "main() {ans = 0; n=5; for (i=1;i<n;i = i+2)
						ans = ans + i;
						return ans;
						}"
try 6 "main() {
	i = 0; n=4; ans = 0; while (i < n) {
				ans = ans + i;
				i = i + 1;
	}
	return ans;
	}"
try 1 "main() {i = 0; n=4; if (i < n) {
							i = i + 2;
							i = i - 1;
							}
							return i;
							}"
try 1 "main() {foo(); return 1;}"
try 1 "main() {fooo(3, 4); return 1;}"
try 55 "
	main() {m = 1;
	n = 10;
	acc = 0;
  for (i = m; i <= n; i = i + 1)
    acc = acc + i;
  return acc;}"
try 9 "sum() {
		a = 3;
		b = 6;
		return a+b;
		}
	main() {
		return sum();
	}
	"
try 9 "sum(m, n) {
		return m+n;
		}
	main() {
		return sum(3,6);
	}
	"
try 55 "
	sum(m, n) {
  acc = 0;
  for (i = m; i <= n; i = i + 1)
    acc = acc + i;
  return acc;
}

main() {
  return sum(1, 10); 
 }
"

echo Ok