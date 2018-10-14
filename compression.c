#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compression.h"

/*******************************************************************************
 * strrev
 * This function reverses a string.
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

/*******************************************************************************
 * com_bitstream_init
 * This function initialises a bit stream
 * inputs: 
 *  stream: pointer to com_bitstream_t structure describing data stream
 *  (read/write)
 *  data: pointer to data for the stream to read from/write to.
 * outputs:
 *  none
 * Author: Miles Burchell
*******************************************************************************/
void com_bitstream_init(com_bitstream_t* stream, const void* data)
{
    stream->bit = 0;
    stream->byte = 0;
    stream->buffer = (char*)data;
}

/*******************************************************************************
 * com_bitstream_rewind
 * This function rewinds a bit stream by one bit.
 * inputs: 
 *  stream: pointer to com_bitstream_t structure describing data stream
 *  (read/write)
 * outputs:
 *  none
 * Author: Miles Burchell
*******************************************************************************/
void com_bitstream_rewind(com_bitstream_t* stream)
{
    if (!stream)
    {
        return;
    }

    if (!stream->bit)
    {
        stream->bit = BITS_IN_BYTE - 1;
        stream->byte--;
        return;
    }

    stream->bit--;
}

/*******************************************************************************
 * com_bitstream_skip
 * This function skips a bit stream to the start of the next byte.
 * inputs: 
 *  stream: pointer to com_bitstream_t structure describing data stream
 *  (read/write)
 * outputs:
 *  returns number of bits skipped (or -1 if failed).
 * Author: Miles Burchell
*******************************************************************************/
int com_bitstream_skip(com_bitstream_t* stream)
{
    if (!stream)
    {
        return -1;
    }

    if (!stream->bit)
    {
        return 0;
    }

    int n = 8 - stream->bit;

    stream->byte++;
    stream->bit = 0;

    return n;
}

