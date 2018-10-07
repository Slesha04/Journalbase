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

/*Proof for algorithm(prime number part)
*
* Contradiction
*
* Theorem: Only all the prime numbers less than or equal to the square root number are needed to check if
* the number is a prime number or a composite instead.
*
* Since the product for number consist of two factors let p and q be those factors for number that is
* assumed to be composite number. This implies there three cases for the values of p and q
*
* 1. p < q
* 2. p > q
* 3. p = q (Meaning p and q is equal to the square root of the number)
*
* This means by checking all the numbers less than the square root of the number your also checking all
* other possible factors greater than the square root. However not al numbers between 2 and the square root
* are not needed ro check whether the number is prime not as by dividing by composite numbers is the same
* whether number is divisible by the prime numbers the composite factors into.
*
*Steps for RSA
*
* 1. Find two prime numbers p and q
* 2. Compute value of p*q which equals n
* 3. Compute value of (p-1)*(q-1) = z
* 4. Find a value of e such that z and e are coprime, small values of e are suitable
* 5. K+= (n, e) (This is public key which uses values of n and e for encryption.
* 6. Find value of d such that ed mod z = 1, i.e. d is the multiplicative modulo inverse.
* 7. Use the private key (n, d) for decryption
*
* These steps make up the key generation system.
*  The next step would be ecrypt the journal by substituting values symbols with ASCII values
*  and then applying the formula for encryption.
*/


#include <stdio.h>
#include <math.h>

   int coprime_checker(int z, int e); /* Use the fact the GCD of two coprime numbers is 1*/
   void prime_checker(int a, int b);


int main(void)

{
    int z, e, modulo;
    z=(p-1)*(q-1)
    int GCD;
    GCD = coprime_checker(z, e);
    printf("%d", GCD);
    modulo = modulo_inverse(z, e);
    printf("The value of d is %d", d)
    prime_checker(p, q); /* error a and b undec;ared? */
    if(GCD=1)
        return 0;
    else
        return 1;

}

    coprime_checker(z, e)
{
    printf("Please enter the values of e separated by a space");
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

{
    int square_root;
    int c;
    int divisor;
    square_root = pow(2, 0.5);
    printf("Please enter two integers separated by a space>");
    scanf("%d %d", &p, &q);
    if (q<p)
        c=p;
        p=q;
        q=c;
    while (p<=q && p>1){
        for(divisor=2; divisor<=square_root; ++divisor)
              {
                  if (a%divisor==0)
              {
                 p=p+1;
                 square_root= pow(a, 0.5);
              }
              else
              {
                  printf("%d is a prime number", p);
                  p=p+1;
              }

}
    }

}

