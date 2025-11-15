#include<vector>
#include<algorithm>
#include<iostream>
using namespace std;


//打印容器vector
void print_vector(vector<int> v){
    if(v.size()>0){
        cout<<"{";  
        for(int i=0;i<int(v.size());i++){  
            cout<<v[i]<<",";  
        }  
        cout<<"\b}";  
    }
    else{
        cout<<"{}";
    }
}
 
//容器vector中元素的去重
vector<int> v_uni(vector<int> v){
    vector<int>::iterator vector_iterator;
    sort(v.begin(),v.end());
    vector_iterator = unique(v.begin(),v.end());
    if(vector_iterator != v.end()){
        v.erase(vector_iterator,v.end());
    }
    return v;
}
 
//两个vector求交集
vector<int> v_inter(vector<int> v1,vector<int> v2){
    vector<int> v;
    sort(v1.begin(),v1.end());   
    sort(v2.begin(),v2.end());   
    set_intersection(v1.begin(),v1.end(),v2.begin(),v2.end(),back_inserter(v));//求交集 
    return v;
}
 
//两个vector求并集
vector<int> v_cup(vector<int> v1,vector<int> v2){
    vector<int> v;
    sort(v1.begin(),v1.end());   
    sort(v2.begin(),v2.end());   
    set_union(v1.begin(),v1.end(),v2.begin(),v2.end(),back_inserter(v));//求交集 
    return v;
}
 
//求差集，v1中
vector<int> v_diff(vector<int> v1,vector<int> v2){
    vector<int> v;
    sort(v1.begin(),v1.end());   
    sort(v2.begin(),v2.end());
    vector<int> inter = v_inter(v1, v2);
    sort(inter.begin(), inter.end());
    set_difference(v1.begin(), v1.end(), inter.begin(), inter.end(), back_inserter(v));//求交集 
    return v;
}

//判断vector的某一元素是否存在
bool v_has(vector<int> v,int element){
    vector<int>::iterator it;
    it=find(v.begin(),v.end(),element);
    if (it!=v.end()){
        return true;
    }
    else{
        return false;
    }
}

//删除vector的某一元素
vector<int> v_del(vector<int> v, int element)
{
    vector<int>:: iterator it;
    it = find(v.begin(), v.end(), element);
    if(it != v.end()){
        v.erase(it);
        return v;
    }
    else
    {
        return v;
    }
}