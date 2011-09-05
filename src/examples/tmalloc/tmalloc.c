// tmalloc.c

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

// You may need to change a few things depending on your system.
// It shouldn't be difficult as there is not much to this library.

#define MALLOC 1
#define CALLOC 2
#define REALLOC 3

typedef struct tagNODE NODE;

// NODE structure before every heap object
typedef struct tagNODE {
	NODE  * pPrev;          // previous object in heap
	NODE  * pNext;          // next object in heap
	size_t  size;           // size of block
	int     type;           // "MALLOC", "CALLOC" or "REALLOC"
	char    filename[32];   // file/module name (names will be limited to 30 chars)
	int     linenum;        // line number
	void  * pMem;           // pointer to user memory
} NODE;

//  Points to latest object in linked list of heap objects
static NODE * pHeadNode = NULL;

// static prototypes
static void AppendToLinkedList( NODE * );
static void RemoveFromLinkedList( NODE * );
static BOOL IsValidPointer( void * );
static void GetNodeDescription( NODE *, char * );

static char * strnCpy(char * s1, char * s2, int num)
{
	strncpy(s1, s2, num);
	s1[num-1] = '\0';
	return s1;
}

static char * ExtractFileName( char * path )
{
	static char name[_MAX_FNAME];
	char ext[_MAX_EXT];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];

	_splitpath( path, drive, dir, name, ext );
	strcat(name, ext);
	return name;
}

static void AppendToLinkedList( NODE * pCurrent )
{
	// Add first node
	if (!pHeadNode) {
		pCurrent->pPrev = NULL; 	// no previous node
		pCurrent->pNext = NULL; 	// no next node
	} else { // else other nodes
		pCurrent->pPrev  = pHeadNode;
		pHeadNode->pNext = pCurrent;
		pCurrent->pNext  = NULL;	// no more nodes
	}
	// make current node head of list
	pHeadNode = pCurrent;
}

static void RemoveFromLinkedList( NODE * pNode )
{
	//  Remove from double linked list
	if (!pNode->pNext && !pNode->pPrev){ 		// only one node left
		pHeadNode = NULL;                    	// no nodes left
		return;
	}else if (!pNode->pPrev){                	// delete first node
		(pNode->pNext)->pPrev = NULL;
	}else if (!pNode->pNext){              		// delete last node
		pHeadNode = pNode->pPrev;
		(pNode->pPrev)->pNext = NULL;
	}else{										// delete other nodes
		(pNode->pPrev)->pNext = pNode->pNext;
		(pNode->pNext)->pPrev = pNode->pPrev;
	}
}

static BOOL IsValidPointer( void * pMem )
{
	BOOL fFlag = FALSE;

	NODE * pFindNode = (NODE*)pMem-1;

	NODE * pNode = pHeadNode;
	// walk through the list to find the node
	while( pNode ){
		if ( pNode == pFindNode ){
		    fFlag = TRUE;
			break;
		}
		pNode = pNode->pPrev;
	}
    return fFlag;    // if not found return FALSE
}

static void GetNodeDescription( NODE * pNode, char * pBuffer )
{
	if ( pNode->pMem == &pNode[1] ){ // check if node has memory
		char s[8];
		if ( pNode->type == MALLOC ){
			strcpy( s, "MALLOC " );
		}else if ( pNode->type == CALLOC ){
			strcpy( s, "CALLOC " );
		}else{
			strcpy( s, "REALLOC" );
		}
		sprintf( pBuffer, "MEM:[%p] TYPE:[%s] SIZE:[%d] LINE:[%d] FILE:[%s] ",
					pNode->pMem, s, pNode->size, pNode->linenum, pNode->filename );
	}else{
		strcpy( pBuffer, "(bad pointer)" );
	}
}

void tfree( void * pMem, char * pFile, int nLine )
{
	// free accepts a NULL pointer.
	if (!pMem)
		return;

	if (IsValidPointer(pMem)) {
		NODE * pNode = (NODE*)pMem-1;
		RemoveFromLinkedList( pNode );
		free(pNode);
	}else{
		char str[256], str1[32];
		strcpy(str, "Trying to free from invalid pointer from -> ");
		strnCpy(str1, ExtractFileName(pFile), 30);
		sprintf( str + strlen(str), "%s %s %d\n\r\n\r%s", str1, "at line ",
   									 nLine, "Aborting operation!" );
		MessageBox(NULL, str, "TMalloc Error. . . ", MB_OK);
	}
}

