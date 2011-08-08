/**
	Code of the implementation: 
	http://www.daniweb.com/software-development/c/code/216526
 **/

#include <stdio.h>
#include <stdlib.h>


#include "../includes.h"
#include "tree.h"

# define F 0
# define T 1


typedef struct NODE * node;

struct NODE
{
	void_p Info;
	int Flag;
	node Left_Child;
	node Right_Child;
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

node Binary_Tree(tree t, void_p Info, node Parent, int *H, int * found)
{
	node Node1;
	node Node2;
	if(!Parent)
	{
		Parent = (node) malloc(sizeof(struct NODE));
		Parent->Info = Info;
		Parent->Left_Child = NULL;
		Parent->Right_Child = NULL;
		Parent->Flag = 0;
		*H = T;
		return (Parent);
	}
	
	if(t->comp(Info, Parent->Info) < 0)
	{
		Parent->Left_Child = Binary_Tree(t, Info, Parent->Left_Child, H, found);
		if(*H)
		/* Left branch has grown higher */
		{
			switch(Parent->Flag)
			{
				case 1: /* Right heavy */
					Parent->Flag = 0;
					*H = F;
					break;
				case 0: /* Balanced tree */
					Parent->Flag = -1;
					break;
				case -1: /* Left heavy */
					Node1 = Parent->Left_Child;
					if(Node1->Flag == -1)
					{
						Parent->Left_Child= Node1->Right_Child;
						Node1->Right_Child = Parent;
						Parent->Flag = 0;
						Parent = Node1;
					}
					else
					{
						Node2 = Node1->Right_Child;
						Node1->Right_Child = Node2->Left_Child;
						Node2->Left_Child = Node1;
						Parent->Left_Child = Node2->Right_Child;
						Node2->Right_Child = Parent;
						if(Node2->Flag == -1)
							Parent->Flag = 1;
						else
							Parent->Flag = 0;
						if(Node2->Flag == 1)
							Node1->Flag = -1;
						else
							Node1->Flag = 0;
						Parent = Node2;
					}
					
					Parent->Flag = 0;
					*H = F;
			}
		}
	}
	else
	if(t->comp(Info, Parent->Info) > 0)
	{
		Parent->Right_Child = Binary_Tree(t,Info, Parent->Right_Child, H, found);
		if(*H)
		/* Right branch has grown higher */
		{
			switch(Parent->Flag)
			{
				case -1: /* Left heavy */
					Parent->Flag = 0;
					*H = F;
					break;
				case 0: /* Balanced tree */
					Parent->Flag = 1;
					break;
					
				case 1: /* Right heavy */
					Node1 = Parent->Right_Child;
					if(Node1->Flag == 1)
					{
						Parent->Right_Child= Node1->Left_Child;
						Node1->Left_Child = Parent;
						Parent->Flag = 0;
						Parent = Node1;
					}
					else
					{
						Node2 = Node1->Left_Child;
						Node1->Left_Child = Node2->Right_Child;
						Node2->Right_Child = Node1;
						Parent->Right_Child = Node2->Left_Child;
						Node2->Left_Child = Parent;
						
						if(Node2->Flag == 1)
							Parent->Flag = -1;
						else
							Parent->Flag = 0;
						if(Node2->Flag == -1)
							Node1->Flag = 1;
						else
							Node1->Flag = 0;
						Parent = Node2;
					}
					
					Parent->Flag = 0;
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
		Output(Tree->Right_Child, Level+1, p);
		printf("\n");
		for (i = 0; i < Level; i++)
			printf("   ");
		p(Tree->Info);
		Output(Tree->Left_Child, Level+1,p);
	}
}

/* Balancing Right Heavy */

node  Balance_Right_Heavy(node Parent, int *H)
{
	node Node1, Node2;
	
	switch(Parent->Flag)
	{
		case -1: 
			Parent->Flag = 0;
			break;
			
		case 0: 
			Parent->Flag = 1;
			*H= F;
			break;
			
		case 1: /* Rebalance */
			Node1 = Parent->Right_Child;
			if(Node1->Flag >= 0)
			{
				Parent->Right_Child= Node1->Left_Child;
				Node1->Left_Child = Parent;
				if(Node1->Flag == 0)
				{
					Parent->Flag = 1;
					Node1->Flag = -1;
					*H = F;
				}
				else
				{
					Parent->Flag = Node1->Flag = 0;
				}
				Parent = Node1;
			}
			else
			{
				Node2 = Node1->Left_Child;
				Node1->Left_Child = Node2->Right_Child;
				Node2->Right_Child = Node1;
				Parent->Right_Child = Node2->Left_Child;
				Node2->Left_Child = Parent;
				
				if(Node2->Flag == 1)
					Parent->Flag = -1;
				else
					Parent->Flag = 0;
				if(Node2->Flag == -1)
					Node1->Flag = 1;
				else
					Node1->Flag = 0;
				Parent = Node2;
				Node2->Flag = 0;
			}
	}
	return(Parent);
}

/* Balancing Left Heavy */

node  Balance_Left_Heavy(node Parent, int *H)
{
	node Node1, Node2;
	
	switch(Parent->Flag)
	{
		case 1: 
			Parent->Flag = 0;
			break;
			
		case 0: 
			Parent->Flag = -1;
			*H= F;
			break;
			
		case -1: /*  Rebalance */
			Node1 = Parent->Left_Child;
			if(Node1->Flag <= 0)
			{
				Parent->Left_Child= Node1->Right_Child;
				Node1->Right_Child = Parent;
				if(Node1->Flag == 0)
				{
					Parent->Flag = -1;
					Node1->Flag = 1;
					*H = F;
				}
				else
				{
					Parent->Flag = Node1->Flag = 0;
				}
				Parent = Node1;
			}
			else
			{
				Node2 = Node1->Right_Child;
				Node1->Right_Child = Node2->Left_Child;
				Node2->Left_Child = Node1;
				Parent->Left_Child = Node2->Right_Child;
				Node2->Right_Child = Parent;
				
				if(Node2->Flag == -1)
					Parent->Flag = 1;
				else
					Parent->Flag = 0;
				
				if(Node2->Flag == 1)
					Node1->Flag = -1;
				else
					Node1->Flag = 0;
				Parent = Node2;
				Node2->Flag = 0;
			}
	}
	return(Parent);
}

/* Replace the node at which key is found with last right key of a left child */

node  DELETE(node R, node Temp, int *H)
{
	node Dnode = R;
	if( R->Right_Child != NULL)
	{
		R->Right_Child = DELETE(R->Right_Child, Temp, H);
		if(*H)
			R = Balance_Left_Heavy(R, H);
	}
	else
	{
		Dnode = R;
		Temp->Info = R->Info;
		R = R->Left_Child;
		free(Dnode);
		*H = T;
	}
	return(R);
}
/* Delete the key element from the tree */

node  Delete_Element(tree t, node Parent, void_p Info, int *H, int * found, void_p found_data)
{
	node Temp;
	if(!Parent)
	{
		printf("\n Information does not exist");
		return(Parent);
	}
	else
	{
		if (t->comp(Info, Parent->Info) < 0)
		{
			Parent->Left_Child = Delete_Element(t, Parent->Left_Child, Info, H, found, found_data);
			if(*H)
				Parent = Balance_Right_Heavy(Parent, H);
		}
		else
			if(t->comp(Info, Parent->Info) > 0)
			{
				Parent->Right_Child = Delete_Element(t, Parent->Right_Child, Info, H, found, found_data);
				if(*H)
					Parent = Balance_Left_Heavy(Parent, H);
			}
			else
			{
				*found = 1;
				*((int*)found_data) = (int) Parent->Info;
				Temp= Parent;
				
				if(Temp->Right_Child == NULL)
				{
					Parent = Temp->Left_Child;
					*H = T;
					free(Temp);
				}
				else
					if(Temp->Left_Child == NULL)
					{
						Parent = Temp->Right_Child;
						*H = T;
						free(Temp);
					}
					else
					{
						Temp->Left_Child = DELETE(Temp->Left_Child, Temp, H);
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
	else if (c(e, n->Info) < 0)
		return node_contains(c,n->Left_Child,e);
	else if (c(e, n->Info) > 0) {
		return node_contains(c,n->Right_Child,e);
	} else {
		return n->Info;
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

void explore(comparer c, node n, void_p * array, int * index) {
	if (n == NULL)
		return;
	explore(c,n->Left_Child,array,index);
	
	((int*)array)[(*index)++] = (int) n->Info;
	
	explore(c,n->Right_Child,array,index);
}


void_p * tree_to_a(tree t)
{
	int * array = (int *)malloc(sizeof(void_p));
	int i = 0;
	explore(t->comp, t->header,(void_p) array, &i);
	
	return (void_p *) array;
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


