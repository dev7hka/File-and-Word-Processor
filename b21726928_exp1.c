#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAXLINELENGTH 200


typedef struct Database {
    int id;
    char *name;
    char *text;
    struct Database *next;
}Database;
char *fgets_ne(char *line, int n, FILE *stream)
{
    char *temp;
    temp = fgets(line, n, stream);
    if(!temp)
        return(temp);

    if(line[strlen(line) - 1] == '\n')
        line[strlen(line) - 1] = 0;

    return(line);
}

int Analyzer(char *line, Database **Data, int *id, FILE *out_ptr);
int Create(Database *Data, char *name, char *text, int *id, FILE *out_ptr);
int Print(Database *Data, char **line_tokens, int tokens_len, FILE *out_ptr);
Database * Delete(char *name, Database **Data, FILE *out_ptr);
int Remove(Database *Data, int tokens_len, char *name, char **line_tokens, FILE *out_ptr);
int Append(Database *Data, char *name, char *text, FILE *out_ptr);
int Replace(Database *Data, char *name, char *old_word, char *new_word, FILE *out_ptr);


int main(int argc, char *argv[]) {

    if (argc != 2){
        printf("Command is wrong !!! Just one file name must be written !!!");
        return 0;
    }

    FILE *file_ptr = fopen(argv[1], "r");
    FILE *out_ptr = fopen("output.txt", "w");

    if (file_ptr == NULL ){
        printf("File not Found !\n");
        return 0;
    }
    if (out_ptr == NULL){
        printf("Output file couldn't opened !!!\n");
    }

    Database *Data = malloc(sizeof(Database));

    Data->next = malloc(sizeof(Database));

    Data->id = 0;

    int id = 1;
    char **lines = malloc(sizeof(char *) * 100);
    int line_number;

    char *temp = malloc(sizeof(char ) * MAXLINELENGTH);
    int i = 0;
    (temp = fgets_ne(temp, MAXLINELENGTH, file_ptr));
    while (temp != NULL){

        char tmp[strlen(temp)];
        strcpy(tmp, temp);
        fprintf(out_ptr, "%s\n",tmp);
        Analyzer(tmp, &Data, &id, out_ptr);
        temp = fgets_ne(temp, MAXLINELENGTH, file_ptr);
        i++;

    }


}

