#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#define VERSION_DESC    "@Copyright 2021 - MIT License Custom head software module. Developed by Batuhan Düzgün\n"

#define HELP_DESC       "****** HELP DOCUMENT *******\n" \
                        "-c ya da --bytes: Dosyanın başından itibaren kaç byte'ın yazdırılacağını belirtir.\n" \
                        "-n ya da --lines: Dosyanın başından itibaren kaç satırın yazdırılacağını belirtir (default 10).\n" \
                        "-v ya da --verbose: Dosyaların isimlerini de yazdırır. İsim aşağıdaki örnekte olduğu gibi yazdırılmalıdır:\n" \
                        "                     \n" \
                        "==> sample.c <==\n" \
                        "                     \n" \
                        "--help: Komut hakkında açıklamayı ekrana yazdırır.\n" \
                        "--version: Copyright bilgisini ve versiyon numarasını yazdırır. (Versiyon numarasını 1.0 alabilirsiniz.)\n"

void print_lines(int argc, char *argv[], int line_count, int v_flag);
void print_chars(int argc, char *argv[], int byte_count, int v_flag);

int main(int argc, char *argv[])
{
    int result;
    int line_count = 0;
    int char_count = 0;
    int error_flag = 0;
    int c_flag, n_flag, v_flag, help_flag, version_flag;
    char *c_arg, *n_arg;
    
    struct option options[] = {
        { "bytes", required_argument, NULL, 'c' },
        { "lines", required_argument, NULL, 'n' },
        { "verbose", no_argument, NULL, 'v' },
        { "help", no_argument, NULL, 1 },
        { "version", no_argument, NULL, 2 },
        { 0, 0, 0, 0 }
    };

    // suppress library errors!
    opterr = 0;
    
    c_flag = n_flag = v_flag = help_flag = version_flag = 0;

    while( (result = getopt_long(argc, argv, "c:n:v", options, NULL)) != -1 )
    {
        switch(result)
        {
            case 'c':
                c_flag = 1;
                c_arg = optarg;
                break;
            case 'n':
                n_flag = 1;
                n_arg = optarg;
                break;
            case 'v':
                v_flag = 1;
                break;
            case 1:
                help_flag = 1;
                break;
            case 2:
                version_flag = 1;
                break;
            case '?':
                if(optopt == 'c')
                {
                    fprintf(stderr, "-c or --bytes option must have an argument!...\n");
                }
                else if(optopt == 'n')
                {
                    fprintf(stderr, "-n or --lines option must have an argument!...\n");
                }
                else if(optopt != 0)
                {
                    fprintf(stderr, "invalid option -%c\n", optopt);
                }
                else
                {
                    fprintf(stderr, "invalid long option!...\n");
                }

                error_flag = 1;
                break;
        }
    }

    if(error_flag)
    {
        exit(EXIT_FAILURE);
    }

    if(argc - optind == 0)
    {
        fprintf(stderr, "specify file names!...\n");
        exit(EXIT_FAILURE);
    }

    if((c_flag + n_flag + v_flag) && (argc - optind == 0))
    {
        fprintf(stderr, "specify file names!...\n");
        exit(EXIT_FAILURE);
    }

    if(c_flag && (n_flag + help_flag + version_flag) > 0 )
    {
        fprintf(stderr, "-c  or --bytes option can be used only with -v or --verbose option together...\n");
        exit(EXIT_FAILURE);
    }

    if(n_flag && (c_flag + help_flag + version_flag) > 0 )
    {
        fprintf(stderr, "-n or --lines option can be used only with -v or --verbose option together...\n");
        exit(EXIT_FAILURE);
    }

    if(v_flag && (help_flag + version_flag) > 0 )
    {
        fprintf(stderr, "-v or --verbose option can be used with (-n / --lines) or (-c / --bytes) option together...\n");
        exit(EXIT_FAILURE);
    }

    if(help_flag && ((n_flag + c_flag + v_flag + version_flag) > 0 || (argc - optind > 0)) )
    {
        fprintf(stderr, "--help cannot be used with another option...\n");
        exit(EXIT_FAILURE);   
    }

    if(version_flag && ((n_flag + c_flag + v_flag + help_flag) > 0 || (argc - optind > 0)) )
    {
        fprintf(stderr, "--version cannot be used with another option...\n");
        exit(EXIT_FAILURE);  
    }



    if(c_flag)
    {
        char_count = atoi(c_arg);
    }

    if(n_flag)
    {
        line_count = atoi(n_arg);
    }

    if( (optind == 1 && argc == optind) || (v_flag && !n_flag && !c_flag && (argc - optind > 0)) )
    {
        n_flag = 1;
        v_flag = 1;
        line_count = 10;
    }

    if( !(v_flag + n_flag + c_flag + help_flag + version_flag) && (argc - optind > 0) )
    {
        n_flag = 1;
        v_flag = 1;
        line_count = 10;
    }

    if(n_flag)
    {
        print_lines(argc, argv, line_count, v_flag);
    }

    if(c_flag)
    {
        print_chars(argc, argv, char_count, v_flag);
    }

    if(help_flag)
    {
        printf("%s\n", HELP_DESC);
    }

    if(version_flag)
    {
        printf("%s\n", VERSION_DESC);
    }

    return 0;
}

void print_lines(int argc, char *argv[], int line_count, int v_flag)
{
    FILE *f;
    int line_counter;

    for (int i = optind; i < argc; i++)
    {
        line_counter = 0;

        if(v_flag)
        {
            printf("==> %s <==\n", argv[i]);
        }

        if ((f = fopen(argv[i], "r")) == NULL) 
        {   
            fprintf(stderr, "file %s cannot open!..\n", argv[i]);
            continue;
        }

        fseek(f, 0, SEEK_END);
        int fpos = ftell(f);

        int val;
        while( (line_counter < line_count) && fpos)
        {
            if (ferror(f)) 
            {
                fprintf(stderr, "value cannot read in file %s \n", argv[i]);
                break;
            }

            fseek(f, --fpos, SEEK_SET);
            val = fgetc(f);
            if(val == '\n')
            {
                line_counter++;
            }
        }

        int v;
        while( (v = fgetc(f)) != EOF )
        {
            if (ferror(f)) 
            {
                fprintf(stderr, "value cannot read in file %s \n", argv[i]);
                break;
            }
            putchar(v);            
        }
        printf("\n");

        fclose(f);
    }
}

void print_chars(int argc, char *argv[], int char_count, int v_flag)
{
    FILE *f;
    int char_counter;
    int tmp_char_count;

    for (int i = optind; i < argc; i++)
    {   
        char_counter = 0;
        tmp_char_count = char_count;

        if(v_flag)
        {
            printf("==> %s <==\n", argv[i]);
        }

        if ((f = fopen(argv[i], "r")) == NULL) 
        {
            fprintf(stderr, "file %s cannot open!..\n", argv[i]);
            continue;
        }

        fseek(f, 0, SEEK_END);
        int fpos = ftell(f);

        while( (char_counter < char_count) )
        {
            fseek(f, --fpos, SEEK_SET);
            ++char_counter;
        }

        int val;
        while( (val = fgetc(f)) != EOF )
        {
            if (ferror(f)) 
            {
                fprintf(stderr, "value cannot read in file %s \n", argv[i]);
                break;
            }
            putchar(val);            
        }
        fclose(f);
        printf("\n");
    }
}