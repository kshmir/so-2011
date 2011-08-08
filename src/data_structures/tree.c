/**
	Code of the implementation: 
	http://www.daniweb.com/software-development/c/code/216526
 **/

#include <stdio.h>
#include <stdlib.h>


#include "../includes.h"
#include "tree.h"
#include "list.h"

# define F 0
# define T 1


typedef struct node * node
;
struct node {
	void_p data;
	int flag;
	node left;
	node right;
};




struct tree
{
	int size;
	comparer comp;
	node  header;
	int *H;
};



static node Binary_Tree (tree,void_p, node, int *, int*);
static node Balance_Right_Heavy(node, int *);
static node Balance_Left_Heavy(node, int *);
static void Output(node, int, printer);
static node DELETE(node, node, int *);
static node Delete_Element(tree, node, void_p, int *, int*, void_p);

/* Function to insert an element into tree */

node Binary_Tree(tree t, void_p data, node Parent, int *H, int * found)
{
	node Node1;
	node Node2;
	if(!Parent)
	{
		Parent = (node) malloc(sizeof(struct node));
		Parent->data = data;
		Parent->left = NULL;
		Parent->right = NULL;
		Parent->flag = 0;
		*H = T;
		return (Parent);
	}
	
	if(t->comp(data, Parent->data) < 0)
	{
		Parent->left = Binary_Tree(t, data, Parent->left, H, found);
		if(*H)
		/* Left branch has grown higher */
		{
			switch(Parent->flag)
			{
				case 1: /* Right heavy */
					Parent->flag = 0;
					*H = F;
					break;
				case 0: /* Balanced tree */
					Parent->flag = -1;
					break;
				case -1: /* Left heavy */
					Node1 = Parent->left;
					if(Node1->flag == -1)
					{
						Parent->left= Node1->right;
						Node1->right = Parent;
						Parent->flag = 0;
						Parent = Node1;
					}
					else
					{
						Node2 = Node1->right;
						Node1->right = Node2->left;
						Node2->left = Node1;
						Parent->left = Node2->right;
						Node2->right = Parent;
						if(Node2->flag == -1)
							Parent->flag = 1;
						else
							Parent->flag = 0;
						if(Node2->flag == 1)
							Node1->flag = -1;
						else
							Node1->flag = 0;
						Parent = Node2;
					}
					
					Parent->flag = 0;
					*H = F;
			}
		}
	}
	else
	if(t->comp(data, Parent->data) > 0)
	{
		Parent->right = Binary_Tree(t,data, Parent->right, H, found);
		if(*H)
		/* Right branch has grown higher */
		{
			switch(Parent->flag)
			{
				case -1: /* Left heavy */
					Parent->flag = 0;
					*H = F;
					break;
				case 0: /* Balanced tree */
					Parent->flag = 1;
					break;
					
				case 1: /* Right heavy */
					Node1 = Parent->right;
					if(Node1->flag == 1)
					{
						Parent->right= Node1->left;
						Node1->left = Parent;
						Parent->flag = 0;
						Parent = Node1;
					}
					else
					{
						Node2 = Node1->left;
						Node1->left = Node2->right;
						Node2->right = Node1;
						Parent->right = Node2->left;
						Node2->left = Parent;
						
						if(Node2->flag == 1)
							Parent->flag = -1;
						else
							Parent->flag = 0;
						if(Node2->flag == -1)
							Node1->flag = 1;
						else
							Node1->flag = 0;
						Parent = Node2;
					}
					
					Parent->flag = 0;
					*H = F;
			}
		}
	}
	else {
		*found = 1;
	}
	return(Parent);
}

/* Output function */

void Output(node Tree,int Level, printer p)
{
	int i;
	if (Tree)
	{
		Output(Tree->right, Level+1, p);
		printf("\n");
		for (i = 0; i < Level; i++)
			printf("   ");
		p(Tree->data);
		Output(Tree->left, Level+1,p);
	}
}

/* Balancing Right Heavy */

node  Balance_Right_Heavy(node Parent, int *H)
{
	node Node1, Node2;
	
	switch(Parent->flag)
	{
		case -1: 
			Parent->flag = 0;
			break;
			
		case 0: 
			Parent->flag = 1;
			*H= F;
			break;
			
		case 1: /* Rebalance */
			Node1 = Parent->right;
			if(Node1->flag >= 0)
			{
				Parent->right= Node1->left;
				Node1->left = Parent;
				if(Node1->flag == 0)
				{
					Parent->flag = 1;
					Node1->flag = -1;
					*H = F;
				}
				else
				{
					Parent->flag = Node1->flag = 0;
				}
				Parent = Node1;
			}
			else
			{
				Node2 = Node1->left;
				Node1->left = Node2->right;
				Node2->right = Node1;
				Parent->right = Node2->left;
				Node2->left = Parent;
				
				if(Node2->flag == 1)
					Parent->flag = -1;
				else
					Parent->flag = 0;
				if(Node2->flag == -1)
					Node1->flag = 1;
				else
					Node1->flag = 0;
				Parent = Node2;
				Node2->flag = 0;
			}
	}
	return(Parent);
}