char ** line_take(FILE *file_ptr, char **lines, int *line_number){

    char *temp = malloc(sizeof(char ) * MAXLINELENGTH);
    int i = 0;
    (temp = fgets_ne(temp, MAXLINELENGTH, file_ptr));
    while (temp != NULL){

        char tmp[strlen(temp)];
        strcpy(tmp, temp);
        temp = fgets_ne(temp, MAXLINELENGTH, file_ptr);
        i++;

    }
    *line_number = i;
    return lines;

}
int Analyzer(char *line, Database **permanent_data, int *id, FILE *out_ptr){

    Database *temp = *permanent_data;
    Database *Data = *permanent_data;
    if(line == NULL) {
        printf("Input Memory Problem has occured !\n");
        return 0;
    }
    char **line_tokens = malloc(sizeof(char *) * strlen(line)) ;

    char *funcptr = NULL;
    funcptr = strtok(line, " ");
    int i = 0;
    while (funcptr != NULL) {
        line_tokens[i] = malloc(strlen(funcptr)+1);
        strcpy(line_tokens[i], funcptr);
        funcptr = strtok(NULL, " ");
        i++;

    }
    int j = 0;
    char *name = malloc(sizeof(char ) * 20);
    char *text = malloc(sizeof(char ) * 20);
    char *old_word = malloc(sizeof(char ) * 20);
    char *new_word = malloc(sizeof(char ) * 20);
    strcpy(name, "null");
    strcpy(text, "Empty File");
    strcpy(old_word, "null");
    strcpy(new_word, "null");

    for (; j < i; j++) {
        if (strcmp("-n", *(line_tokens + j)) == 0) {
            free(name);
            name = malloc(strlen(*(line_tokens + j + 1)));  // j + 1 might be out of memory handle it
            strcpy(name, *(line_tokens + j + 1));
        }
        if (strcmp("-t", *(line_tokens + j)) == 0 && (strcmp(*line_tokens,"create") == 0 || strcmp(*line_tokens,"append") == 0 )) {

            int k  = j + 1;
            int len_text = 0;

            while ( k < i && strcmp(*(line_tokens+k), "-n") != 0) {
                len_text += strlen(*(line_tokens+k));
                k++;
            }
            k = j + 1;
            char *temp_text = malloc(sizeof(char) * len_text + 1);
            strcpy(temp_text, *(line_tokens + k));
            k++;

            for ( ; k < i && strcmp(*(line_tokens+k), "-n")!= 0 ; k++) {
                strcat(temp_text, " ");
                strcat(temp_text, *(line_tokens +k));
            }
            text = realloc(text,strlen(temp_text));
            strcpy(text, temp_text);
        }
        if (strcmp("-ow", *(line_tokens + j)) == 0 && strcmp(*(line_tokens + j +1),"-nw") != 0 && strcmp(*(line_tokens + j +1),"-n") != 0) {
            strcpy(old_word, *(line_tokens + j + 1));
        }
        if (strcmp("-nw", *(line_tokens + j)) == 0 && strcmp(*(line_tokens + j +1),"-ow") != 0 && strcmp(*(line_tokens + j +1),"-n") != 0) {
            strcpy(new_word, *(line_tokens + j + 1));
        }
    }
    if (strcmp(name, "null") == 0 && strcmp(*line_tokens, "print") != 0) {
        fprintf(out_ptr, "no file name is given ! \n");
        return 0;
    }
    int file_id = -1;
    while (temp != NULL) {
        if (temp->id == 0) {
            break;
        }
        else if (strcmp(temp->name, name) == 0) {
            file_id = temp->id;
            break;
        }
        else
            temp = temp->next;
    }
    if (strcmp(*line_tokens, "create") == 0) {
        if (file_id == -1)
            Create(Data, name, text, id, out_ptr);
        else
            fprintf(out_ptr, "A file already exists by this name !!!\n");
    }
    else if (strcmp(*line_tokens, "print") == 0)
        Print(Data, line_tokens, i, out_ptr);
    else {
        if (file_id == -1)
            fprintf(out_ptr, "There is no file by this name !!!\n");
        else if (strcmp(*line_tokens, "delete") == 0)
            Delete(name, permanent_data, out_ptr);
        else if (strcmp(*line_tokens, "remove") == 0)
            Remove(Data, i, name, line_tokens, out_ptr);
        else if (strcmp(*line_tokens, "append") == 0)
            Append(Data, name, text, out_ptr);
        else if (strcmp(*line_tokens, "replace") == 0)
            Replace(Data, name, old_word, new_word, out_ptr);
        else
            fprintf(out_ptr, "Wrong User Input !!! \n");  // specialize to say which line

    }
    free(line_tokens);free(text);free(name);free(old_word);free(new_word);

}

int Create(Database *Data, char *name, char *text, int *id, FILE *out_ptr)
{
    while (1){
        if (Data->id == 0 ){
            Data->name = malloc(strlen(name) + 1);
            Data->text = malloc(strlen(text) + 1);
            strcpy(Data->name, name);
            strcpy(Data->text, text);
            Data->id = *id;
            *id += 1 ;
            Data->next = malloc(sizeof(Database));
            Data = Data->next;
            Data->name = malloc(sizeof(char));
            Data->text = malloc(sizeof(char));
            Data->id = 0;

            break;
        }
        else
            Data = Data->next;

    }

}

