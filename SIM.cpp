/* On my honor, I have neither given nor received unauthorized aid on this assignment */

#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <set>
#include <bitset>
#include <cassert>
#include <algorithm>
using namespace std;
struct element{
    int freq;
    int pos;
};
typedef vector<pair<string, element> > vector_rank;
map<string, element> Dictionary;     //  Dictionary
vector<string> file_cont;        
vector<int> no_of_occ; 

map<string, int> alltypes; //compression types
int arr[] = {2,12,8,8,13,3,32};
vector<string> compression;     // compressed code
vector<string> vec_dict;
string d_previous;

bool pri_bit(pair<string, element>& a, pair<string, element>& b){
  
 //        sort by freq (desc)
    if(a.second.freq > b.second.freq){
        return 1;
         //        if freq is equal, give priority to position
    }else if(a.second.freq == b.second.freq){
    	
        if(a.second.pos < b.second.pos)return 1;
        else return 0;
    }else return 0;
}
  
vector_rank map_sort(map<string, element>& M){
    //    Sort a given map 
    
    vector_rank A;

    for (auto& it : M) A.push_back(it);
    sort(A.begin(), A.end(), pri_bit);
  
    return A;
}

void Dict_init(){
  
    map<string, element> frequency;
    map<string, element> it;

    for(int i=0 ;i < file_cont.size(); i++){
        if(frequency.count(file_cont[i]) == 0){
            frequency[file_cont[i]].freq = 1;
            frequency[file_cont[i]].pos  = i+1;
        }else frequency[file_cont[i]].freq += 1;
    }
    
    vector_rank sorted;
    sorted = map_sort(frequency);

    int items = 0;
    for (auto it : sorted) {

        if(items < 8 && it.second.freq > 0){
            Dictionary[it.first].freq = it.second.freq;
            Dictionary[it.first].pos  = items;
            vec_dict.push_back(it.first); 
        }
        items++;
        
    }
}

vector<string> readf(string filename){
 

    ifstream file(filename);
    string str; 

    vector<string> lines;
    string previous="";
    int i = 0;

    while (getline(file, str)){
        lines.push_back(str);

        if(previous == str)no_of_occ.push_back(1); //build similarity map
        else no_of_occ.push_back(0);

        previous = str;
        i++;
    }

    for (int j = no_of_occ.size()-1 ; j > 0; j--){
        no_of_occ[j-1] = (no_of_occ[j] + no_of_occ[j-1])*no_of_occ[j-1]; //summation array
    }

    return lines;
}

pair<int,string> MM_count(string x, string y){
    // return number of mismatch count along with the XORed strings
    int mismatches = 0;
    string m_ = (bitset<32>(x) ^ bitset<32>(y)).to_string();

    for (int i=0; i < m_.size(); i++)mismatches += m_[i] - '0';

    pair<int,string> out;
    out.first  = mismatches;
    out.second = m_; 
    
    return out;
}

pair<int,int> CMM_LOC(string bitmask, int length=1){
   

    vector<int> summ;
    int pos = 0;
    pair<int,int> out;

    int x = bitmask[0] - '0';
    summ.push_back(x);
    

    for(int i=1;i < bitmask.size(); i++){
        int x = bitmask[i] - '0';
        summ.push_back((summ[i-1]+x)*x); //summation array
    }

    for(int i=0;i < summ.size(); i++){
        if(summ[i]==1){ //first mismatch pos detected
            pos = i;
            break;
        }
    }

    int consecutive_mismatches = *max_element(summ.begin(), summ.end());

    out.first  = consecutive_mismatches;
    out.second = pos;

    if(length!=consecutive_mismatches) out.first = -1;
    return out;
}

pair<int,string> bitmaskPossible(string bitmask,int diffCount){
   

    int pos = 0;
    pair<int,string> out;

    for(int i=0;i < bitmask.size(); i++){
        if(bitmask[i]=='1'){    //identify first mismatch
            pos = i;
            break;
        }
    }

    string mask_pattern = bitmask.substr(pos,4); 

    int diff = count(mask_pattern.begin(),mask_pattern.end(),'1');

    out.second = mask_pattern;
    out.first  = pos;

    if(diff != diffCount){
        out.first = -1;
    }
    return out;
}

