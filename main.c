#include <pcre.h>   /* PCRE lib        NONE  */
#include <stdio.h>  /* I/O lib         C89   */
#include <stdlib.h> /* Standard Lib    C89   */
#include <string.h> /* Strings         C89   */

/**********************************************************************************************************************************/

void handlePcreError(int pcreExecRet)
{
    switch (pcreExecRet)
    {
    case PCRE_ERROR_NOMATCH:
        // printf("String did not match the pattern\n");
        break;
    case PCRE_ERROR_NULL:
        printf("Something was null\n");
        break;
    case PCRE_ERROR_BADOPTION:
        printf("A bad option was passed\n");
        break;
    case PCRE_ERROR_BADMAGIC:
        printf("Magic number bad (compiled re corrupt?)\n");
        break;
    case PCRE_ERROR_UNKNOWN_NODE:
        printf("Something kooky in the compiled re\n");
        break;
    case PCRE_ERROR_NOMEMORY:
        printf("Ran out of memory\n");
        break;
    default:
        printf("Unknown error\n");
        break;
    }
}

pcre *compileRegex(const char *regexStr)
{
    const char *pcreErrorStr = NULL;
    int pcreErrorOffset = 0;
    pcre *reCompiled = pcre_compile(regexStr, 0, &pcreErrorStr, &pcreErrorOffset, NULL);
    if (reCompiled == NULL)
    {
        printf("ERROR: Could not compile '%s': %s\n", regexStr, pcreErrorStr);
        exit(EXIT_FAILURE);
    }
    return reCompiled;
}

pcre_extra *optimizeRegex(pcre *compiledRegex)
{
    const char *pcreErrorStr = NULL;
    pcre_extra *pcreExtra = pcre_study(compiledRegex, 0, &pcreErrorStr);
    if (pcreExtra == NULL && pcreErrorStr != NULL)
    {
        printf("ERROR: Could not optimize regex: %s\n", pcreErrorStr);
        exit(EXIT_FAILURE);
    }
    return pcreExtra;
}

#define MAX_NUM_OF_MATCH 100
#define MAX_NUM_OF_MATCH_VEC_ELEMS 300
int main(void)
{
    char* files[] =
    {
        "htmle/L_2005149PL.01002201.xml.html",
        "htmle/L_2007319PL.01000101.xml.html",
        "htmle/L_2009033PL.01001001.xml.html",
        "htmle/L_2010334PL.01000101.xml.html",  
        "htmle/L_2011055PL.01000101.xml.html",  
        "htmle/L_2011304PL.01006401.xml.html",  
        "htmle/L_2013165PL.01000101.xml.html",  
        "htmle/L_2015326PL.01000101.xml.html",
        "htmle/TEKST_skonsolidowany_31993L0013-PL-12.12.2011.html",
        "htmle/TEKST_skonsolidowany_31999L0044-PL-12.12.2011.html",
        "htmle/TEKST_skonsolidowany_32002L0065-PL-13.01.2018.html",
        "htmle/TEKST_skonsolidowany_32004R0261-PL-17.02.2005.html",
        NULL
    };
    

    int subStrVec[MAX_NUM_OF_MATCH_VEC_ELEMS] = {0};

    const char *regexStr = "([Aa]rtykuł \\d+)+|([Aa]rt\\. \\d+)+|([Pp]kt "
                           "\\d+)+|([Pp]unkt \\d+)+|([Uu]st\\. \\d+)+|([Uu]stęp \\d+)+";

    pcre *reCompiled = compileRegex(regexStr);
    pcre_extra *pcreExtra = optimizeRegex(reCompiled);
    char *lineBuffer = (char *)malloc(40960);
    memset(lineBuffer, 0, 40960);

    for(int y = 0; files[y] != NULL; y++)
    {
        const char* fileName = files[y];
        printf("%s\n", fileName);
        FILE *file = fopen(fileName, "r");
        if(!file)
        {
            perror("File opening failed");
            continue;
        }
        
        size_t lineNumber = 0;
        while (fgets(lineBuffer, 40960, file) != NULL)
        {
            lineNumber++;
            int lineLen = (int)strlen(lineBuffer);
            /* Try to find the regex in aLineToMatch, and report results. */
            int pcreExecRet = pcre_exec(reCompiled, pcreExtra, lineBuffer,
                                        lineLen, // length of string
                                        0,                       // Start looking at this point
                                        PCRE_NOTEMPTY,                       // OPTIONS
                                        subStrVec,
                                        MAX_NUM_OF_MATCH_VEC_ELEMS); // Length of subStrVec

            if (pcreExecRet < 0)
            {
                handlePcreError(pcreExecRet);
            }
            else
            {
                if (pcreExecRet == 0)
                {
                    pcreExecRet = MAX_NUM_OF_MATCH;
                }

                printf("%zu: ", lineNumber);
                for (int i = 0; i < pcreExecRet - 1; i++)
                {
                    const char *psubStrMatchStr = NULL;
                    pcre_get_substring(lineBuffer, subStrVec, pcreExecRet, i, &(psubStrMatchStr));
                    printf("%s ", psubStrMatchStr);
                    pcre_free_substring(psubStrMatchStr);
                }

                for(;;)
                {
                    int options = 0;
                    int startOffset = subStrVec[1];
                    if(subStrVec[0] == subStrVec[1])
                    {
                        if(subStrVec[0] == lineLen) break;
                        options = PCRE_NOTEMPTY_ATSTART | PCRE_ANCHORED;
                    }
                    pcreExecRet = pcre_exec(reCompiled, pcreExtra, lineBuffer,
                                        lineLen, // length of string
                                        startOffset,                       // Start looking at this point
                                        PCRE_NOTEMPTY | options,                       // OPTIONS
                                        subStrVec,
                                        MAX_NUM_OF_MATCH_VEC_ELEMS); // Length of subStrVec
                    if(pcreExecRet == PCRE_ERROR_NOMATCH)
                    {
                        break;
                    }
                    if(pcreExecRet == 0)
                    {
                        pcreExecRet = MAX_NUM_OF_MATCH;
                    }
                    if(pcreExecRet < 0)
                    {
                        handlePcreError(pcreExecRet);
                        break;
                    }
                    for(int i = 0; i < pcreExecRet -1; i++)
                    {
                        const char *psubStrMatchStr = NULL;
                        pcre_get_substring(lineBuffer, subStrVec, pcreExecRet, i, &(psubStrMatchStr));
                        printf("%s ", psubStrMatchStr);
                        pcre_free_substring(psubStrMatchStr);
                    }
                }
                printf("\n");
            }
        }

        if (!feof(file))
        {
            perror("Problem while reading input file");
        }
        printf("\n");
        fclose(file);
    }

    pcre_free(reCompiled);
    if (pcreExtra != NULL)
    {
#ifdef PCRE_CONFIG_JIT
        pcre_free_study(pcreExtra);
#else
        pcre_free(pcreExtra);
#endif
    }
    free(lineBuffer);

    exit(EXIT_SUCCESS);
}
