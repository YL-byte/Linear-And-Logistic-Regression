#include<stdlib.h>
#include<stdio.h>
#include <unistd.h>

class File {
    public:
        FILE *fptr = NULL;
        char *file_name = NULL;
        char *path = NULL;

        File(char *Arr_file_name = NULL){
            fptr = NULL;
            file_name = Arr_file_name;
        }

        void getPath_(){
            int last_slash_index = 0;
            int i = 0;
            while(file_name[i] != '\0'){
                if(file_name[i] == '\\'){
                    last_slash_index = i;
                }
                i++;
            }
            path = (char*)malloc(sizeof(char) * (last_slash_index + 1));
            for (i = 0; i < last_slash_index; i++){
                path[i] = file_name[i];
            }
            path[last_slash_index] = 0;
        }

        void operator = (char Arr_file_name[]){
            int i = 0;
            file_name = (char *)malloc(sizeof(char));
            while (Arr_file_name[i] != 0){
                file_name = (char *)realloc(file_name, sizeof(char) * (i + 1));
                file_name[i] = Arr_file_name[i];
                i++;
            }
            //Add zero to the end of the string
            file_name = (char *)realloc(file_name, sizeof(char) * (i + 1));
            file_name[i] = '\0';
            if(fileExists() == false){
                createFile();
            }
        }

        void createFile(){
            //If file doesn't exist - create it
            if ((fptr = fopen(file_name, "r")) == NULL){
                printf("Creating file: %s.\n", file_name);
                fclose(fptr);
                fptr = fopen(file_name, "w");
                fclose(fptr);
            }
        }

        bool fileExists(){
            if ((fptr = fopen(file_name, "r")) == NULL) {
                printf("File not found.\n");
                fclose(fptr);
                return false;
            }
            return true;
        }

        void write(char text[]){
            int i = 0;
            fptr = fopen(file_name, "w");
            while(text[i] != 0) {
                fputc((int) *(text + i), fptr);
                i++;
            }
            fclose(fptr);
        }


        void append(char text[]){
            int i = 0;
            fptr = fopen(file_name, "a");
            while(text[i] != 0) {
                fputc((int) *(text + i), fptr);
                i++;
            }
            fclose(fptr);
        }

        void appendFromPointer(char *text){
            int i = 0;
            fptr = fopen(file_name, "a");
            while(text[i] != 0) {
                fputc((int) *(text + i), fptr);
                i++;
            }
            fclose(fptr);
        }

        void appendCharacter(char c){
            fptr = fopen(file_name, "a");
            fputc((int)c, fptr);
            fclose(fptr);
        }

        void write_binary(char text[]){
            int i = 0;
            fptr = fopen(file_name, "wb");
            while(text[i] != 0) {
                fputc((int) *(text + i), fptr);
                i++;
            }
            fclose(fptr);
        }

        char *read(){
            int i = 0;
            int character = 0;
            char *read_text = (char *)malloc(1);
            fptr = fopen(file_name, "r");
            while (character != EOF){
                character = fgetc(fptr);
                read_text = (char *)realloc(read_text, (i + 1) * sizeof(char));
                read_text[i] = (char)character;
                i++;
            }
            read_text = (char *)realloc(read_text, (i + 1) * sizeof(char));
            read_text[i] = '\0';
            fclose(fptr);
            return read_text;
        }

        char *read_binary(){
            int i = 0;
            int character = 0;
            char *read_text = (char *)malloc(1);
            fptr = fopen(file_name, "rb");
            while (character != EOF){
                character = fgetc(fptr);
                read_text = (char *)realloc(read_text, (i + 1) * sizeof(char));
                read_text[i] = (char)character;
                i++;
            }
            read_text = (char *)realloc(read_text, (i + 1) * sizeof(char));
            read_text[i] = '\0';
            fclose(fptr);
            return read_text;
        }

        void freeFile_(){
            fclose(fptr);
            free(file_name);
            free(path);
        }

};