int Print(Database *Data, char **line_tokens, int tokens_len, FILE *out_ptr) {

    int name = -1, text = -1, all = -1, ext = -1, cont = -1, cw = -1, cs = -1;
    int i = 0;
    for (; i < tokens_len; ++i) {
        if (strcmp(*(line_tokens + i), "-n") == 0)
            name = i + 1;
        if (strcmp(*(line_tokens + i), "-t") == 0)
            text = i;
        if (strcmp(*(line_tokens + i), "-a") == 0)
            all = i + 1;
        if (strcmp(*(line_tokens + i), "-e") == 0)
            ext = i + 1;
        if (strcmp(*(line_tokens + i), "-c") == 0)
            cont = i + 1;
        if (strcmp(*(line_tokens + i), "-cw") == 0)
            cw = i + 1;
        if (strcmp(*(line_tokens + i), "-cs") == 0)
            cs = i + 1;
    }

    if (all != -1) {
        if (tokens_len != 2){
            fprintf(out_ptr, "Wrorng input is given !!\n");
            return 0 ;
        }
        i = 0;
        while (Data != NULL) {
            if (Data->id != 0) {
                fprintf(out_ptr, "Filename %d: %s\n", Data->id, Data->name);
                i++;
            }
            Data = Data->next;
        }
    } else if (ext != -1) {
        if (tokens_len != 3){
            fprintf(out_ptr, "Wrorng input is given !!\n");
            return 0 ;
        }
        while (Data != NULL) {
            if (Data->id != 0) {
                char *data_name = malloc(sizeof(char) * strlen(Data->name));
                strcpy(data_name, Data->name);
                char *str_tok = strtok(data_name, ".");
                char *temp_name = malloc(sizeof(char) * strlen(str_tok));
                strcpy(temp_name, str_tok);
                str_tok = strtok(NULL, ".");
                if (strcmp(str_tok, *(line_tokens + ext)) == 0)
                    fprintf(out_ptr, "Filename %d: %s\nText: %s\n", Data->id, temp_name, Data->text);

                Data = Data->next;
            } else
                break;
        }
    } else if (cont != -1) {
        if (tokens_len != 2){
            fprintf(out_ptr, "Wrorng input is given !!\n");
            return 0 ;
        }
        int idd = Data->id;
        while (Data != NULL && Data->id != 0) {

            fprintf(out_ptr, "Num: %d\nName: %s\nText: %s\n", Data->id, Data->name, Data->text);

            Data = Data->next;

        }

    } else if (name != -1 && text != -1) {
        if (tokens_len != 4){
            fprintf(out_ptr, "Wrorng input is given !!\n");
            return 0 ;
        }
        int result = 0;
        while (Data != NULL) {
            if (Data->id != 0 && strcmp(Data->name, *(line_tokens + name)) == 0) {
                fprintf(out_ptr, "Text: %s\n", Data->text);
                result = 1;
                break;
            }
            Data = Data->next;
        }
        if (result == 0)
            fprintf(out_ptr, "There is no file by this name !!!\n");
    } else if (name != -1 && cw != -1) {
        if (tokens_len != 5){
            fprintf(out_ptr, "Wrorng input is given !!\n");
            return 0 ;
        }
        int result = 0;
        while (Data != NULL) {

            if (Data->id != 0 && strcmp(Data->name, *(line_tokens + name)) == 0) {
                result = 1;
                break;
            }
            Data = Data->next;
        }
        if (result == 0) {
            fprintf(out_ptr, "There is no file by this name !!!\n");
            return 0;
        }

        int occur = 0;

        char *temp_text = strstr(Data->text, *(line_tokens + cw));

        while (temp_text  != NULL){

            occur +=1;
            temp_text += strlen(*(line_tokens + cw));
            temp_text = strstr(temp_text, *(line_tokens + cw));
        }
        fprintf(out_ptr,"Text: %s\n",Data->text);
        fprintf(out_ptr, "Number of occurrence of '%s' : %d\n",*(line_tokens+cw), occur);

    }
    else if (name != -1 && cs != -1){
        if (tokens_len != 4){
            fprintf(out_ptr, "Wrorng input is given !!\n");
            return 0 ;
        }
        int result = 0;
        while (Data != NULL) {

            if (Data->id != 0 && strcmp(Data->name, *(line_tokens + name)) == 0) {
                result = 1;
                break;
            }
            Data = Data->next;
        }
        if (result == 0) {
            fprintf(out_ptr, "There is no file by this name !!!");
            return 0;
        }
        char *punc = ".!?";
        char *temp_text = malloc(strlen(Data->text) * sizeof(char));
        strcpy(temp_text, Data->text);
        temp_text = strtok(temp_text, punc);
        int occur = 0;
        while (temp_text  != NULL) {
            temp_text = strtok(NULL, punc);
            occur +=1;

        }
        if (Data->text[strlen(Data->text) -1 ] == '.' ||Data->text[strlen(Data->text) -1 ] == '!' ||Data->text[strlen(Data->text) -1 ] == '?')
            occur += 1;
        fprintf(out_ptr, "Number of Sentence in '%s'  is %d\n" , Data->name, occur -1 );
    }
}
Database * Delete(char *name, Database **permanent_data, FILE *out_ptr) {

    Database *temp_data = *permanent_data;
    Database *Data = *permanent_data;
    int result = 0;
    while (Data != NULL) {

        if (Data->id != 0 && strcmp(Data->name, name) == 0) {
            result = 1;
            break;
        }
        temp_data = Data;
        Data = Data->next;
    }
    if (result == 0) {
        fprintf(out_ptr, "There is no file by this name !!!");
        return 0;
    }

    if (temp_data == Data){
        Data->id = 0;
        temp_data = temp_data->next;
        free(Data);
        *permanent_data = temp_data;
        return 0;
    }
    else{
        temp_data->next = Data->next;
        free(Data);
        return 0;
    }

}