void * tmalloc( size_t size, char * pFile, int nLine )
{
	void * ret;
	NODE * pNode;

	pNode = malloc( sizeof(NODE) + size );
	if (pNode) {
		AppendToLinkedList( pNode );
		pNode->size      = size;
		pNode->type      = MALLOC;
	void * pn = pNode->filename;
		strnCpy(pNode->filename, ExtractFileName(pFile), 30);
		pNode->linenum   = nLine;
		// The user mem is + sizeof(NODE)
		// it will be on a 4 byte boundary
		pNode->pMem      = pNode+1;
		ret = pNode->pMem;
	}else {
		ret = NULL;
	}

	return ret;
}

void * tcalloc( size_t nmemb, size_t size, char * pFile, int nLine )
{
	void * ret;
	NODE * pNode;
	pNode = calloc( nmemb, sizeof(NODE) + size );
	if (pNode) {
		AppendToLinkedList( pNode );
		pNode->size      = size;
		pNode->type      = CALLOC;
		pNode->linenum   = nLine;
		// The user mem is + sizeof(NODE)
		// it will be on a 4 byte boundary
		pNode->pMem      = pNode+1;
		strnCpy(pNode->filename, ExtractFileName(pFile), 30);
		ret = pNode->pMem;
	}else {
		ret = NULL;
	}
	return ret;
}

void * trealloc( void * pOld, size_t size, char * pFile, int nLine )
{
	void * ret;
	NODE * pNode=(NODE*)pOld-1;
	NODE * pNewNode;

	// realloc works like malloc if NULL pointer is passed
	if (!pOld){
		ret = tmalloc( size, pFile, nLine );
		goto ende;
	}

	// realloc works like free if size is zero
	if (!size ){
		tfree( pOld, pFile, nLine );
		ret = NULL;
		goto ende;
	}

	if ( IsValidPointer(pOld) ) {

		RemoveFromLinkedList( pNode ); 	// take out old node

		// Try to reallocate block
		// The user mem is + sizeof(NODE)
		pNewNode = realloc( pNode, sizeof(NODE) + size);

		// if failed return
		if (!pNewNode){
			ret = NULL;
			goto ende;
		}

		AppendToLinkedList( pNewNode );		// insert in new postion

		pNewNode->size      = size;
		pNewNode->type      = REALLOC;
		pNewNode->linenum   = nLine;
		// The user mem is + sizeof(NODE)
		// it will be on a 4 byte boundary
		pNewNode->pMem      = pNewNode+1;
		strnCpy(pNewNode->filename, ExtractFileName(pFile), 30);
		ret = pNewNode->pMem;
	}else{
		char str[256], str1[32];
		strcpy(str, "Trying to realloc from invalid pointer from -> ");
		strnCpy(str1, ExtractFileName(pFile), 30);
		sprintf( str + strlen(str), "%s %s %d\n\r\n\r%s", str1, "at line ",
	   									 nLine, "Aborting operation!" );
		MessageBox(NULL, str, "TMalloc Error. . . ", MB_OK);
		ret = NULL;
	}

ende:
	return ret;
}

void report_heap( char * pOutFile = NULL )
{
	char buffer[200];
	FILE * fout = NULL;

	if ( pOutFile ){
		fout = fopen( pOutFile, "w");
		if (!fout){
			MessageBox(0, "Could not open report file!", "TMalloc Error. . . ", MB_OK);
			return;
		}
	}else{
		fout = stdout;
	}

	fprintf( fout, "\nWalking though allocated heap\n\n" );

	if (pHeadNode) {
		NODE * pCur = pHeadNode;
		while ( pCur ) {
			GetNodeDescription( pCur, buffer );
			fprintf( fout, "%s\n", buffer );
			pCur = pCur->pPrev;
		}
	}else{
		fprintf( fout, "No heap to walk\n" );
	}

	if ( pOutFile && fout )
		fclose(fout);
}






