#include <stdio.h>
#include <string.h>
#include "Global.h"
#include "html_conv.h"


static char* res_kwords_data[] = {"const", "volatile", "extern", "auto", "register",
   						   "static", "signed", "unsigned", "short", "long",
						   "double", "char", "int", "float", "struct",
						   "union", "enum", "void", "typedef",
						  };

static char* res_kwords_non_data[] = {"goto", "return", "continue", "break",
							   "if", "else", "for", "while", "do",
							   "switch", "case", "default","sizeof", "NULL"
							  };

/*to hold one byte of data*/
char byte;

void insert_header()
{
    fprintf(fptr_dest, "<!DOCTYPE html>\n");
    fprintf(fptr_dest, "<html lang=\"en-US\">\n");
    fprintf(fptr_dest, "<head>\n");
    fprintf(fptr_dest, "<title>%s</title>\n", "code2html");
    fprintf(fptr_dest, "<meta charset=\"UTF-8\">\n");
    fprintf(fptr_dest, "<link rel=\"stylesheet\" href=\"styles.css\">\n");
    fprintf(fptr_dest, "</head>\n");
    fprintf(fptr_dest, "<body>\n");
    fprintf(fptr_dest, "<pre>\n");
}

void insert_footer()
{
    fprintf(fptr_dest, "</pre>\n");
	  fprintf(fptr_dest, "</body>\n");
	  fprintf(fptr_dest, "</html>\n");
}

int html_conv()
{
    /*saperate all data in another file*/
    saperator();

    /*open temp file in read mode*/
    fptr_temp = fopen(TEMP,"r");

    get_data();

}

void get_data()
{
      while( (byte = fgetc(fptr_temp)) != EOF )
      {
          switch(byte)
          {
              case '/':/*handle single line or multiline comment*/
                      single_multiline_comment();
                      break;
              case '#':/*insert preprocessor*/
                    preprocesor_insert();
                    break;
              case '0'...'9':constant_insert();
                    break;
              case 'a' ... 'z': get_new_line_word();
                    break;
              case '(':fprintf(fptr_dest, "%c",byte);
                    break;
              case ')':fprintf(fptr_dest, "%c",byte);
                    break;
              case '{':fprintf(fptr_dest,"%c\n",byte);
                    break;
              case '}':fprintf(fptr_dest,"\n%c",byte);
                    break;
              case ';':fprintf(fptr_dest,"%c",byte);
                    break;
              case '\"':insert_string();
                    break;
              case '+':fprintf(fptr_dest,"%c",byte);
                    break;
              case '-':fprintf(fptr_dest,"%c",byte);
                    break;
              case '=':fprintf(fptr_dest,"%c",byte);
                    break;
              case '\n':byte = fgetc(fptr_temp);
                      if( byte == '\n')
                      fprintf(fptr_dest,"%c",byte);
                      else
                      fseek(fptr_temp,-1L,SEEK_CUR);
                            break;
              case ' ':fprintf(fptr_dest,"%c",byte);
                    break;
              case '&':fprintf(fptr_dest,"%c",byte);
                    break;
              case ',':fprintf(fptr_dest,"%c",byte);
                    break;
              case '*':fprintf(fptr_dest,"%c",byte);
                    break;
              case '\t':fprintf(fptr_dest,"%c",byte);
                    break;
          }
      }
}

int saperator()
{
    /*open a temp file*/
    fptr_temp = fopen(TEMP,"w");

    if( fptr_temp == NULL)
    {
        perror("");
        printf("Unable to open temp file\n");
        return 3;
    }

    while( ( byte = fgetc(fptr_src)) != EOF )
    {
        /*get each byte check and insert in file*/
        switch(byte)
        {
            case '/':/*single line or multiline comment*/
                    single_multiline_commnet_handle();
                    break;
            case '#':preprocessor_handle();
                    break;
            case '<':fprintf(fptr_temp,"%s\n","&lt;");
                    break;
            case '>':fprintf(fptr_temp,"%s\n","&gt;");
                    break;
            case '\n':fprintf(fptr_temp,"%c",'\n');
                    break;
            case '(':fprintf(fptr_temp,"%c\n",'(');
                    break;
            case ')':fprintf(fptr_temp,"%c\n",')');
                    break;
            case ' ':fprintf(fptr_temp,"%c",' ');
                    break;
            case 'a' ... 'z':get_word();
                    break;
            case '\"':get_string();
                    break;
            default:fprintf(fptr_temp,"%c\n",byte);
                    break;
        }
    }

    /*reading done close source and temp file*/
    fclose(fptr_temp);
    fclose(fptr_src);

}

void get_string()
{
    fprintf(fptr_temp,"%c",byte);
    byte = fgetc(fptr_src);
    while( byte != '\"' )
    {
        fprintf(fptr_temp,"%c",byte);
        byte = fgetc(fptr_src);
    }
    fprintf(fptr_temp,"%s","\"\n");
}

