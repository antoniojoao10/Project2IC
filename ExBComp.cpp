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
#include <map>
#include <iterator>

using namespace std;
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
    SNDFILE* read_file;
    SNDFILE* out_file;

    ofstream ot ("test1.txt");
    ofstream hist("hist.txt");
    
    ofstream ofs (argv[2]);
    //ofstream ofs (argv[2], ios::out | ios::binary);
    
 
    SF_INFO		sfinfo ;     
    read_file = sf_open(argv[1], SFM_READ, &sfinfo);
    short *tmpIn = new short[sfinfo.channels * sfinfo.frames];
    sf_read_short(read_file, tmpIn, sfinfo.channels * sfinfo.frames);
    int channels = sfinfo.channels;
    int frame = sfinfo.frames;
    sf_close(read_file);     
    int total = 0;
    vector<int> re;
    map<int,int> histMap;
    int cnt =0; 
    int diff = 0; 
    int p = 0;
    for(int i  =0; i < frame*channels ; i++){
        switch (cnt){
            case 0:
                break;
            case 1:
                p = tmpIn[i-1];
                break;
            case 2:
                p = 2*tmpIn[i-1] - tmpIn[i-2];
                break;
            case 3:
                p = 3*tmpIn[i-1] - 3*tmpIn[i-2] + tmpIn[i-3];
                break;
            default:
                diff++;
                p=0;
                cnt=0;
        }
        re.push_back(tmpIn[i] - p);

        //histogram
        histMap[tmpIn[i] - p] ++;


        cnt++;
        ot << re.at(i) << endl;

        string res = gl.encoder( 10000 , re.at(i));

        ofs << res << endl;
        /*
        int res = gl.encoder( 10000 , re.at(i)).length();

        // declaring character array
        char arr[sz + 1];
    
        // copying the contents of the
        // string to char array
        strcpy(arr, gl.encoder( 10000 , re.at(i)).c_str());
        ofs.write(arr, 25); //
        */
        
    } 

    map<int, int>::iterator itr;
    for (itr = histMap.begin(); itr != histMap.end(); ++itr) {
        hist << itr->first << "\t ----> \t" << itr->second << endl;
    }
    return 0;
}