/* Creator: Tegh Bir Singh,
*Date: 3/10/18
*
*The intended purpose of this program is determine whether z and e are coprime by finding the greatest common divisor
* and the value of d as such d is the multiplicative modulo inverse such that ed mod z =1.
*
*
*Corrolarly: If there are two integers a and b then there are two different integers such that a = bq+r where r is the remainder
* such and q is the quotient as q>r.
*
*
*
*Proof:
*
* variables for proof( b_i, q_i where i>= 1)
* a = (b_1)(q_1)+ b_2
* => b_1 = (b_2)(q_2)+ b_3
* => b_2 = (b_3)(q_3)+ b_4
* ...
* => b_n = (b_(n+1))(q_(n+1)) as the value of remainder continues to decrease to zero
*
*Through the substitution of b_n it can seen b_n is the GCD of a and b and therfore GCD(a, b)= GCD(b, a%b)
*/

#include <stdio.h>

   int coprime_checker(int z, int e) /* Use the fact the GCD of two coprime numbers is 1*/

{
    printf("Please enter the values of z and e separated by a space");
    scanf("%d %d", z, e);
    int c;
    do
    {

    if (e > z)
    {
        c = e;
        e = z;
        z = c;
    }
    else
        z = z-e;
    }
    while (z > e);

    return e;
}

    int modular_inverse(z, e)
{
    int d;
    d=2;

    while ((e*d)%z != 1) /* The Extended Euclidean algorithm is better method.  */
    {
        d=d+1;
    }

    printf("The value of d is %d", d);

    return d;
}

int main(void)

{
    int a, b;
    int GCD;
    GCD = coprime_checker(a, b);
    printf("%d", GCD);
    return 0;
}
