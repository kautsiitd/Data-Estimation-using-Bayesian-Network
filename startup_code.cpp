#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <bits/stdc++.h>

// Format checker just assumes you have Alarm.bif and Solved_Alarm.bif (your file) in current directory
using namespace std;
vector< vector<int> > table,o_table;
int total_pat=0,var_size[37];
vector<int> max_type;
time_t start;

// Our graph consists of a list of nodes where each node is represented as follows:
class Graph_Node{

private:
	string Node_Name;  // Variable name 
	vector<int> Children; // Children of a particular node - these are index of nodes in graph.
	vector<string> Parents; // Parents of a particular node- note these are names of parents
	int nvalues;  // Number of categories a variable represented by this node can take
	vector<string> values; // Categories of possible values
	vector<float> CPT; // conditional probability table as a 1-d array . Look for BIF format to understand its meaning

public:
	// Constructor- a node is initialised with its name and its categories
    Graph_Node(string name,int n,vector<string> vals)
	{
		Node_Name=name;
	
		nvalues=n;
		values=vals;
		

	}
	string get_name()
	{
		return Node_Name;
	}
	vector<int> get_children()
	{
		return Children;
	}
	vector<string> get_Parents()
	{
		return Parents;
	}
	vector<float> get_CPT()
	{
		return CPT;
	}
	int get_nvalues()
	{
		return nvalues;
	}
	vector<string> get_values()
	{
		return values;
	}
	void set_CPT(vector<float> new_CPT)
	{
		CPT.clear();
		CPT=new_CPT;
	}
    void set_Parents(vector<string> Parent_Nodes)
    {
        Parents.clear();
        Parents=Parent_Nodes;
    }
    // add another node in a graph as a child of this node
    int add_child(int new_child_index )
    {
        for(int i=0;i<Children.size();i++)
        {
            if(Children[i]==new_child_index)
                return 0;
        }
        Children.push_back(new_child_index);
        return 1;
    }



};


 // The whole network represted as a list of nodes
class network{

	list <Graph_Node> Pres_Graph;

public:
	int addNode(Graph_Node node)
	{
		Pres_Graph.push_back(node);
		return 0;
	}
    
    
	int netSize()
	{
		return Pres_Graph.size();
	}
    // get the index of node with a given name
    int get_index(string val_name)
    {
        list<Graph_Node>::iterator listIt;
        int count=0;
        for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++)
        {
            if(listIt->get_name().compare(val_name)==0)
                return count;
            count++;
        }
        return -1;
    }
// get the node at nth index
    list<Graph_Node>::iterator get_nth_node(int n)
    {
       list<Graph_Node>::iterator listIt;
        int count=0;
        for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++)
        {
            if(count==n)
                return listIt;
            count++;
        }
        return listIt; 
    }
    //get the iterator of a node with a given name
    list<Graph_Node>::iterator search_node(string val_name)
    {
        list<Graph_Node>::iterator listIt;
        for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++)
        {
            if(listIt->get_name().compare(val_name)==0)
                return listIt;
        }
    
            cout<<"node not found\n";
        return listIt;
    }
	

}Alarm,last_Alarm;

network read_network()
{
	network Alarm;
	string line;
	int find=0;
  	ifstream myfile("alarm.bif"); 
  	string temp;
  	string name;
  	vector<string> values;
  	
    if (myfile.is_open())
    {
    	while (! myfile.eof() )
    	{
    		stringstream ss;
      		getline (myfile,line);
      		
      		
      		ss.str(line);
     		ss>>temp;
     		
     		
     		if(temp.compare("variable")==0)
     		{
                    
     				ss>>name;
     				getline (myfile,line);
                   
     				stringstream ss2;
     				ss2.str(line);
     				for(int i=0;i<4;i++)
     				{
     					
     					ss2>>temp;
     					
     					
     				}
     				values.clear();
     				while(temp.compare("};")!=0)
     				{
     					values.push_back(temp);
     					
     					ss2>>temp;
    				}
     				Graph_Node new_node(name,values.size(),values);
     				int pos=Alarm.addNode(new_node);
     				// cout << pos << endl;

     				
     		}
     		else if(temp.compare("probability")==0)
     		{
                    
     				ss>>temp;
     				ss>>temp;
     				
                    list<Graph_Node>::iterator listIt;
                    list<Graph_Node>::iterator listIt1;
     				listIt=Alarm.search_node(temp);
                    int index=Alarm.get_index(temp);
                    ss>>temp;
                    values.clear();
     				while(temp.compare(")")!=0)
     				{
                        listIt1=Alarm.search_node(temp);
                        listIt1->add_child(index);
     					values.push_back(temp);
     					
     					ss>>temp;

    				}
                    listIt->set_Parents(values);
    				getline (myfile,line);
     				stringstream ss2;
                    
     				ss2.str(line);
     				ss2>> temp;
                    
     				ss2>> temp;
                    
     				vector<float> curr_CPT;
                    string::size_type sz;
     				while(temp.compare(";")!=0)
     				{
                        
     					curr_CPT.push_back(atof(temp.c_str()));
     					
     					ss2>>temp;
                       
                        

    				}
                    
                    listIt->set_CPT(curr_CPT);


     		}
            else
            {
                
            }
     		
     		

    		
    		
    	}
    	
    	if(find==1)
    	myfile.close();
  	}
  	
  	return Alarm;
}

