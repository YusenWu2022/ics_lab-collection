/* 
 * CS:APP Data Lab 
 * 
 * <Please put your name and userid here>
 * Yusen Wu 2000013137
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */

#endif
/* Copyright (C) 1991-2018 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */
/* This header is separate from features.h so that the compiler can
   include it implicitly at the start of every compilation.  It must
   not itself include <features.h> or any other header that includes
   <features.h> because the implicit include comes before any feature
   test macros that may be defined in a source file before it first
   explicitly includes a system header.  GCC knows the name of this
   header in order to preinclude it.  */
/* glibc's intent is to support the IEC 559 math functionality, real
   and complex.  If the GCC (4.9 and later) predefined macros
   specifying compiler intent are available, use them to determine
   whether the overall intent is to support these features; otherwise,
   presume an older compiler has intent to support these features and
   define these macros by default.  */
/* wchar_t uses Unicode 10.0.0.  Version 10.0 of the Unicode Standard is
   synchronized with ISO/IEC 10646:2017, fifth edition, plus
   the following additions from Amendment 1 to the fifth edition:
   - 56 emoji characters
   - 285 hentaigana
   - 3 additional Zanabazar Square characters */
/* We do not support C11 <threads.h>.  */
/* 
 * bitNot - ~x without using ~
 *   Example: bitNot(0) = 0xffffffff
 *   Legal ops: ! & ^ | + << >>
 *   Max ops: 12
 *   Rating: 1
 */
int bitNot(int x)
{
  int mask = (1<<31)>>31;
  return (mask ^ x);
}
/* 
 * bitXor - x^y using only ~ and & 
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y)
{
  return ~(~x & ~y) & ~(x & y);
}
/* 
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x)
{
  int mask = 0xAA + (0xAA << 8);
  mask =mask+ (mask << 16);
  return !((mask & x) ^ mask);
}
/* 
 * rotateRight - Rotate x to the right by n
 *   Can assume that 0 <= n <= 31
 *   Examples: rotateRight(0x87654321,4) = 0x18765432
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 25
 *   Rating: 3 
 */
int rotateRight(int x, int n)
{
int help = (~n + 1);
  return ((((((1 << n)+~1+1) & x) << 1) << (31 + help)) | ((x >> n) & ((2 << (31 + help))+~1+1)));
}
/*  int help = (~n + 1);
  int a = (((1 << n)+~1+1) & x);
  a = (a << 1);
  a = (a << (31 + help));
  int b = (1 << 1);
  b = (b << (31 + help));
  int c = ((x >> n) & (b-1));
  return (a | c);
 * palindrome - return 1 if x is palindrome in binary form,
 *   return 0 otherwise
 *   A number is palindrome if it is the same when reversed
 *   YOU MAY USE BIG CONST IN THIS PROBLEM, LIKE 0xFFFF0000
 *   YOU MAY USE BIG CONST IN THIS PROBLEM, LIKE 0xFFFF0000
 *   YOU MAY USE BIG CONST IN THIS PROBLEM, LIKE 0xFFFF0000
 *   Example: palindrome(0xff0000ff) = 1,
 *            palindrome(0xff00ff00) = 0
 *   Legal ops: ~ ! | ^ & << >> +
 *   Max ops: 40
 *   Rating: 4

 */
int palindrome(int x)
{
    int n = (x & 0x0000ffff);
    int m = (x & 0xffff0000);
    
    n = ((n & 0xff00ff00) >> 8) | ((n & 0x00ff00ff) << 8);
   
    n = ((n & 0xf0f0f0f0) >> 4) | ((n & 0x0f0f0f0f) << 4);
    
    n = ((n & 0xcccccccc) >> 2) | ((n & 0x33333333) << 2);
   
    n = ((n & 0xaaaaaaaa) >> 1) | ((n & 0x55555555) << 1);
    
    n = n << 16;
    
    return !(n ^ m);
}
/*
 * countConsecutive1 - return the number of consecutive 1‘s
 *   in the binary form of x
 *   Examples: countConsecutive1(0xff00ff00) = 2,
 *             countConsecutive1(0xff10ff10) = 4,
 *             countConsecutive1(0xff80ff80) = 2,
 *             countConsecutive1(0b00101010001111110101110101110101) = 10
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 50
 *   Rating: 4
 */
int countConsecutive1(int x)
{
  int help = (x ^ (x << 1));
  int m_1;
  int m_2;
  int m_4;
  int m_8;
  int m_16;
  int a;
  int b;
  int c;
  int d;
  int e;
  int f;
  m_1 = (0x55) + (0x55 << 8);
  m_1 = m_1 + (m_1 << 16);
  m_2 = (0x33) + (0x33 << 8) ;
  m_2 = m_2 + (m_2 << 16);
  m_4 = (0x0f) + (0x0f << 8);
  m_4 = m_4 + (m_4 << 16);
  m_8 = 0xff + (0xff << 16);
  m_16 = 0xff + (0xff << 8);
  b = (help & m_1) + ((help >> 1) & m_1);
  c = (b & m_2) + ((b >> 2) & m_2);
  d = (c & m_4) + ((c >> 4) & m_4);
  e = (d & m_8) + ((d >> 8) & m_8);
  f = (e & m_16) + ((e >> 16) & m_16);
  return (f + 1) >> 1;
}
/* 
 * counter1To5 - return 1 + x if x < 5, return 1 otherwise, we ensure that 1<=x<=5
 *   Examples counter1To5(2) = 3,  counter1To5(5) = 1
 *   Legal ops: ~ & ! | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int counter1To5(int x)
{
  int help = ~(!(6 + ~x)) + 1;
  return x + 1 + (help << 2) + help;
}
/*
 * fullSub - 4-bits sub using bit-wise operations only.
 *   (0 <= x, y < 16) 
 *   Example: fullSub(12, 7) = 0x5,
 *            fullSub(7, 8) = 0xf,
 *   Legal ops: ~ | ^ & << >>
 *   Max ops: 35
 *   Rating: 2
 */
