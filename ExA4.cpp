#include<iostream>
#include<fstream>
#include<string>
#include<cmath>
#include "Golomb.cpp"

int main(void){
    Golomb gl;
    for( int m = 1 ; m < 6 ; m ++){
        cout << "M ----> " << m << endl;
        for( int i = -5 ; i<5 ; i++){
            cout << "S ----> " << i << endl;
            string res = gl.encoder(m, i);
            cout << res << endl;
            cout << gl.decoder(m, res) << endl;
        }
    }
    return 0;
}