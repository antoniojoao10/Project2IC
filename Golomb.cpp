#include<iostream>
#include<fstream>
#include<string>
#include<cmath>
#include <bitset>
#include <type_traits>
#include<climits>
using namespace std;


class Golomb{
    public:

      #define IS_INTEGRAL(T) typename std::enable_if< std::is_integral<T>::value >::type* = 0

      template<class T>

      string integral_to_binary_string(T byte, IS_INTEGRAL(T))
      {
          std::bitset<sizeof(T) * CHAR_BIT> bs(byte);
          return bs.to_string();
      }

      string encoder(int m, int s){
        if(s>=0) s = 2 * abs(s);
        else if(s<0) s = 2 * abs(s) - 1;
        int k =  ceil(log2(m));
        int r = s%m;
        int q = floor(s/m);
        string qo = "";
        for( int i = 0; i< q ; i++){
          qo += "1";
        }
        qo += "0";
        int t = pow(2,k) - m;
        string ro = "";

        if( r < t){
          string s = integral_to_binary_string(abs(r));
          
          for( int i = k-1; i>0 ; i--){
              ro += s[sizeof(s)-i] ;
          }   
        }else{
          r = r + t;
          
          string s = integral_to_binary_string(abs(r));
          
          for( int i = k; i>0 ; i--){
              ro += s[sizeof(s)-i] ;
          }   
        }

        string res = qo + ro;
        return res;
      }

      int decoder(int m, string sin){
        int k =  ceil(log2(m));
        int t = pow(2,k) - m;
        int cnt =sin.length();
        int sz= sin.length();
        int s=0;
        for( int i = sz -1 ; i >=0 ; i--){
          cnt --;
          if (sin[sz -1 -i] == '0' && i == sz -1 ) break;
          if( sin[sz -1 -i] == '1' && i == sz -1 ) s+=1;
          else if(sin[sz -1 -i]=='1' && sin[sz -1 -i -1] =='1') s+=1;
          else if(sin[sz -1 -i] =='0' && s!=0) break;
          else if(sin[sz -1 -i] =='1' && sin[sz -1 -i -1] == '0') s+=1;
        }
        int scnt = k -1;
        string x1 = "";
        for( int i = cnt -1; i>=0 ; i--){
          cnt=i;
          if(scnt ==0) break;
          x1 += sin[sz-1-i];
          scnt--;
        }
        int x = 0;
        for( int i = x1.length()-1 ; i >=0 ; i--) if(x1[i] == '1') x += pow(2,x1.length()-1-i);
        if(x<t){
            s = s * m + x;
        }else{
            x = x * 2;
            if(sin[sz -1 - cnt] == '1') x+=1;
            s = s * m + x - t;
        }
        if(s%2!=0) s = -(s/2) - 1;
        else if(s%2==0) s = s/2 ;
        return s;
      }
        
};

/*int main(int argc, char** argv) {
  Golomb gl;
  string res = gl.encoder(atoi(argv[1]), atoi(argv[2]));
  cout << res << endl;
  cout << gl.decoder(atoi(argv[1]), res) << endl;
  return 0;
}*/