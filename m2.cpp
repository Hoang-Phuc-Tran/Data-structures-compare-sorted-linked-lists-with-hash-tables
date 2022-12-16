/*
 * Project:	    Major 2
 * Author:	    Hoang Phuc Tran
 * Student ID:  8789102
 * Date:		June 29, 2022
 * Description:  A program that gets words to load into a hash table from a file called names.txt as 
				input (using file I/O). After that, search for the name in a standalone linked
				list and a hash table.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma warning(disable: 4996)


#define MAX_HASH 127
#define STRING_LENGTH 21


 // Prototype
unsigned long hash(char* str);
unsigned long myHashFunction(char* str);
int insert(struct WordNode** head, char* str);
struct WordNode* searchLinkedList(char* searchName, struct WordNode* head, int* comparisonCoun);
void  searchForNameTwice(char* searchName, struct WordNode* linkedList, struct WordNode* hashTable[], int comparisonCount[2]);
void freeAll(WordNode* head);


 // Define a struct called WordNode
struct WordNode {
	char *name;
	struct WordNode* next;
};



int main()
{
	// a hash table 
	struct WordNode* hashTable[MAX_HASH];
	// a standlone linked list
	struct WordNode* head = NULL;
	char stringName[STRING_LENGTH] = { 0 };
	// a file pointer
	FILE* fp = NULL;
	unsigned long numberHash = 0;
	int i = 0;
	int j = 0;
	int compare = 1;

	// Use loop to assign NULL to 127 pointers
	for (i = 0; i < MAX_HASH; i++)
	{
		hashTable[i] = NULL;
	}

	// Open myTextFile.txt for reading
	fp = fopen("names.txt", "r");

	// Check if any error during opening file 
	if (fp == NULL)
	{
		printf("Can't open names.txt\n");
		return -1;
	}
	// If file is opened sucessfully
	else
	{
		// Use while loop to read the name of other files.
		while (fgets(stringName, sizeof(stringName), fp) != 0)
		{
			int returnValue = 0;

			// Get red of the newline character
			stringName[strcspn(stringName, "\n")] = 0;

			// Call myHashFunction and assign its return value to numberHash variable
			numberHash = myHashFunction(stringName);

			// Insert a name into a linked list
			returnValue = insert(&hashTable[numberHash], stringName);

			// Check if the allocation of newBlock runs out of memnory
			if (returnValue == -1)
			{
				printf("Error! No more memory!\n");
				return -1;
			}

			// Insert a name into a linked list
			returnValue = insert(&head, stringName);

			// Check if the allocation of newBlock runs out of memnory
			if (returnValue == -1)
			{
				printf("Error! No more memory!\n");
				return -1;
			}

			// Check error
			if (ferror(fp))
			{
				// there's an error
				printf("Error reading line from "
					"file\n");

				// try to close the file, since we're done with it
				if (fclose(fp) != 0)
				{
					// we can't even close the file
					printf("Can't close file opened "
						"for reading\n");
				}
				return -2;
			}
			
		}
	}

	// Check closing the file.
	if (fclose(fp) != 0)
	{
		// If it fails to close file, displaying error message.
		printf("Error closing file\n");

		return -2;
	}

	printf("Search for names. Input '.' to exit the loop.\n\n");

	int search = 0;
	int sumLinkedList = 0;
	int sumHashTable = 0;

	while (compare)
	{
		
		int comparisonCount[2] = { 0 };
		char inputName[STRING_LENGTH] = { 0 };

		// Get a name for searching from the user
		fgets(inputName, sizeof(inputName), stdin);

		// Get rid of newline character
		inputName[strcspn(inputName, "\n")] = '\0';

		// Check '.' character
		compare = strcmp(inputName, ".");

		// If user does not input '.' character
		if (compare != 0)
		{
			// Call searForNameTwice function 
			searchForNameTwice(inputName, head, hashTable, comparisonCount);

			// calculate the total number of comparisons in linked list
			sumLinkedList = sumLinkedList + comparisonCount[0];

			// calculate the total number of comparisons in hash table
			sumHashTable = sumHashTable + comparisonCount[1];

			// total searching
			search++;
		}
	}
	printf("\n\n\n");
	printf("\tTotal Number of Searches: %d\n", search);
	printf("\tTotal Number of Comparisons in Linked List: %d\n", sumLinkedList);
	printf("\tTotal Number of Comparisons in Hash Table: %d\n", sumHashTable);

	// Free all the memory of hash table
	for (j = 0; j < MAX_HASH; j++)
	{
		freeAll(hashTable[j]);
		hashTable[j] = NULL;
	}

	// Free all the memory of the standalone linked list
	freeAll(head);
	head = NULL;

	return 0;
}


/*
 * Author: Dan Bernstein
 * Function: hash()
 * Description: This function will calculate a string and return a number (unsigned long)
 * Parameters:  char *firstName: string containing a name
 * Returns: unsigned long
*/
unsigned long hash(char* str)
{
	unsigned long hash = 5381;
	int c = 0;
	while ((c = *str++) != '\0')
	{
		hash = ((hash << 5) + hash) + c;
	}
	return hash;
}


/*
 * Function: myHashFunction()
 * Description: This function will calculate a string and return a number that moduled to 11 (unsigned long)
 * Parameters:  char *firstName: string containing a name
 * Returns: unsigned long
*/
unsigned long myHashFunction(char* str)
{
	unsigned long number = 0;

	number = hash(str);

	return number % MAX_HASH;
}


