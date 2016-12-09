/****************************************************************/
/*	Project: Hashtag counter using Fibonacci Heap				*/
/*	Last Edited	: Nov 18, 2016									*/
/*	Sri Akhilesh Joshi										    */
/*  UFID : 83666029												*/
/*  Course: Advanced Data Structures							*/
/*  Course Number: COP 5536										*/
/****************************************************************/
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <type_traits>
#include <math.h>
#include <sstream>
#include <map>
#include <utility>

using namespace std;
typedef pair<string, int> vector_pair;
struct node
{
    int data;
    node *right;
    node *left;
    node *child;
	string name;
	node *parent;
    bool childcut_value;
	int rank;

};

class Fibonacci_Heap
{
public:
	typedef pair<int,node*> hash_pair;
	
    void Insert(node* new_node);
    node* Create(string hashtag, int value);
	void Increase_Key(node* thenode, int value);
	void Cascade_Cut(node* thenode);
	void Remove_Node(node* thenode);
	void Remove_Max();
	void Meld();
	void Meld2(node* current_node, node* table_node);
	void FindMax();
	map<string,hash_pair> fib_hashtable;
	map<int, node*> fib_rank;
	node* max_pointer;
	vector<vector_pair> removed_values;
	void Delete(string hashtag);
	void Write_Output(string thestring);
    ofstream outfile;
	Fibonacci_Heap()
    {
		root=NULL;
		remove("output_file.txt");
		outfile.open("output_file.txt");
    }
private:
    node* root;
};

void Fibonacci_Heap::Insert(node* new_node)			/*Inserts new_node in the root list*/
{
    if(root==NULL)									/*Fibonacci Heap is new */
    {
        root=new_node;
		root->left=root;							/*Points to itself*/
		root->right=root;
		max_pointer=root;
    }
    else
    {
		(root->left)->right=new_node;				/*new_node is placed in the root list*/
		new_node->right=root;
		new_node->left=root->left;
		root->left=new_node;
		new_node->childcut_value=false;
		new_node->parent=NULL;
		if(new_node->data > max_pointer->data)
		{
			max_pointer=new_node;					/*max pointer is calculated*/
		}
    }
}

node* Fibonacci_Heap::Create(string hashtag, int value)
{
	/*See if the hashtag in the hash_table or not. If not, create the node or else do increment max and update the max_pointer*/

	if(fib_hashtable.find(hashtag) == fib_hashtable.end())		/*if not found*/
	{
		node *create_node=new node;								/*node is initialized*/
		create_node->data=value;
		create_node->right=NULL;
		create_node->left=NULL;
		create_node->child= NULL;
		create_node->parent=NULL;
		create_node->childcut_value=false;
		create_node->rank=0;
		create_node->name=hashtag;
		fib_hashtable[hashtag] = make_pair(value, create_node);
		Insert(create_node);
		return create_node;
	}
	else //found in hashtable
	{
		/* call increasekey operation */

		fib_hashtable[hashtag].first+=value;					/*update the hash table*/
		Increase_Key(fib_hashtable[hashtag].second, value);		/*Increase_Key will increase the data*/
		return fib_hashtable[hashtag].second;
	}
}

void Fibonacci_Heap::Increase_Key(node* thenode, int value)		/*should also incorporate the cascade cut operation*/
{
	thenode->data+=value;
	if(thenode->parent!=NULL && thenode->data > thenode->parent->data) /*first !=NULL should come or else seg fault*/
	{
		Remove_Node(thenode);									/*Node is removed and reinserted into the root list*/
		
		/* Node is now in the roor list*/

		Cascade_Cut(thenode);									/*Recursive Cascade Cut operation*/
	}
	if(thenode->parent==NULL)
	{
		if(thenode->data > max_pointer->data)
		{
			max_pointer=thenode;
		}
	}
}

void Fibonacci_Heap::Cascade_Cut(node* thenode)
{
			node* parent = thenode->parent;
			//first remove the node(see 2 steps below) and then insert the node.
			//this node has to be removed from the doubly linked list which is with its siblings
			//2 cases. One is its the direct child and the other is not.
			//Remove_Node(thenode); //Insert happening in removenode itself
			//Insert(thenode); //if parent=NULL insert shouldn't happen.
			if(parent!=NULL)									/* Has parent - Didn't reach root list*/
			{
				if(parent->childcut_value ==false)				/*Parent is safe-not removed*/
				{
					parent->childcut_value=true;				/*Child cut value is changed to true and cascade cut stops here*/
				}
				else
				{
					Remove_Node(parent);						/*Parent is removed*/

					/*Parent is now in the root list*/

					Cascade_Cut(parent);						/*Cascade cut - recursive*/
				}
			}
}