string DirM(int i){
  
    int count = 0;

    if(Dictionary.count(file_cont[i]) > 0){ //check if exists in Dictionary
        string pattern = bitset<3>(Dictionary[file_cont[i]].pos).to_string();
        return "101" + pattern;
    }else return "";
}

string B2MM_com(int i,int count=2){
   
    for(int it = 0; it < vec_dict.size(); it++){   //loop Dictionary

        pair<int,string> Mismatch_count  = MM_count(vec_dict[it],file_cont[i]);
        if(Mismatch_count.first == count){

            vector<size_t> positions; 

            size_t pos = Mismatch_count.second.find("1", 0);
            while(pos != string::npos){
                positions.push_back(pos);
                pos = Mismatch_count.second.find("1",pos+1);
            }

            string ml1  = bitset<5>(positions[0]).to_string();
            string ml2  = bitset<5>(positions[1]).to_string();
            string dict = bitset<3>(it).to_string();
            return "100" + ml1 + ml2 + dict;
        
        }
    }
    return "";
}

string MM_com(int i,int count, string type){
   
    
    for(int it = 0; it < vec_dict.size(); it++){   //loop Dictionary

        pair<int,string> Mismatch_count  = MM_count(vec_dict[it],file_cont[i]);
        if(Mismatch_count.first==count){
            pair<int,int> cmmC = CMM_LOC(Mismatch_count.second,count);

            if(cmmC.first == count){
                string dict     = bitset<3>(it).to_string();
                string mismatch = bitset<5>(cmmC.second).to_string();
                return type  + mismatch + dict;
            }
        }
    }
    return "";
}

string Bit_com(int i){
   
    for(int it = 0; it < vec_dict.size(); it++){   

        pair<int,string> Mismatch_count  = MM_count(vec_dict[it],file_cont[i]);

        if(Mismatch_count.first <= 4){
            pair<int,string> bitmask = bitmaskPossible(Mismatch_count.second, Mismatch_count.first);

            if(bitmask.first >= 0){ 
                string start_loc = bitset<5>(bitmask.first).to_string();
                string mask   = bitset<4>(bitmask.second).to_string();
                string dict   = bitset<3>(it).to_string();
                return "001" + start_loc + mask + dict;
            }
        }
    }
    return "";
}

string compressLine(int i){
  
    string com;

    com = DirM(i);    
    if(com.size() > 0) return com;
    
   com = MM_com(i,1,"010"); 
    if(com.size() > 0) return com;

    com = MM_com(i,2,"011"); 
    if(com.size() > 0) return com;

    com = Bit_com(i);           
    if(com.size() > 0) return com;

    com = B2MM_com(i);     
    if(com.size() > 0) return com;

    return "110" + file_cont[i];  
}

void writeToFile(string filename,string text){
    ofstream myfile;
    myfile.open (filename);

    int i = 0;
    while(i  <  text.size()){
        string line = text.substr(i,32);
        line.insert(line.end(), 32 - line.size(), '1');
        myfile << line<<endl;
        i+=32;
    }
    myfile.close();
}

string compress(){
    file_cont = readf("original.txt");
    Dict_init();

    int i = 0;

    string out="";

    while(i < file_cont.size()){
       

        if( no_of_occ[i]!=0 ){
            
            int skip = min(5,no_of_occ[i]);
            int rem  = no_of_occ[i] - skip;
            
            string x = "000" + bitset<2>(skip-1).to_string();
            out+=x;
           

            i = i + skip;
           
            if(rem > 0){ no_of_occ[i] = 0;}
            

        }else if(compressLine(i).size() > 0){
            string x = compressLine(i);
            out+=x;
            
            i++;

        }
        
    }
    writeToFile("cout.txt",out);
    
    //write Dictionary to the same file
        ofstream outfile;
        outfile.open("cout.txt", ios_base::app);
        outfile << "xxxx\n"; 
        for(int k=0; k < vec_dict.size(); k++){
            outfile<<vec_dict[k]<<"\n";
        }

    return out;
}