/*
 * Function: insert()
 * Description: This function inserts an element in the correct location in the list, sorted alphabetically.
 * Parameters:  char *str: string containing the name
 *				WordNode **head: POINTERS TO head of the list
 * Returns: -1 if the insertion failed, 0 otherwise
*/
int insert(struct WordNode** head, char* str)
{
	WordNode* newBlock = NULL;
	WordNode* ptr = NULL;
	WordNode* prev = NULL;

	size_t maxLengthOfString = 0;

	maxLengthOfString = strlen(str) + 1;

	// allocate a block of memory for new record
	newBlock = (WordNode*)malloc(sizeof(WordNode));

	if (newBlock == NULL)
	{
		return -1;
	}

	// allocate a block of memory for new string
	newBlock->name = (char*)malloc(maxLengthOfString * sizeof(char));

	// Check if the allocation fails
	if (newBlock->name == NULL)
	{
		return -1;
	}

	strcpy(newBlock->name, str);

	newBlock->next = NULL;

	// now link this record to the list in the appropriate place
	if (*head == NULL)
	{
		// empty list, so set head
		*head = newBlock;
	}
	else if (strcmp((*head)->name, newBlock->name) >= 0) // special case
	{

		// location that used to be at the front of the list
		newBlock->next = *head;
		// set first_number to point to the new start of the list
		*head = newBlock;
	}
	else
	{
		prev = *head;		// first item in list
		ptr = (*head)->next;	// second item in list 

		while (ptr != NULL)
		{
			if (strcmp(ptr->name, newBlock->name) >= 0)
			{
				// we've found a name in the list that is either equal to or greater 
				// than the one we're entering now
				break;
			}
			prev = ptr;
			ptr = ptr->next;
		}	/* end while */

		// add the new node here, between prev and ptr
		newBlock->next = ptr;
		prev->next = newBlock;

	}
	return 0;
}


/*
* Function: WordNode* searchLinkedList()
* Description : This function will return NULL if a string is not found or it returns a
pointer to the node containing the string, if the string is matched
* Parameters : 3 parameter(WordNode *LinkedList: start of the linked list - char *searchName: pointer
to null-terminated string containing name - pointer to int filled in by the function with the
count of strcmp comparisons).
* Returns : A node if sucessfully, NULL otherwise
*/
struct WordNode* searchLinkedList(char* searchName, struct WordNode* linkedList, int* comparisonCount)
{
	WordNode* curr = NULL;
	curr = linkedList;
	int compare = 0;

	// Check if curr is not NULL
	while (curr != NULL)
	{
		// Increment comparisonCount by 1
		(*comparisonCount)++;
		
		compare = strcmp(curr->name, searchName);

		// Found the name return this node
		if (compare == 0)
		{
			return curr;
		}
		
		// If string is lager than searchName, then break
		if (compare > 0)
		{
			return NULL;
		}
		
		curr = curr->next;
	}

	return NULL;
}

/*
* Function: WordNode* searchForNameTwice()
* Description : This function will call linked list search functions. It then displays one of the messages 
once the search is done.
* Parameters : 4 parameter(WordNode *LinkedList: start of the linked list - char *searchName: pointer
to null-terminated string containing name - WordNode* hashTable[]: hash table to search - int comparisonCount[2]: 
the count of strcmp comparisons done in searching the extremely-long sorted linked list (element 0) and in searching
the hash table (element 1).
* Returns : void
*/
void  searchForNameTwice(char* searchName, struct WordNode* linkedList, struct WordNode* hashTable[], int comparisonCount[2])
{
	struct WordNode* foundNode = NULL;
	struct WordNode* linkedListNode = NULL;
	unsigned long numberHash = 0;

	// Search in the linked list
	foundNode = searchLinkedList(searchName, linkedList, &comparisonCount[0]);

	// If foundNode is not NULL, the string is found
	if (foundNode != NULL)
	{
		printf("\t%s was found in the linked list in "
			"%d comparisons\n", foundNode->name, comparisonCount[0]);
	}
	else
	{
		printf("\t%s was NOT found in the linked list in "
			"%d comparisons\n", searchName, comparisonCount[0]);
	}

	numberHash = myHashFunction(searchName);

	// search in a linked list of hash table
	linkedListNode = searchLinkedList(searchName, hashTable[numberHash], &comparisonCount[1]);

	// If foundNode is not NULL, the string is found
	if (linkedListNode != NULL)
	{
		printf("\t%s was found in the hash table bucket in "
			"%d comparisons\n", linkedListNode->name, comparisonCount[1]);
	}
	else
	{
		printf("\t%s was NOT found in the hash table bucket in "
			"%d comparisons\n", searchName, comparisonCount[1]);
	}

}

/*
 * Function: freeAll(WordNode *head)
 * Description: This function will free all allocated memory in the linked list.
 * Parameter: WordNode *head(a double pointer): start of the linked list
 * Returns: void
 */
void freeAll(WordNode* head)
{
	WordNode* curr = NULL;
	WordNode* next = NULL;

	curr = head;

	// traverse the list, being careful to not access freed blocks
	while (curr != NULL)
	{
		// keep a pointer to the next block so we can go there after it's freed
		next = curr->next;
		free(curr->name);
		free(curr);
		curr = next;
	}

}