void Fibonacci_Heap::Remove_Node(node* thenode)					/*Will be called only from cascade_cut*/
{
	node* parent= thenode->parent;
	if(parent==NULL)											/*Reached the root list*/
	{
		return;
	}
	else
	{
		if(thenode==thenode->right)								/*only child*/
		{
			thenode->parent->child=NULL;						/*making the child NULL*/
		}
		(thenode->left)->right=thenode->right;
		(thenode->right)->left=thenode->left;					/*Node is removed*/
		if(thenode==thenode->parent->child)
		{
			thenode->parent->child=thenode->right;				/*Child pointer is changed if the removed node was previously the child*/
		}
		thenode->parent->rank-=1;								/*Decreasing the rank as child is removed*/
		thenode->right = thenode;
		thenode->left = thenode;								/*Node completed removed*/
		thenode->parent->childcut_value = true;					/*Child cut of parent made true as node is removed*/
		Insert(thenode);										/*Inserts the node in the root list*/
	}
}

void Fibonacci_Heap::Remove_Max()
{
	string print_max = max_pointer->name;
	print_max.erase(0,1);
	//cout << << max_pointer->data;			/*Erasing # from the hashtag*/
	Write_Output(print_max);									/*Writes the max pointer to output file*/


	removed_values.push_back(make_pair(max_pointer->name, fib_hashtable[max_pointer->name].first));

	/*Stores name, value of the max pointer in the removed_values vector and later delete the entry from hashtable and delete the node*/
	
	if(root->right == root || root->left ==root)
	{
		root = NULL;											/*Only element*/
	}
	else if(root==max_pointer)									/*Changing root if it is the max pointer*/
	{
		root=max_pointer->right;
	}
	if(max_pointer->child!=NULL)								/*Max pointer has children, so add the rank1 children to the root list*/
	{
		node* child = max_pointer->child;
		node* tmp = child;

		/*Remove from the max pointer from the root list and modify the root list*/

		(max_pointer->right)->left = max_pointer->left;
		(max_pointer->left)->right=max_pointer->right;
		max_pointer->right=max_pointer;
		max_pointer->left=max_pointer;
		max_pointer->child = NULL;
		max_pointer->childcut_value =false;
		max_pointer->parent=NULL;

		/*Make child and others NULL*/

		vector<node*> child_pointers;
		child_pointers.push_back(tmp);
		if(tmp!=NULL)
		{
			tmp = tmp->right; //Condition for NULL here. Error here NULL->right
			while(tmp!=child)
			{
				child_pointers.push_back(tmp);
				tmp = tmp->right;
			}
		}

		/*iterate through the vector while doing insert()*/
		
		vector<node*>::iterator node_iterator;

		/*Will delete the node and from the hashtable here*/

		Delete(max_pointer->name);
		
		for(node_iterator=child_pointers.begin(); node_iterator!=child_pointers.end(); node_iterator++)
		{
			Insert(*node_iterator);									/*Insert the rank1 children in the root list*/
		}
	}
	else
	{
		
		if(max_pointer==root)
		{
			root=max_pointer->right;								/*Changing the root to the right pointer as max pointer will be removed*/
		}

		/*Remove from the root list*/

		(max_pointer->right)->left = max_pointer->left;				/*Maximum Node is removed in the next 4 statements*/
		(max_pointer->left)->right=max_pointer->right;
		max_pointer->right=max_pointer;
		max_pointer->left=max_pointer;
		max_pointer->child = NULL;									/*Making child and others NULL*/

		max_pointer->childcut_value =false;
		max_pointer->parent=NULL;

		/*Deleting the max_pointer*/

		Delete(max_pointer->name);
	}

	Meld();
	return;
}

void Fibonacci_Heap::Meld()
{
	fib_rank.clear();
	node* tmp = root;
	fib_rank[tmp->rank]=tmp;										/*for the table_node*/
	if(tmp->right!=NULL)											/*Elements exist or not*/
	{
		tmp = tmp->right;
		while(tmp!=root)
		{
			if(fib_rank.find(tmp->rank) == fib_rank.end())			/*if rank is not found then insert into hash table*/
			{
				fib_rank[tmp->rank]=tmp;							/*Insert into the hashtable*/
				if(tmp->right==NULL)
					return;
				tmp=tmp->right;
			}
			else													/*found in the hash table*/
			{
				
				/*Melding is done here by melding the current node with the node in the hashtable*/
				
				Meld2(tmp, fib_rank[tmp->rank]);					/*Actual Melding is done here*/
				tmp=root;
				tmp=tmp->right;
				if(tmp==NULL)
				{
					break;
				}
			}

		}
	}
	FindMax();														/*Set the new max here*/
}

