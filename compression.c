#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compression.h"

const char com_table_chars[COM_TABLE_SIZE] = { ' ', 'e', 't', 'o', 'i', 'a', 
    'n', 's', '\?', 'r', 'h', 'l', 'c', 'd', 'u', 'p', 'f', 'm', 'g', ',', 'w',
    'y', 'b', '.', '-', 'T', 'H', 'O', '1', 'V', 'I', 'C', ':', ';', 'P', 'A',
    'S', 'N', '(', ')' };

const int com_table_freqs[COM_TABLE_SIZE] = { 2000, 700, 600, 550, 500, 480, 
    450, 440, 400, 380, 270, 260, 250, 200, 180, 150, 140, 100, 95, 90, 88, 87,
    81, 78, 51, 48, 46, 45, 43, 42, 41, 40, 38, 37, 36, 35, 34, 33, 32, 31 };

#ifdef _DEBUG
/*******************************************************************************
 * strrev
 * This function reverses a string. Used for debug output of Huffman codes.
 * inputs: 
 *  str: string to reverse (read/write)
 * outputs:
 *  writes reversed string to str
 * Author: Miles Burchell
*******************************************************************************/
void strrev(char *str)
{
    int i, len = strlen(str);
    char* rev = (char*)calloc(len + 1, sizeof(char));

    if (!rev)
    {
        printf("Error: strrev calloc failed.\n");
    }

    for (i = 0; i < len; i++)
    {
        rev[i] = str[len - i - 1];
    }

    strcpy(str, rev);

    free(rev);
}
#endif

/*******************************************************************************
 * com_bitstream_writebit
 * This function writes an individual bit to a data stream
 * inputs: 
 *  stream: pointer to com_bitstream_t structure describing data stream
 *  bit: value to write (1 or 0)
 * outputs: 
 *  none
 * Author: Miles Burchell
*******************************************************************************/
void com_bitstream_writebit(com_bitstream_t* stream, char bit)
{
    unsigned char mask;

    if (!stream)
    {
        return;
    }

    if ((bit < 0) || (bit > 1))
    {
        return;
    }

    if (stream->bit > 7)
    {
        stream->byte++;
        stream->bit = 0;
    }

    mask = (128 >> stream->bit);

    if (!bit)
    {
        mask = ~ mask;
    }

    stream->buffer[stream->byte] & mask;

    stream->bit++;
}

/*******************************************************************************
 * com_bitstream_readbit
 * This function reads an individual bit from a data stream
 * inputs: 
 *  stream: pointer to com_bitstream_t structure describing data stream 
 *  (read/write)
 * outputs: 
 *  returns the value of the bit (1 or 0), or -1 if the function fails.
 * Author: Miles Burchell
*******************************************************************************/
char com_bitstream_readbit(com_bitstream_t* stream)
{
    unsigned char mask, read;

    if (!stream)
    {
        return -1;
    }

    if (stream->bit > 7)
    {
        stream->byte++;
        stream->bit = 0;
    }

    mask = (128 >> stream->bit);

    read = mask & stream->buffer[stream->byte];

    stream->bit++;

    return read != 0;
}

/*******************************************************************************
 * com_bitstream_read
 * This function reads a series of bits from a data stream
 * inputs: 
 *  stream: pointer to com_bitstream_t structure describing data stream
 *  buffer: char array to write output to (1 or 0 as char) (write)
 *  length: number of bits to read
 * outputs:
 *  writes to buffer
 *  returns 0 if the function succeeds, 1 if it fails
 * Author: Miles Burchell
*******************************************************************************/
int com_bitstream_read(com_bitstream_t* stream, char* buffer, size_t length)
{
    size_t s;

    if (!buffer || !length || !stream)
    {
        return 1;
    }

    for (s = 0; s < length; s++)
    {
        buffer[s] = com_bitstream_readbit(stream);
    }

    return 0;
}

