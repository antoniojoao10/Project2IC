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
#include<sndfile.h>

using namespace std;

string convertToString(char* a, int size)
{
    int i;
    string s = "";
    for (i = 0; i < size; i++) {
        s = s + a[i];
    }
    return s;
}

class Golomb{
    public:
      char *una[10] = {
        "0","10","110","1110",
        "11110","111110","1111110",
        "11111110","111111110","1111111110"
      };

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
        string qo = una[q];
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

int main(int argc, char** argv) {
    Golomb gl;
    SNDFILE* out_file;
    ofstream ot("test.txt");
    
    //ifstream ofs (argv[2], ios::in | ios::binary);
    ifstream ofs (argv[2]);

    string buffer;
    int cnt = 0;
    int buffcnt=0;
    vector<int> out;
    int i = 0;
    int diff = 1;
    int case0 = 0;
    int case1 = 0;
    int case2 = 0;
    int case3 = 0;

    while(getline(ofs,buffer)){
        int res = gl.decoder(10000, buffer );
        

        switch (buffcnt){
            case 0:
                case0=res;
                out.push_back(res);
                break;
            case 1:
                case1=res + case0;
                out.push_back(case1);
                break;
            case 2:
                case2 = res + 2*case1 - case0;
                out.push_back( case2 );
                break;
            case 3:
                case3 = res + 3*case2 - 3*case1 + case0;
                out.push_back( case3 );
                break;
            default:
                case0=res;
                out.push_back(res);
                buffcnt=0;
        }
        i++;
        buffcnt++;
    }

        
    for( int i =0 ; i< out.size() ; i++) ot << out.at(i) << endl;

    SF_INFO		sfinfo ;     
    SNDFILE* read_file;
    
    read_file = sf_open("sample01.wav", SFM_READ, &sfinfo);
    short *tmpIn = new short[sfinfo.channels * sfinfo.frames];
    sf_read_short(read_file, tmpIn, sfinfo.channels * sfinfo.frames);
    int frame = sfinfo.frames;
    sf_close(read_file);

    short outArr[out.size()];
    std::copy(out.begin(), out.end(), outArr);
    
    out_file = sf_open(argv[1], SFM_WRITE, &sfinfo);
    sfinfo.frames=out.size();
    sf_write_short(out_file, outArr , 2 * sfinfo.frames);
    sf_close(out_file);
    
    return 0;
}