void Fibonacci_Heap::Meld2(node* current_node, node* table_node)
{
	if(current_node->data > table_node->data)						/*tablenode should be removed*/
	{

		if(table_node==root)
		{
			root = current_node;									/*Root points to current_node as table_node becomes child of it*/
		}
		if(current_node->child==NULL)								/* Current Node has no children*/
		{
			/*Below two statements removes table_node from root list*/

			(table_node->right)->left = table_node->left;
			(table_node->left)->right = table_node->right;
			table_node->right=table_node;
			table_node->left=table_node;							/*Totally removed from the root list*/
			current_node->child=table_node;
			current_node->rank=1;									/*Update the rank to 1 as there was no child before*/
			table_node->parent=current_node;
			fib_rank.clear();										/*clearing the hash table and you start again from the root*/
		}
		else														/*has children, then add the table_node to the list of children to the current_node*/
		{
			(table_node->left)->right = table_node->right;
			(table_node->right)->left = table_node->left;
			node* child = current_node->child;
			node* after_node = child->right;
			child->right = table_node;
			table_node->left=child;
			table_node->right=after_node;
			after_node->left=table_node;
			table_node->parent=current_node;						/*update the parent*/
			current_node->rank+=1;									/*Rank increased by 1*/
			fib_rank.clear(); //clear the hash table and you start again from the root
		}
	}
	else															/*current_node will be removed*/
	{
		if(current_node==root)
		{
			root = table_node;
		}
		if(table_node->child==NULL)
		{
			(current_node->right)->left = current_node->left;
			(current_node->left)->right = current_node->right;

			/*above two statements removes table_node from root list*/
			
			current_node->right=current_node;
			current_node->left=current_node;
			table_node->child=current_node;
			current_node->parent=table_node;
			table_node->rank=1;										/*Now current node is the only child of table node. So rank is 1*/
		}
		else														/*has children then add current_node to children of table_node*/
		{
			(current_node->right)->left = current_node->left;
			(current_node->left)->right = current_node->right;

			/*above two statements removes current_node from root_list*/

			node* child = table_node->child;
			node* after_node = child->right;
			child->right = current_node;
			current_node->left=child;
			current_node->right=after_node;
			after_node->left=current_node;
			current_node->parent=table_node;
			table_node->rank+=1;									/*Increasing the rank by 1*/
		}
	}
	fib_rank.clear();												/*clear the fib_rank table*/
	fib_rank[root->rank]=root;										/*Inserting root's rank in the hash table*/
}

void Fibonacci_Heap::FindMax()										/*Finds max pointer*/
{
	node* tmp = root;
	max_pointer = root;
	tmp = tmp->right;
	if(tmp!=NULL)
	{
		while(tmp!=root)
		{
			if(tmp->data > max_pointer->data)
			{
				max_pointer=tmp;
			}
			tmp=tmp->right;
		}
	}
}


void Fibonacci_Heap::Write_Output(string thestring)				/*Writes max pointer to output_file.txt*/
{
	outfile << thestring;										/*Appending the output*/

}

void Fibonacci_Heap::Delete(string hashtag)						/*Deleting the hashtag*/
{
	delete fib_hashtable[hashtag].second;						/*Deleting the node to free memory*/
	map<string, hash_pair>::iterator iterate;
	iterate = fib_hashtable.find(hashtag);
	fib_hashtable.erase(iterate);								/*Erasing from hash table*/
}

int main(int argc, char *argv[])
{
	Fibonacci_Heap fib_heap;
	string line;
	int i=0;
	ifstream infile(argv[1]);									/*Input steam of the file*/
	while (getline(infile, line))
	{
		istringstream iss(line);
		int value;
		int query_value;
		string input_hashtag;
		if (!(iss >> input_hashtag >> value))												/*Eating the input into input_hashtag and value*/
		{
			if(input_hashtag.compare("STOP")==0)
			{
				break;
			}
			else
			{
				stringstream convert(input_hashtag);
				convert>>query_value;
				vector<vector_pair>::iterator node_iterator;
				i=0;
				fib_heap.removed_values.clear();											/*Cleared so that next query is clean*/
				while(i<query_value)
				{
					fib_heap.Remove_Max();
					i++;
					if(i<query_value)														/*Will prevent from appending ',' for the last value*/
					{
						fib_heap.Write_Output(",");											/*Appending output to the file*/
					}
				}
				fib_heap.Write_Output("\n");												/*Goes to next line after query is displayed*/

				/*Inserting all the removed nodes*/

				for(node_iterator=fib_heap.removed_values.begin(); node_iterator!=fib_heap.removed_values.end(); node_iterator++)
				{
					fib_heap.Create(node_iterator->first, node_iterator->second);
		
				} 

			}
		}
		else
		{
			node* created = fib_heap.Create(input_hashtag,value);						/*Insert or increase key*/
		}
	}
    return 1;
}