void d_initialize(){
    ifstream file("compressed.txt");
    string all="";
    string str; 

    bool dict = 0;
    while (getline(file, str)){
        if(str == "xxxx"){ dict = 1; continue;}

        if(!dict){ all+=str;}
        else vec_dict.push_back(str);
    }

    for(int i=0;i<8;i++)
        alltypes[bitset<3>(i).to_string()] = arr[i];

    int i=0;
    
    while(i < all.size()){
        string type = all.substr(i,3); //type 
        int skip    = alltypes[type];  //skip length 
		if(type == "111") break;

        string compressed_code = all.substr(i+3, skip); //extract compressed length

        compression.push_back(type); //save compression type
        file_cont.push_back(compressed_code); //compressed code segment

        i = i + 3 + skip;
    }  
}

string RLE_dec(int i){
   
    string out = "";
    int count = bitset<2>(file_cont[i]).to_ulong() + 1;

    for (int k=0;k < count ; k++)out+=d_previous;
    return out;
}

string MM_dec(int i,int count){
  
    int start_pos  = bitset<5>(file_cont[i].substr(0,5)).to_ulong();
    int dict_index = bitset<3>(file_cont[i].substr(5,3)).to_ulong();

    string ans  = "";

    string dict = vec_dict[dict_index];

    for(int k=0; k < dict.size(); k++){
        if(k==start_pos){
            if(dict[k]=='0')ans+='1';
            else ans+='0';

            for(int j=1; j < count; j++){
                k++;
                if(dict[k]=='0')ans+='1';
                else ans+='0';
            }
            continue;
        }else ans+=dict[k];
    }

    return ans;
}

string MM2_dec(int i){
 

    int pos1  = bitset<5>(file_cont[i].substr(0,5)).to_ulong();
    int pos2  = bitset<5>(file_cont[i].substr(5,5)).to_ulong();
    int dict_index = bitset<3>(file_cont[i].substr(10,3)).to_ulong();


    string ans  = "";
    string dict = vec_dict[dict_index];

    for(int k=0; k < dict.size(); k++){
        if(k==pos1 || k==pos2){
            if(dict[k]=='0')ans+='1';
            else ans+='0';

            continue;
        }else ans+=dict[k];
    }

    return ans;
}
string bitmask_dec(int i){
  
    string out = "";
    int start_pos  = bitset<5>(file_cont[i].substr(0,5)).to_ulong();
    int mask       = bitset<4>(file_cont[i].substr(5,4)).to_ulong() << (32-start_pos-4) ;
    int dict_index = bitset<3>(file_cont[i].substr(9,3)).to_ulong();
    
    string m_ = (bitset<32>(mask) ^ bitset<32>(vec_dict[dict_index])).to_string();
    return m_;
}

string decompressLine(int i){
    string type = compression[i];
    string compressed_code = file_cont[i];
   

    if (type=="110") { 
        if(compressed_code.size()==32)return compressed_code;
        else return "";
    }
    else if (type=="000") return RLE_dec(i);
    else if (type=="001") return bitmask_dec(i);
    else if (type=="010") return MM_dec(i,1);
    else if (type=="011") return MM_dec(i,2);
    else if (type=="100") return MM2_dec(i);
    else if(type=="101"){
        int key = bitset<3>(compressed_code).to_ulong();
        return vec_dict[key];
    }else return "";

} 

string decompress(){
    d_initialize(); 

    assert(compression.size() == file_cont.size());
    string out="";
    for(int i = 0 ; i < compression.size(); i++){
        d_previous = decompressLine(i); //decompress each line
        out+=d_previous;
        
    }

    writeToFile("dout.txt",out);
    return out;
}

int main(int argc, char *argv[]){
    if(argc==2){

        if(argv[1][0]=='1')compress();
        else if(argv[1][0]=='2') decompress();
        else cout<<"Wrong Argument"<<endl;

    }else{
        cout<<"Argument is not available\n";
    }


    return 0;
}