/* Balancing Left Heavy */

node  Balance_Left_Heavy(node Parent, int *H)
{
	node Node1, Node2;
	
	switch(Parent->flag)
	{
		case 1: 
			Parent->flag = 0;
			break;
			
		case 0: 
			Parent->flag = -1;
			*H= F;
			break;
			
		case -1: /*  Rebalance */
			Node1 = Parent->left;
			if(Node1->flag <= 0)
			{
				Parent->left= Node1->right;
				Node1->right = Parent;
				if(Node1->flag == 0)
				{
					Parent->flag = -1;
					Node1->flag = 1;
					*H = F;
				}
				else
				{
					Parent->flag = Node1->flag = 0;
				}
				Parent = Node1;
			}
			else
			{
				Node2 = Node1->right;
				Node1->right = Node2->left;
				Node2->left = Node1;
				Parent->left = Node2->right;
				Node2->right = Parent;
				
				if(Node2->flag == -1)
					Parent->flag = 1;
				else
					Parent->flag = 0;
				
				if(Node2->flag == 1)
					Node1->flag = -1;
				else
					Node1->flag = 0;
				Parent = Node2;
				Node2->flag = 0;
			}
	}
	return(Parent);
}

/* Replace the node at which key is found with last right key of a left child */

node  DELETE(node R, node Temp, int *H)
{
	node Dnode = R;
	if( R->right != NULL)
	{
		R->right = DELETE(R->right, Temp, H);
		if(*H)
			R = Balance_Left_Heavy(R, H);
	}
	else
	{
		Dnode = R;
		Temp->data = R->data;
		R = R->left;
		free(Dnode);
		*H = T;
	}
	return(R);
}
/* Delete the key element from the tree */

node  Delete_Element(tree t, node Parent, void_p data, int *H, int * found, void_p found_data)
{
	node Temp;
	if(!Parent)
	{
		printf("\n datarmation does not exist");
		return(Parent);
	}
	else
	{
		if (t->comp(data, Parent->data) < 0)
		{
			Parent->left = Delete_Element(t, Parent->left, data, H, found, found_data);
			if(*H)
				Parent = Balance_Right_Heavy(Parent, H);
		}
		else
			if(t->comp(data, Parent->data) > 0)
			{
				Parent->right = Delete_Element(t, Parent->right, data, H, found, found_data);
				if(*H)
					Parent = Balance_Left_Heavy(Parent, H);
			}
			else
			{
				*found = 1;
				*((int*)found_data) = (int) Parent->data;
				Temp= Parent;
				
				if(Temp->right == NULL)
				{
					Parent = Temp->left;
					*H = T;
					free(Temp);
				}
				else
					if(Temp->left == NULL)
					{
						Parent = Temp->right;
						*H = T;
						free(Temp);
					}
					else
					{
						Temp->left = DELETE(Temp->left, Temp, H);
						if(*H)
							Parent = Balance_Right_Heavy(Parent, H);
					}
			}
	}
	return(Parent);
}

void_p tree_delete(tree t, void_p e)
{
	int boolean = 0;
	void_p data = (void_p) malloc(sizeof(void_p));
	t->header = Delete_Element(t, t->header, e, t->H, &boolean, (void_p)data);
	if (boolean) {
		t->size--;
		int ret = (int)(*((int*)data));
		free(data);
		return (void_p)ret;
	}
	else {
		free(data);
		return NULL;
	}

}

int tree_add(tree t, void_p e)
{
	int boolean = 0;
	t->header = Binary_Tree(t, e, t->header, t->H, &boolean);
	if (!boolean)
		t->size++;
	return !boolean;
}

static void_p node_contains(comparer c, node n, void_p e){
	if (n == NULL)
		return NULL;
	else if (c(e, n->data) < 0)
		return node_contains(c,n->left,e);
	else if (c(e, n->data) > 0) {
		return node_contains(c,n->right,e);
	} else {
		return n->data;
	}
}

void_p tree_get(tree t, void_p e)
{
	return node_contains(t->comp, t->header, e);
}

void tree_print(tree t, printer p)
{
	Output(t->header, 0, p);
}

int tree_size(tree t)
{
	return t->size;
}


void explore_list(comparer c, node n, list l) {
	if (n == NULL)
		return;
	explore_list(c,n->left,l);
	
	list_add(l, n->data);
	
	explore_list(c,n->right,l);
}


list tree_to_list(tree t)
{
	list l = list_init();
	explore_list(t->comp, t->header, l);
	
	return l;
}

tree tree_init(comparer comp)
{
	tree t = (tree) malloc(sizeof(struct tree));
	t->header = NULL;
	t->H = (int*)malloc(sizeof(int));
	t->size = 0;
	t->comp = comp;
	return t;
}

void tree_free(tree t) {
	free(t->H);
	free(t);
}


