#include"file.cpp"

float string_to_float(char *s);
char **parse_string(char *s, char seperators[], int *size_address);
bool value_contained_in_list(char c, char l[]);
float *parsed_string_to_float_vector(char **parsed_string, int *number_of_strings);
float *read_table(char reading_filename[], char seperators[]);

float *read_table(char reading_filename[], char seperators[], int *number_of_strings){
    int character = 0;
    int i = 0;
    File F;
    F = reading_filename;
    char *read_text = F.read();
    char **parsed_text = parse_string(read_text, seperators, number_of_strings);
    float *l = parsed_string_to_float_vector(parsed_text, number_of_strings);

    F.freeFile_();
    free(read_text);
    for (int i = 0; i < *number_of_strings; i++){
        free(parsed_text[i]);
    }

    free(parsed_text);
    return l;
}

float *parsed_string_to_float_vector(char **parsed_string, int *number_of_strings){
    float *l = (float*)malloc(sizeof(float) * *number_of_strings);
    for (int word_index = 0; word_index < *number_of_strings; word_index++){
        l[word_index] = string_to_float(parsed_string[word_index]);
    }
    return l;
}

char **parse_string(char *s, char seperators[], int *size_address = NULL){
    char **parsed_string = (char**)malloc(sizeof(char*) * 1);
    parsed_string[0] = (char*)malloc(sizeof(char));
    int i = 0, word_index = 0, letter_index = 0;
    bool finished = false;
    while (!finished){
        if(s[i] == 0){
            finished = true;
        }
        if (value_contained_in_list(s[i], seperators) || s[i] == 0){
            parsed_string[word_index] = (char*)realloc(parsed_string[word_index], sizeof(char) * (letter_index + 1));
            parsed_string[word_index][letter_index] = 0;
            letter_index = 0;
            word_index++;
            parsed_string = (char**)realloc(parsed_string, sizeof(char*) * (word_index + 1));
            parsed_string[word_index] = (char*)malloc(sizeof(char));
            parsed_string[word_index] = parsed_string[word_index];
        }
        else{
            parsed_string[word_index] = (char*)realloc(parsed_string[word_index], sizeof(char) * (letter_index + 1));
            parsed_string[word_index][letter_index] = s[i];
            letter_index++;
        }
        i++;
    }
    //Send size of parsed string back to the user via the pointer
    if (size_address != NULL){
        *size_address = word_index;;
    }
    return parsed_string;
}

bool value_contained_in_list(char c, char l[]){
    int i = 0;
    while (l[i] != 0){
        if (c == l[i]){
            return true;
        }
        i++;
    }
    return false;
}

float string_to_float(char *s){
    float value = 0;
    bool after_decimal_point = false;
    int decimal_power = 10;
    int i = 0;
    bool is_negative = false;
    if (s[i] == '-'){
        is_negative = true;
    }
    while (s[i] != 0){
        if((s[i] == '.') && (after_decimal_point == false)){
            after_decimal_point = true;
        }
        else if ((s[i] >= '0') && (s[i] <= '9') && (after_decimal_point == false)){
            value *= 10;
            value += s[i] - '0';
        }
        else if ((s[i] >= '0') && (s[i] <= '9') && (after_decimal_point == true)){
            float add_decimel = (s[i] - '0');
            add_decimel /= decimal_power;
            decimal_power *= 10;
            value += add_decimel;
        }
        i++;
    }
    if(is_negative){
        value *= -1;
    }
    return value;
}

char *float_to_string(float f, int how_much_after_decimal = 0){
    char *c = (char*)malloc(sizeof(char) * 1);
    c[0] = 0;
    bool is_negative = false;
    if (f < 0){
        is_negative = true;
        f *= -1;
    }
    for (int i = 0; i < how_much_after_decimal; i++){
        f *= 10;
    }
    int number = (int)f;
    int char_counter = 1;
    int i = 0;
    while (number / 10 != 0){
        c = (char*)realloc(c, sizeof(char) * (char_counter + 1));
        c[i] = c[i - 1];
        if(char_counter - 1 == how_much_after_decimal){
            c[i - 1] = '.';
        }
        else{
            c[i - 1] = (number % 10) + '0';
        }
        char_counter++;
        number /= 10;
    }
    if (is_negative){
        c = (char*)realloc(c, sizeof(char) * (char_counter + 1));
        for (int i = 0; i < char_counter - 1; i++){
            c[i + 1] = c[i];
        }
        c[0] = '-';
    }
    return c;
}
