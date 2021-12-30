#include<iostream>
#include<fstream>
#include<string>
#include<cmath>
#include <bitset>
#include <type_traits>
#include<climits>
#include<iostream>
#include<fstream>
#include<vector>  
#include<stdio.h>
#include<string.h>
#include<map>
#include<iterator>

using namespace std;
 
class BitStream {
    public:
        char type;
        int pos = 0;
        int p = -1;
        unsigned char buff = 0;
        unsigned char readBuff = 0;
        fstream fsi;
        fstream fso;
        bitset<8> b;
        string filename;

        BitStream(string fn, char op) {
            filename = fn;
            type = op;

            if (type == 'w')
                fso.open(filename, ios::binary | ios::app);
            else if (type == 'r')
                fsi.open(filename, ios::in | ios::binary);
        }

        void closeFO() {
            fso.close();
        }

        void closeFI() {
            fsi.close();
        }

       int readBit() {
            char bit;

            if (p == -1) {
                fsi.read(reinterpret_cast<char *>(&readBuff), 1);
                p = 7;
            }

            b = bitset<8>(readBuff); 

            bit = b.to_string()[p];
            p--;

            return bit - '0';
       }

       string readNBits(int n) {
           string s;

            for (int i = 0; i < n; i++)
                s.append(std::to_string(readBit()));

            return s;
       }

       void writeBit(int bit) {
            if (bit)
                buff |= (1 << pos); //add to buffer
            pos++;

            if (pos == 8) { //when we have enough bits to write 1 byte (8 bits), write to the file
                fso.write(reinterpret_cast<char*>(&buff), sizeof(buff) * sizeof(char));
                pos = 0;
                buff = 0;
            }
        }

       void writeNBits(int n, int b) {
            int a[8];
            int i;
            int lengthA = 0; //length of a
            int surplus = 0; //numer of extra 0s we'll add at the left side of the binary number

            //turn the given number into binary
            for (i = 0; n > 0; i++) {
                a[i] = n % 2;
                n = n / 2;
            }

            if (b >= lengthA) {
                surplus = b - lengthA;

                for (int j = b; j < 8; j++) 
                    writeBit(0); //write the surplus' 0s
                
                for (int k = lengthA; k > 0; k--) 
                    writeBit(a[k]); //write the binary number
                
                for (int l = 0; l < surplus; l++) 
                    writeBit(0); //write the remaining 0s
            }
       }
};