void ran_cpt(){
	// start from variables and finding corresponding CPT
	for(int i=0;i<37;i++){
		list<Graph_Node>::iterator cu_var=Alarm.get_nth_node(i);
		vector<string> cu_par=cu_var->get_Parents();
		// also including number itself as parent at 0th index
		int n_par=cu_par.size()+1;
		int par_ind[n_par+1],max_par[n_par+1],l_frq=cu_var->get_nvalues();
		par_ind[0]=i;
		max_par[0]=cu_var->get_nvalues();
		for(int j=1;j<n_par;j++){
			// where to search in records
			par_ind[j]=Alarm.get_index(cu_par[j-1]);
			// storing low medium high for parents
			max_par[j]=Alarm.search_node(cu_par[j-1])->get_nvalues();
			l_frq*=max_par[j];
		}
		float frq[l_frq],o_frq[l_frq];
		for(int j=0;j<l_frq;j++){
			frq[j]=.0001;
			o_frq[j]=.0001;
		}
		// finding frquency of diffrent diffrent cpt
		for(int j=0;j<total_pat;j++){
			int ind_incr=0,temp=1,skip=0;
			for(int k=n_par-1;k>=0;k--){
				if(o_table[j][par_ind[k]]==-1){
					skip=1;
				}
				ind_incr+=temp*table[j][par_ind[k]];
				temp*=max_par[k];
			}
			if(skip!=1)o_frq[ind_incr]+=1;
			frq[ind_incr]+=1;
		}
		// calculating prob of cpt's
		vector<float> new_cpt;
		int first_s=cu_var->get_nvalues();
		for(int j=0;j<l_frq;j++){
			float numerator=frq[j]*1.0,denomenator=0;
			for(int k=0;k<max_par[0];k++){
				denomenator+=frq[(j+(l_frq/first_s)*k)%l_frq];
			}
			float prob=numerator/denomenator;
			new_cpt.push_back(prob);
		}
		Alarm.get_nth_node(i)->set_CPT(new_cpt);
	}

	//printf("3\n");
}

void cpt_newcpt(){
	// generating new_frq
	for(int i=0;i<37;i++){
		list<Graph_Node>::iterator cu_var=Alarm.get_nth_node(i);
		vector<string> cu_par=cu_var->get_Parents();
		// also including number itself as parent at 0th index
		int n_par=cu_par.size()+1;
		int par_ind[n_par+1],max_par[n_par+1],l_frq=cu_var->get_nvalues();
		par_ind[0]=i;
		max_par[0]=cu_var->get_nvalues();
		for(int j=1;j<n_par;j++){
			// where to search in records
			par_ind[j]=Alarm.get_index(cu_par[j-1]);
			// storing low medium high for parents
			max_par[j]=Alarm.search_node(cu_par[j-1])->get_nvalues();
			l_frq*=max_par[j];
		}
		float o_frq[l_frq];
		for(int j=0;j<l_frq;j++){
			o_frq[j]=.0001;
		}
		// finding frquency of diffrent diffrent cpt
		for(int j=0;j<total_pat;j++){
			int ind_incr=0,temp=1,skip=0;
			for(int k=n_par-1;k>=0;k--){
				if(o_table[j][par_ind[k]]==-1){
					skip=1;
				}
				ind_incr+=temp*table[j][par_ind[k]];
				temp*=max_par[k];
			}
			if(skip==1){
				int ind_incr=0,temp=1,skip=0;
				for(int k=n_par-1;k>0;k--){
					ind_incr+=temp*table[j][par_ind[k]];
					temp*=max_par[k];
				}
				vector<float> cpt;
				cpt=cu_var->get_CPT();
				for(int k=0;k<max_par[0];k++){
					o_frq[ind_incr+k*temp]+=cpt[ind_incr+k*temp];
				}
			}
			else{
				o_frq[ind_incr]+=1;
			}
		}
		// calculating prob of cpt's
		vector<float> new_cpt,last_cpt=Alarm.get_nth_node(i)->get_CPT();
		int first_s=cu_var->get_nvalues();
		for(int j=0;j<l_frq;j++){
			float numerator=o_frq[j]*1.0,denomenator=0;
			for(int k=0;k<max_par[0];k++){
				denomenator+=o_frq[(j+(l_frq/first_s)*k)%l_frq];
			}
			float prob=numerator/denomenator;
			new_cpt.push_back(prob);
		}
		last_Alarm.get_nth_node(i)->set_CPT(last_cpt);
		Alarm.get_nth_node(i)->set_CPT(new_cpt);
	}
	//printf("4\n");
}