/*******************************************************************************
 * com_buildtree
 * This function builds the Huffman tree from predetermined static tables.
 * inputs: 
 *  tree_p_out: pointer to write heap pointer block to (write)
 * outputs:
 *  returns numbers of elements in the heap, or 0 if failed
 * Author: Miles Burchell
*******************************************************************************/
int com_buildtree(com_huffnode_t*** tree_p_out)
{
    #ifdef _DEBUG
    printf("DEBUG: com_buildtree: Computing static huffman tree.... :)\n");
    #endif

    int i, heapsize = 0;
    com_huffnode_t** heap;
    com_huffnode_t *lowest, *secondlowest, *high;

    /* create heap of node pointers ready for linkin' */
    heap = (com_huffnode_t**)calloc(COM_HEAP_SIZE, sizeof(void*));

    /* create our dummy node with maximum frequency for later */
    high = (com_huffnode_t*)calloc(1, sizeof(com_huffnode_t));
    high->frequency = 0x7fffffff;

    /* add all our finish nodes (ie. actual codes) from the table */
    for (i = 0; i < COM_TABLE_SIZE; i++)
    {
        heap[i] = (com_huffnode_t*)calloc(1, sizeof(com_huffnode_t));
        heap[i]->character = com_table_chars[i];
        heap[i]->frequency = com_table_freqs[i];
    }

    heapsize = COM_TABLE_SIZE;

    /* iterate, finding the two lowest frequencies and building the tree up
       from there */
    while (1)
    {
        /* set lowest and secondlowest to point to our dummy with max freq */
        lowest = secondlowest = high;

        /* find the lowest and 2nd lowest frequency nodes that haven't already
           been built on */
        for (i = 0; i < heapsize; i++)
        {
            /* check if node has been built on, if so skip it */
            if (heap[i]->up)
            {
                continue;
            }

            if (heap[i]->frequency < secondlowest->frequency)
            {
                /* lower than our 2nd lowest */

                if (heap[i]->frequency < lowest->frequency)
                {
                    /* lower than our lowest: new lowest */
                    /* and set the old lowest to the new second lowest */
                    secondlowest = lowest;
                    lowest = heap[i];
                }
                else
                {
                    /* not lower than our lowest: new 2nd lowest */
                    secondlowest = heap[i];
                }
            }
        }

        if (lowest->frequency == 0x7fffffff || 
            secondlowest->frequency == 0x7fffffff)
        {
            #ifdef _DEBUG
            printf("Huffman tree finished, used %d nodes.\n", heapsize);
            int total = 0;

            for (i = 0; i < COM_TABLE_SIZE; i++)
            {
                total += com_table_freqs[i];
            }

            /* Check that the frequency of the top of the tree is equal to
               the sum of all frequencies (it should be!) */
            printf("Total of all freqs is %d, freq of top node is %d.\n", 
                total, heap[heapsize - 1]->frequency);
            #endif

            break;
        }

        /* build a new node on top of the two lowest!
           imagine our tree starts as left highest freq, right lowest freq.
           so we should put the 2nd lowest on the left and lowest on the right
           link of the new node. */
        
        if (heapsize == COM_HEAP_SIZE)
        {
            printf("Error: com_buildtree: Ran out of Huffman tree heap!!\n");

            *tree_p_out = 0;
            return 0;
        }

        #ifdef _DEBUG
        printf("Lowest nodes are %c (%d freq) and %c (%d freq), linking them.\n",
            secondlowest->character, secondlowest->frequency, 
            lowest->character, lowest->frequency);
        #endif

        heap[heapsize] = (com_huffnode_t*)calloc(1, sizeof(com_huffnode_t));
        heap[heapsize]->character = '\"'; /* represents a non-coding node */
        heap[heapsize]->left = secondlowest;
        heap[heapsize]->right = lowest;
        heap[heapsize]->frequency = lowest->frequency + secondlowest->frequency;

        lowest->up = heap[heapsize];
        lowest->updir = 1;
        secondlowest->up = heap[heapsize];
        secondlowest->updir = 0;

        heapsize++;
    }

    #ifdef _DEBUG
    /* printf out all huff codes */
    printf("DEBUG: Printing out all huffman codes:\n");

    com_huffnode_t* node;
    char buffer[32];
    char c;
    int bits, j;

    for (i = 0; i < COM_TABLE_SIZE; i++)
    {
        memset(buffer, 0, 32);

        node = heap[i];
        bits = 0;

        c = node->character;

        /* we're going in reverse */
        while(node->up)
        {
            sprintf(&buffer[bits], "%d", (int)node->updir);
            bits++;
            node = node->up;
        }
        
        strrev(buffer);

        printf("Character %c code %s\n", c, buffer);
    }

    /* test to find which character 111011 is 
       remember, 1 is right 0 is left */
    node = heap[heapsize - 1];

    printf("Traversal test: 110111 is character %c\n", 
        node->right->right->left->right->right->right->character);
    /*        1      1      0     1      1      1        */
    #endif

    /* attempt to resize heap pointer block to reduce memory usage */
    com_huffnode_t** reheap = 0;
    reheap = (com_huffnode_t**)realloc(heap, sizeof(void*) * heapsize);

    if (reheap)
    {
        heap = reheap;
    }
    #ifdef _DEBUG
    else
    {
        printf("com_buildtree: couldn't realloc heap pointer block! :(\n");
    }
    #endif

    *tree_p_out = heap;
    return heapsize;
}