/*******************************************************************************
 * com_bitstream_writebit
 * This function writes an individual bit to a data stream
 * inputs: 
 *  stream: pointer to com_bitstream_t structure describing data stream
 *  (read/write)
 *  bit: value to write (1 or 0)
 * outputs: 
 *  none
 * Author: Miles Burchell
*******************************************************************************/
void com_bitstream_writebit(com_bitstream_t* stream, char bit)
{
    unsigned char shift;

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

    shift = BITS_IN_BYTE - stream->bit - 1;

    if (bit)
    {
        stream->buffer[stream->byte] |= (1 << shift);
    }
    else
    {
        stream->buffer[stream->byte] &= ~(1 << shift);
    }

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
 * com_bitstream_writechar
 * This function writes an entire char to the data stream
 * inputs: 
 *  stream: pointer to com_bitstream_t structure describing data stream 
 *  (read/write)
 * outputs: 
 *  none
 * Author: Miles Burchell
*******************************************************************************/
void com_bitstream_writechar(com_bitstream_t* stream, char data)
{
    if (!stream)
    {
        return;
    }

    if (stream->bit)
    {
        printf("Error: com_bitstream_writechar: Not a whole byte.\n");
        return;
    }

    com_bitstream_skip(stream);

    stream->buffer[stream->byte] = data;
    stream->byte++;
}

/*******************************************************************************
 * com_bitstream_readchar
 * This function reads an entire char from a data stream
 * inputs: 
 *  stream: pointer to com_bitstream_t structure describing data stream 
 *  (read/write)
 * outputs: 
 *  returns the value of the char read, or 0 if the function fails (read data
 *  can also be 0!)
 * Author: Miles Burchell
*******************************************************************************/
char com_bitstream_readchar(com_bitstream_t* stream)
{
    if (!stream)
    {
        return 0;
    }

    if (stream->bit)
    {
        printf("Error: com_bitstream_readchar: Not a whole byte.\n");
        return 0;
    }

    char c = stream->buffer[stream->byte];

    stream->byte++;

    return c;
}

/*******************************************************************************
 * com_bitstream_writecom
 * This function writes a series of bits representing a huffman coded character
 * to a data stream.
 * inputs: 
 *  stream: pointer to com_bitstream_t structure describing data stream
 *  (read/write)
 *  node: huffman node representing character to write
 * outputs:
 *  returns number of bits written, or 0 if failed.
 * Author: Miles Burchell
*******************************************************************************/
int com_bitstream_writehuff(com_bitstream_t* stream, com_huffnode_t* node)
{
    if (!stream || !node)
    {
        return 0;
    }

    int i;

    for (i = 0; i < node->enc_length; i++)
    {
        com_bitstream_writebit(stream, node->enc_bits[i]);
    }

    return node->enc_length;
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
    printf("DEBUG: com_buildtree: Computing static huffman tree....\n");
    #endif

    /* Static frequency data for huffman tree generation. \? is used to represent
       the start of plain data (not coded.) */

    const char com_table_chars[COM_TABLE_SIZE] = { ' ', 'e', 't', 'o', 'i', 'a', 
    'n', 's', '\?', 'r', 'h', 'l', 'c', 'd', 'u', 'p', 'f', 'm', 'g', ',', 'w',
    'y', 'b', '.', '-', 'T', 'H', 'O', '1', 'V', 'I', 'C', ':', ';', 'P', 'A',
    '\n', '\r', 'S', 'N', 'z' };

    const int com_table_freqs[COM_TABLE_SIZE] = { 2000, 700, 600, 550, 500, 480, 
    450, 440, 400, 380, 270, 260, 250, 200, 180, 150, 140, 100, 95, 90, 88, 87,
    81, 78, 51, 48, 46, 45, 43, 42, 41, 40, 38, 37, 36, 35, 34, 33, 32, 31, 30};

    char buffer[32], c;
    int bits, i, j, heapsize = 0;
    com_huffnode_t *node, *lowest, *secondlowest, *high, **heap;

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
        printf("Lowest nodes are %c (%d f) and %c (%d f), linking them.\n",
            secondlowest->character, secondlowest->frequency, 
            lowest->character, lowest->frequency);
        #endif

        heap[heapsize] = (com_huffnode_t*)calloc(1, sizeof(com_huffnode_t));
        heap[heapsize]->character = '\0';
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
    #endif

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

        node = heap[i];
        
        strrev(buffer);

        /* set the bits in node struct */
        node->enc_length = bits;

        for (j = 0; j < bits; j++)
        {
            if (buffer[j] == '0')
            {
                node->enc_bits[j] = 0;
            }
            else if (buffer[j] == '1')
            {
                node->enc_bits[j] = 1;
            }
            else
            {
                printf("ERROR: com_buildtree: invalid binary character\n");
                node->enc_bits[j] = 0;
            }
        }

        #ifdef _DEBUG
        printf("Character %c code %s\n", c, buffer);
        #endif
    }

    #ifdef _DEBUG
    /* test to find which character 111011 is 
       remember, 1 is right 0 is left */
    node = heap[heapsize - 1];

    printf("Traversal test: 11010 is character %c\n", 
        node->right->right->left->right->left->character);
    /*        1      1      0     1      0       */
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
    const char com_table_chars[COM_TABLE_SIZE] = { ' ', 'e', 't', 'o', 'i', 'a', 
    'n', 's', '\?', 'r', 'h', 'l', 'c', 'd', 'u', 'p', 'f', 'm', 'g', ',', 'w',
    'y', 'b', '.', '-', 'T', 'H', 'O', '1', 'V', 'I', 'C', ':', ';', 'P', 'A',
    '\n', '\r', 'S', 'N', 'z' };

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
        printf("Error: com_decompressfile: Non-compressed file.\n");

        return 1;
    }

    if (file->encrypted)
    {
        printf("Error: com_decompressfile: Encrypted file.\n");

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

    /* local variables for compression algorithm */
    char bit, nlen = 0, noncode = '\?';
    com_huffnode_t * node = 0, * topnode;
    com_bitstream_t bitreader;
    int i;

    char* buffer;
    int buf_pos = 0;

    /* Allocate memory buffer for decompressed data */
    buffer = (char*)calloc(sizeof(char), (file->length * 2) + 512);

    /* Initialise bitreader */
    com_bitstream_init(&bitreader, file->data);

    /* Last added node is the top of the tree. */
    topnode = tree[treesize - 1];

    /* Sanity check */
    if (!topnode || !topnode->left || !topnode->right)
    {
        printf("Error: com_decompressfile: Invalid top node.\n");
        return 1;
    }

    /* Start from the top */
    node = topnode;

    /* Start reading from the input buffer, bit by bit by bit */
    while (bitreader.byte < file->length)
    {
        bit = com_bitstream_readbit(&bitreader);

        /* Traverse tree */
        if (bit)
        {
            node = node->right; /* right: 1 */
        }
        else
        {
            node = node->left;  /* left: 0  */
        }

        if (!node)
        {
            printf("Error: com_decompressfile: Null node pointer.\n");
            printf("%s\n", buffer);
            return 1;
        }

        if (node->character != '\0')
        {
            /* We've hit a character */
            if (node->character == noncode)
            {
                /* 'noncoded incoming' escape char, proceed with processing
                   noncoded data. First skip to end of byte. */
                com_bitstream_skip(&bitreader);

                /* read nlen (represents number of non-coded chars) */
                nlen = com_bitstream_readchar(&bitreader);

                /* nlen = 0: raw data until EOF. */
                if (nlen == 0)
                {
                    for (i = 0; i < (file->length - bitreader.byte); i++)
                    {
                        buffer[buf_pos] = com_bitstream_readchar(&bitreader);

                        buf_pos++;
                    }
                }
                else
                {
                    /* otherwise, copy nlen chars */
                    for (i = 0; i < nlen; i++)
                    {
                        buffer[buf_pos] = com_bitstream_readchar(&bitreader);

                        buf_pos++;
                    }
                }

                node = topnode;
            }
            else
            {
                /* Standard character. Output char. */
                buffer[buf_pos] = node->character;

                buf_pos++;

                node = topnode;
            }
        }
    }

    /* free original data buffer */
    free(file->data);

    /* null terminate our output */
    buffer[buf_pos] = '\0';

    /* attempt realloc our buffer to a sensible size */
    void* realloc_buffer = realloc(buffer, buf_pos + 1);

    if (realloc_buffer)
    {
        file->data = realloc_buffer;
    }
    else
    {
        file->data = buffer;
    }

    /* set new file length, and decompressed attribute */
    file->length = buf_pos - 1;
    file->compressed = FALSE;

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
        printf("Error: com_compressfile: Compressed file.\n");

        return 1;
    }

    if (file->encrypted)
    {
        printf("Error: com_compressfile: Encrypted file.\n");

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

    /* local variables for comrpession algorithm */
    char c, nlen = 0, noncode = '\?';
    int i, j;
    com_huffnode_t* node = 0;
    com_bitstream_t bitwriter;
    void* filebuffer;
    char* buffer = (char*)file->data;

    /* Allocate memory buffer for compressed data (adds 512 bytes for breathing
       room, we will realloc this later anyway) */
    filebuffer = calloc(sizeof(char), file->length + 512);

    /* Initialise bitwriter */
    com_bitstream_init(&bitwriter, filebuffer);

    #ifdef _DEBUG
    /*printf("DEBUG: Testing compression on data \'%s\'\n", (char*)file->data);
    printf("\nCompressed data:\n");*/
    #endif

    /* loop through all chars and perform compression */
    i = 0;

    printf("buffer: %s\n", buffer);
    printf("length: %d\n", file->length);

    while (i < file->length)
    {
        c = buffer[i];

        node = com_getnode(c, tree);

        /* We can't let '/?' be treated as a normal character */
        if (c == noncode)
        {
            node = 0;
        }

        /* noncoded character, use escape char ('/?') followed by nlen - char
           specifying length of noncoded data to follow (positive number)
           or number of times to repeat the following char (negative number) */
        if (!node)
        {
            /* first write our escape char in Huff code */
            node = com_getnode(noncode, tree);

            com_bitstream_writehuff(&bitwriter, node);

            /* next finish the current byte */
            com_bitstream_skip(&bitwriter);

            /* next determine length of uncompressed data (nlen) */
            nlen = 1;

            while (1)
            {
                c = buffer[i + nlen];

                if ((i + nlen) == file->length)
                {
                    /* 0 nlen: raw data until EOF. */
                    nlen = 0;
                    break;
                }

                if (c == noncode || !com_getnode(c, tree))
                {
                    nlen++;
                }
                else
                {
                    break;
                }
            }

            /* write nlen */
            com_bitstream_writechar(&bitwriter, nlen);

            #ifdef _DEBUG
            printf("Adding raw data length %d\n", (int)nlen);
            #endif

            /* if 0 nlen write everything else we got */
            if (nlen == 0)
            {
                for (j = 0; j < (file->length - bitwriter.byte); j++)
                {
                    com_bitstream_writechar(&bitwriter, buffer[i]);
                    i++;
                }
            }
            else if (nlen > 0)
            {
                /* write nlen bytes of  uncompressed data */
                for (j = 0; j < nlen; j++)
                {
                    com_bitstream_writechar(&bitwriter, buffer[i]);
                    i++;
                }
            }
        }
        /* Not special char: simply write compressed data */
        else
        {
            com_bitstream_writehuff(&bitwriter, node);
            i++;
        }
    }

    /* Finish up the last byte if we haven't already */
    com_bitstream_skip(&bitwriter);

    #ifdef _DEBUG
    /* Read compressed data and output as binary (debug) */
    com_bitstream_t bitreader;
    bitreader.buffer = bitwriter.buffer;
    bitreader.byte = 0;
    bitreader.bit = 0;
    double comp_ratio = 0.0;

    for (i = 0; i < (bitwriter.byte * 8); i++)
    {
        printf("%d", (int)com_bitstream_readbit(&bitreader));

        if ((i % 8) == 7)
        {
            printf(" ");
        }
    }

    /* Write compression ratio */
    printf("\nOriginal: %d bytes. Compressed: %d bytes (padded to 8 bits).\n", 
        file->length, bitwriter.byte);
    
    comp_ratio = (double)file->length / (double)bitwriter.byte;

    printf("Compression ratio: %0.2lf:1\n", comp_ratio);
    #endif

    /* free original data buffer */
    free(file->data);

    /* attempt realloc our buffer to a sensible size */
    void* realloc_buffer = realloc(filebuffer, bitwriter.byte + 1);

    if (realloc_buffer)
    {
        file->data = realloc_buffer;
    }
    else
    {
        file->data = filebuffer;
    }

    /* set new file length, and compressed attribute */
    file->length = bitwriter.byte;
    file->compressed = TRUE;

    /* dispose of the huffman tree */
    com_freetree(tree, treesize);

    return 0;
}