float diff(){
	float d=0;
	for(int i=0;i<37;i++){
		int limit=var_size[i];
		vector<float> old=last_Alarm.get_nth_node(i)->get_CPT(),newone=Alarm.get_nth_node(i)->get_CPT();
		for(int j=0;j<limit;j++){
			d+=abs(old[j]-newone[j]);
		}
	}
	return d;
}

int main(int argc,const char* argv[])
{
	time_t start=time(NULL),current;
	Alarm=read_network();
	last_Alarm=read_network();
	for(int i=0;i<37;i++){
		list<Graph_Node>::iterator cu_var=Alarm.get_nth_node(i);
		vector<string> cu_par=cu_var->get_Parents();
		// also including number itself as parent at 0th index
		int n_par=cu_par.size()+1;
		int par_ind[n_par+1],max_par[n_par+1],l_frq=cu_var->get_nvalues();
		par_ind[0]=i;
		max_par[0]=cu_var->get_nvalues();
		for(int j=1;j<n_par;j++){
			// where to search in records
			par_ind[j]=Alarm.get_index(cu_par[j-1]);
			// storing low medium high for parents
			max_par[j]=Alarm.search_node(cu_par[j-1])->get_nvalues();
			l_frq*=max_par[j];
		}
		var_size[i]=l_frq;
	}
    
// Example: to do something
// taking zero based value for low normal high
	fstream records;
	records.open(argv[2]);
	string line;
	// max_type and type for all 37 variables
	vector< vector<string> > type;
	for(int i=0;i<37;i++){
		type.push_back(Alarm.get_nth_node(i)->get_values());
		max_type.push_back(Alarm.get_nth_node(i)->get_nvalues()-1);
	}
	//printf("1\n");
	// making table storing whole records as in form of 0 1 2
	while(!records.eof()){
		stringstream ss;
		getline(records,line);
		ss.str(line);
		vector<int> diseases,o_diseases;
		for(int i=0;i<37;i++){
			string temp;
			ss>>temp;
			int val=0,o_val=0;
			if(temp[1]=='?'){
				// choose random between 0 and max_type including both
				o_val=-1;
				val=rand()%(max_type[i]+1);
			}
			else{
				while(temp!=type[i][val]){
					val++;
					o_val++;
				}
			}
			diseases.push_back(val);
			o_diseases.push_back(o_val);
		}
		table.push_back(diseases);
		o_table.push_back(o_diseases);
		total_pat+=1;
	}
	records.close();

	//printf("2\n");
	ran_cpt();
	current=time(NULL);
	for(int k=0;k<500 && current-start<480 ;k++){
		cpt_newcpt();
		float dif=diff();
		//printf("diffrence %f\n",dif);
		if(dif<.0001){
			break;
		}
		current=time(NULL);
		//printf("k %d\n",k);
	}

	// writing solved_alarm.bif
	ifstream alarm("alarm.bif");
	ofstream solved("solved_alarm.bif");
	int var_num=0,cpt_num=0;
	char ch;
	while(var_num!=37){
		alarm.get(ch);
		vector<float> ans_cpt=Alarm.get_nth_node(var_num)->get_CPT();
		cpt_num=0;
		int found=0;
		while(ch!='\n'){
			if(ch=='-'){
				if(ans_cpt[cpt_num]<.00005){
					solved<<.0001;
				}
				else if(ans_cpt[cpt_num]>=.99995){
					solved<<.9999;
				}
				else{
					solved<<fixed<<setprecision(4)<<ans_cpt[cpt_num];
				}
				cpt_num++;
				found=1;
				alarm.get(ch);
			}
			else{
				solved<<ch;
			}
			alarm.get(ch);	
		}
		if(found==1){
			var_num++;
		}
		solved<<ch;
	}
	while(!alarm.eof()){
		alarm.get(ch);
		if(alarm.eof()){
			break;
		}
		solved<<ch;
	}
	alarm.close();
	solved.close();
	//cout<<"Perfect! Hurrah! \n";
	return 0;
}