/*******************************************************************************
 * com_freetree
 * This function frees all memory allocated to a huffman tree and its pointer
 * block.
 * inputs: 
 *  tree: tree pointer block
 *  treesize: number of elements in tree pointer block
 * outputs: 
 *  none
 * Author: Miles Burchell
*******************************************************************************/
void com_freetree(com_huffnode_t** tree, int treesize)
{
    int i;

    for (i = 0; i < treesize; i++)
    {
        free(tree[i]);
    }

    free(tree);
}

/*******************************************************************************
 * com_getnode
 * This function finds the Huffman node for a specific character
 * inputs: 
 *  character: character to get node for
 *  tree: the Huffman tree pointer block
 * outputs: 
 *  returns pointer to node, or 0 if node does not exist
 * Author: Miles Burchell
*******************************************************************************/
com_huffnode_t* com_getnode(char character, com_huffnode_t** tree)
{
    /* ? is reserved for our "uncoded data" flag */
    if (character == '\?')
    {
        return 0;
    }

    int i;

    for (i = 0; i < COM_TABLE_SIZE; i++)
    {
        if (character == com_table_chars[i])
        {
            return tree[i];
        }
    }

    return 0;
}

/*******************************************************************************
 * com_decompressfile
 * This function decompresses a file loaded into memory.
 * inputs: 
 *  file: pointer to dat_file_t structure
 * outputs: 
 *  return 0 if successful, 1 if failed.
 * Author: Miles Burchell
*******************************************************************************/
int com_decompressfile(dat_file_t* file)
{
    if (!file->compressed)
    {
        printf("Error: Trying to decompress a decompressed file.\n");

        return 1;
    }

    if (file->encrypted)
    {
        printf("Error: Trying to decompress an encrypted file.\n");

        return 1;
    }

    /* get a huffman tree */
    com_huffnode_t** tree = 0;
    int treesize = 0;
    
    treesize = com_buildtree(&tree);

    if (!treesize)
    {
        return 1;
    }

    /* */

    /* dispose of the huffman tree */
    com_freetree(tree, treesize);

    return 0;
}

/*******************************************************************************
 * com_compressfile
 * This function compresses a file loaded into memory.
 * inputs: 
 *  file: pointer to dat_file_t structure
 * outputs: 
 *  return 0 if successful, 1 if failed.
 * Author: Miles Burchell
*******************************************************************************/
int com_compressfile(dat_file_t* file)
{
    if (file->compressed)
    {
        printf("Error: Trying to compress a compressed file.\n");

        return 1;
    }

    if (file->encrypted)
    {
        printf("Error: Trying to compress an encrypted file.\n");

        return 1;
    }

    /* get a huffman tree */
    com_huffnode_t** tree = 0;
    int treesize = 0;
    
    treesize = com_buildtree(&tree);

    if (!treesize)
    {
        return 1;
    }

    char* currentchar = file->data;
    int i, comlen = 0, origlen = 0;
    com_huffnode_t* node = 0;

    printf("Testing compression on data \'%s\'\n", (char*)file->data);

    printf("Original data:\n");

    com_bitstream_t bitreader;
    bitreader.buffer = file->data;
    bitreader.byte = 0;
    bitreader.bit = 0;

    for (i = 0; i < (file->length * 8); i++)
    {
        printf("%d", (int)com_bitstream_readbit(&bitreader));

        if ((i % 8) == 7)
        {
            printf(" ");
        }
    }

    printf("\nCompressed data:\n");

    for (i = 0; i < file->length; i++)
    {
        origlen += 8;

        node = com_getnode(*currentchar, tree);

        currentchar++;

        if (!node)
        {
            comlen += 8;
            continue;
        }

        while (node)
        {
            printf("%d", (int)node->updir);
            node = node->up;

            if ((comlen % 8) == 7)
            {
                printf (" ");
            }

            comlen++;
        }
    }

    int rem = 8 - (comlen % 8);

    for (i = 0; i < rem; i++)
    {
        printf("0");
        comlen++;
    }

    printf("\nOriginal: %d bytes. Compressed: %d bytes (padded to 8 bit).\n", 
        origlen / 8, comlen / 8);

    /* dispose of the huffman tree */
    com_freetree(tree, treesize);

    return 0;
}
