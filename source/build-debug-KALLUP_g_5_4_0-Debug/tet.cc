#include <iostream>
#include <fstream>
 
int main()
{
    std::fstream file;
    file.open("Prim.txt", std::ios::out);
 
    for(int i = 2; i < 300000; i++) //die zu prüfende Primzahl
    {
        bool isPrime = true;
        for(int j = 2; isPrime && j < i; j++) //Schleife damit diese Zahl durch jede geteilt wird
        {
            if(i != j) //damit sie sich nicht durch sich selbst teilt
            {
                isPrime = !!(i % j);
            }
        }
        if (isPrime)
          file << i << " ";
    }
    file.close();
}