void get_word()
{
    while( byte != ' ' && byte != '>' && byte != '+' && byte != '-' && byte != '(' && byte != ';')
    {
        fprintf(fptr_temp,"%c",byte);
        byte = fgetc(fptr_src);
    }
    fseek(fptr_src,-1L,SEEK_CUR);
    fprintf(fptr_temp,"%c",'\n');
}

void preprocessor_handle()
{
    while( byte != ' ' && byte != '<' && byte != '\"')
    {
        fprintf(fptr_temp,"%c",byte);
        byte = fgetc(fptr_src);
    }
    fprintf(fptr_temp,"%c",'\n');
    fseek(fptr_src,-1L,SEEK_CUR);
}

void single_multiline_commnet_handle()
{
      byte = fgetc(fptr_src);

      /*single line comment*/
      if( byte == '/')
      {
          /*move pointer two times back*/
          fseek(fptr_src,-2L,SEEK_CUR);
          /*get all char upto \n*/
          while( (byte = fgetc(fptr_src)) != '\n')
          fprintf(fptr_temp,"%c",byte);
          /*lastly insert new line*/
          fprintf(fptr_temp,"%c",byte);
      }
      /*multi line comment*/
      else if( byte == '*')
      {
          fprintf(fptr_temp,"%s","/*");

          while( ( byte = fgetc(fptr_src) ) != EOF )
          {
              if( byte == '*' )
              {
                  byte = fgetc(fptr_src);
                  if( byte == '/' )
                  break;
                  else
                  fprintf(fptr_temp,"%c",byte);
              }
              else
              fprintf(fptr_temp,"%c",byte);
          }

          fprintf(fptr_temp,"%s\n","*/");
      }
}

void single_multiline_comment()
{
    char buff[1000];
    int i = 0;

    byte = fgetc(fptr_src);
    /*single line comment*/
    if( byte == '/')
    {
      /*move pointer two times back*/
      fseek(fptr_src,-2L,SEEK_CUR);
      /*get all char upto \n*/
      while( (byte = fgetc(fptr_src)) != '\n')
      buff[i++] = byte;
      /*lastly insert new line*/
      buff[i] = '\0';
      fprintf(fptr_dest,"<span class=\"comment\">%s</span>\n",buff);
  }
  /*multi line comment*/
  else if( byte == '*')
  {
      while( ( byte = fgetc(fptr_src) ) != EOF )
      {
          if( byte == '*' )
          {
              byte = fgetc(fptr_src);
              if( byte == '/' )
              break;
              else
              buff[i++] = byte;
          }
          else
          buff[i++] = byte;
      }

      buff[i] = '\0';
      fprintf(fptr_dest,"<span class=\"comment\">/*%s*/</span>\n",buff);
  }
}

void preprocesor_insert()
{
    char str[1000];
    int i = 0, count = 0;
    while( (byte = fgetc(fptr_temp))!= '\n')
    {
        str[i++] = byte;
    }

    str[i] = '\0';
    fprintf(fptr_dest,"<span class=\"preprocess_dir\">#%s</span>",str);

    byte = fgetc(fptr_temp);

    if( strcmp(str,"include") == 0 )
    {
        /*header file*/
        str[0] = '\0';
        i = 0;
        while( count < 3 )
        {
            if( byte == '\n' || byte == '\"')
            count++;
            else
            str[i++] = byte;

            byte = fgetc(fptr_temp);
        }
        str[i] = '\0';
        fprintf(fptr_dest,"<span class=\"header_file\">%s</span>\n",str);
    }
}

void constant_insert()
{
    fprintf(fptr_dest,"<span class=\"numeric_constant\">%c</span>",byte);
}

void get_new_line_word()
{
    int i = 0, lock = 0;
    char buff[1000];
    fseek(fptr_temp,-1L,SEEK_CUR);
    while( (byte = fgetc(fptr_temp)) != '\n' )
    {
        buff[i++] = byte;
    }
    buff[i] = '\0';
    //printf("buff %s\n",buff);
    i = 0;
    while( res_kwords_data[i] != NULL)
    {
        if( strcmp(buff,res_kwords_data[i++]) == 0 )
        {
        fprintf(fptr_dest,"<span class=\"reserved_key1\">%s</span>",buff);
        lock = 1;
        }
    }
    if( lock == 0 )
    i = 0;
    while(res_kwords_non_data[i] != NULL )
    {
        if( strcmp(buff,res_kwords_non_data[i++]) == 0 )
        {
        fprintf(fptr_dest,"<span class=\"reserved_key2\">%s</span>",buff);
        lock = 1;
        }
    }

    if( lock == 0)
        fprintf(fptr_dest,"%s",buff);

}


void insert_string()
{
    int i = 0;
    char buff[1000];

    while( ( byte = fgetc(fptr_temp)) != '\"')
    {
          buff[i++] = byte;
    }

    buff[i] = '\0';

    fprintf(fptr_dest,"<span class=\"string\">\"%s\"</span>",buff);
}