int fullSub(int x, int y)
{
  int x_y;
  int _xy;
  int a;
  int e;
  int f;
  int c;
  int b;
  y = ~y;
  x_y = x & y;
  _xy = x ^ y;
  e = _xy;
  f = x_y << 1;
  x_y = e & f;
  _xy = e ^ f;
  e = _xy;
  f = x_y << 1;
  x_y = e & f;
  _xy = e ^ f;
  e = _xy;
  f = x_y << 1;
  x_y = e & f;
  _xy = e ^ f;
  e = _xy;
  f = x_y << 1;
  y = _xy;
  a = y & 1;
  y = y ^ 1;
  a = a << 1;
  b = y & a;
  y = y ^ a;
  b = b << 1;
  c = y & b;
  y = y ^ b;
  c = c << 1;
  y = y ^ c;
  return y & 0xf;
}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y)
{
  int negX=~x+1;
  int addX=negX+y;
  int check = addX>>31&1;
  int leftBit = 1<<31;
  int xLeft = x&leftBit;
  int yLeft = y&leftBit;
  int Xor = xLeft ^ yLeft;
  Xor = (Xor>>31)&1;
  return ((!Xor)&(!check))|(Xor&(xLeft>>31));
}
/* 
 * sm2tc - Convert from sign-magnitude to two's complement
 *   where the MSB is the sign bit
 *   Example: sm2tc(0x80000005) = -5.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 4
 */
int sm2tc(int x)
{
  int m = x >> 31;
  int neg_x = ~x + 1;
  return (~m & x) | (((x & (1<<31))+neg_x) & m);
}
/*
 * satAdd - adds two numbers but when positive overflow occurs, returns
 *          maximum possible value, and when negative overflow occurs,
 *          it returns minimum positive value.
 *   Examples: satAdd(0x40000000,0x40000000) = 0x7fffffff
 *             satAdd(0x80000000,0xffffffff) = 0x80000000
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 30
 *   Rating: 4
 */
int satAdd(int x, int y)
{
  int mask1 = ~(x ^ y) >> 31;
  int mask2 = ((x + y) ^ y) >> 31;
  int totalmask = (mask1 & mask2);
  int pos = (x >> 31);
  int help = ((~0) ^ (1 << 31));
  return ((~(totalmask)) & (x + y)) + (totalmask & (((1 << 31) & pos) + (help & ~pos)));
}
/*
 * trueFiveEighths - multiplies by 5/8 rounding toward 0,
 *  avoiding errors due to overflow
 *  Examples: trueFiveEighths(11) = 6
 *            trueFiveEighths(-9) = -5
 *            trueFiveEighths(0x30000000) = 0x1E000000 (no overflow)
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 25
 *  Rating: 4
 */
int trueFiveEighths(int x)
{
  int y = x & 0x7;
x = x >> 3;
return (x << 2) + x + ((y + y + y + y + y + ((x >> 31) & 0x7)) >> 3);

}
/* 
 * float_twice - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_twice(unsigned uf)
{
  int exp = (uf&0x7f800000)>>23;
  int sign = uf&(1<<31);
  if(exp==0) return uf<<1|sign;
  if(exp==255) return uf;
  exp++;
  if(exp==255) return 0x7f800000|sign;
  return (exp<<23)|(uf&0x807fffff);
}
/* 
 * float_half - Return bit-level equivalent of expression 0.5*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4

int exp = (uf & 0x7f800000) >> 23;
  int signal = uf & (1 << 31);
  int frac = uf & 0x007fffff;
  if (!exp && !frac)
    return uf;
if(exp==0&&frac==1)
return signal;
  if (!exp)
    return uf >> 1 | signal;
  if (exp == 0xff)
    return uf;
if(exp==1)
   return ((uf-signal)>>1)|signal;
  exp = exp - 1;
  return (exp << 23) | (uf & 0x807fffff);


 */
unsigned float_half(unsigned uf)
{
 int exp = (uf & 0x7f800000) >> 23;
  int signal = uf & (1 << 31);
  int frac = uf & 0x007fffff;
  int tmp;
  if ((frac & 2) && (frac & 1 ))
    tmp = (frac >> 1) + 1;
  else
    tmp = frac >> 1;
  if (exp == 0)
    return signal | tmp;
  else if (exp == 0xff)
    return uf;
  else
    exp = exp - 1;
  if (exp == 0)
    return signal | (tmp + (1 << 22));
  return signal | (exp << 23) | frac;
}
/* 
 * float_f2i - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int float_f2i(unsigned uf)
{
  int s    = uf>>31;
  int exp  = ((uf&0x7f800000)>>23)-127;
  int frac = (uf&0x007fffff)|0x00800000;
  if(!(uf&0x7fffffff)) return 0;

  if(exp > 31) return 0x80000000;
  if(exp < 0) return 0;

  if(exp > 23) frac <<= (exp-23);
  else frac >>= (23-exp);

  if(!((frac>>31)^s)) return frac;
  else if(frac>>31) return 0x80000000;
  else return ~frac+1;
}
/* 
 * float_pwr2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 * 
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while 
 *   Max ops: 30 
 *   Rating: 4
 */
unsigned float_pwr2(int x)
{
  if (x < -149)
    return 0; 
  if (x < -126)
    return (0x1 << (x + 149)); 
  if (x < 128)
    return (x + 127) << 23; 
  return 0xff << 23;        
}