int Remove(Database *Data, int tokens_len, char *name, char **line_tokens, FILE *out_ptr){


    int i = 0 ;
    long rmv_start = 0, rmv_length = 0;
    for (; i < tokens_len; ++i){
        if (strcmp(*(line_tokens+i),"-s") == 0) {
            rmv_start = atoi(*(line_tokens + i + 1));
        }
        if (strcmp(*(line_tokens+i),"-l") == 0)
            rmv_length = strtol(*(line_tokens + i + 1),NULL,10);
    }
    if (rmv_start == 0 || rmv_length == 0) {
        fprintf(out_ptr, "The input is not enough to Remove !!! \n");
        return 0;
    }
    int result = 0;
    while (Data != NULL) {

        if (Data->id != 0 && strcmp(Data->name, name) == 0) {
            result = 1;
            break;
        }
        Data = Data->next;              // Revise it later we check if name exists before in analyze !!!
    }
    if (result == 0)
        fprintf(out_ptr, "There is no file by this name !!!\n");
    else
    {

        char *new_text = (char *) malloc(strlen(Data->text) - rmv_length);
        int k = 0, l = 0;

        for (; k < strlen(Data->text) ; k++) {
            if (k < rmv_start || k >= rmv_start+rmv_length  ) {
                *(new_text + l) = *(Data->text + k);
                l++;
            }

        }
        Data->text = realloc(Data->text, strlen(new_text) * sizeof(char));
        strcpy(Data->text, new_text);

    }

}


int Append(Database *Data, char *name, char *text, FILE *out_ptr){

    if (strcmp(text , "Empty File") == 0) {
        fprintf(out_ptr, "No Text is given to append !!!\n");
        return 0;
    }
    int result = 0;
    while (Data != NULL && Data->id != 0) {

        if (strcmp(Data->name, name) == 0) {
            result = 1;
            break;
        }
        Data = Data->next;
    }
    if (result == 0) {
        fprintf(out_ptr, "There is no file by this name !!!\n");
        return 0;
    }

    char *new_text = malloc(strlen(text)+ strlen(Data->text));
    strcpy(new_text,Data->text);
    strcat(new_text,text);
    Data->text = realloc(Data->text, strlen(new_text));
    strcpy(Data->text, new_text);

}


int Replace(Database *Data, char *name, char *old_word, char *new_word, FILE *out_ptr) {

    if (strcmp("null", old_word) == 0 || strcmp("null", new_word) == 0) {
        fprintf(out_ptr, "Wrong input is given !!! \n");
        return 0;
    }
    int result = 0;
    while (Data != NULL) {

        if (Data->id != 0 && strcmp(Data->name, name) == 0) {
            result = 1;
            break;
        }
        Data = Data->next;
    }
    if (result == 0) {
        fprintf(out_ptr, "There is no file by this name !!!\n");
        return 0;
    }

    char *text = malloc(strlen(Data->text));
    char *new_text = malloc(strlen(Data->text));

    strcpy(text, Data->text);

    char *tok = strstr(text, old_word);
    if (tok == NULL)
        fprintf(out_ptr, "no word is found !! \n");
    else {
        strncpy(new_text, text, tok - text);
        strcat(new_text, new_word);
        tok += strlen(old_word);
        text = tok;
        tok = strstr(tok, old_word);
        if (tok == NULL);
        else {
            while (tok != NULL) {

                strncat(new_text, text, tok - text);
                strcat(new_text, new_word);
                tok += strlen(old_word);
                text = tok;
                tok = strstr(tok, old_word);
            }
        }
    }
    strcat(new_text,text);
    Data->text = realloc(Data->text, strlen(new_text) * sizeof(char));
    strcpy(Data->text,new_text);
}
