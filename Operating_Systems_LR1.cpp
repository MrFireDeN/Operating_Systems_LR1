#define _CRT_SECURE_NO_WARNINGS 
 
#include <stdio.h> 
#include <conio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <direct.h> 
#include <process.h> 
 
void interactive_mode(void); 
void error_message(const char* message); 
void input_string(const char* message, char* destination); 
void pack(char* filename);
void unpack(const char* zipname, char* dir); 
void crypt(char* filename); 
void uncrypt(char* filename); 
 
void main(int argc, char* argv[]) { 
 char filename[100]; 
 char zipname[100]; 
 char dir[100]; 
 
 // Анализируем параметры командной строки 
 // Первый параметр - всегда название ссамого запакуемого процесса 
 if (argc < 2) 
  // Пользователь не указал никаких параметров 
  interactive_mode(); 
 else 
  if (!strcmp(argv[1], "-p")) { //если указан первый флаг "-p", то запоковать 
   if (argc < 3) // название файла неуказано 
    input_string("Enter file name: ", filename); 
   else 
    strcpy(filename, argv[2]); 
   pack(filename); 
  } 
  else 
   if (!strcmp(argv[1], "-u")) { //если указан первый флаг "-u", то запоковать 
    if (argc < 3) // название архива не указано 
     input_string("Enter archive name: ", zipname); 
    else 
     strcpy(zipname, argv[2]); 
 
    if (argc < 4) // название директории не указано 
     input_string("Enter directory name: ", dir); 
    else 
     strcpy(dir, argv[3]); 
    unpack(zipname, dir); 
   } 
   else 
    error_message("Unknown parameter"); 
} 
 
// Пользователь не указал никаких параметров 
void interactive_mode() { 
 printf("Pack it up'? (y\\n): "); 
 char mode = _getch(); 
 char filename[100]; 
 char zipname[100]; 
 char dir[100]; 
 
 input_string("\nEnter file name: ", filename); 
 
 if (mode == 'n') { 
  input_string("Enter the directory to unpack: ", dir); 
  unpack(filename, dir); 
 } 
 else 
  pack(filename); 
} 
 
// Вставить название 
void input_string(const char* message, char* destination) { 
 printf("%s", message); 
 scanf("%s", destination); 
 if (!strlen(destination)) 
  error_message("zero string"); 
} 
 
// Сообщеие об ошибке 
void error_message(const char* message) { 
 printf("%s\n", message); 
 // Выйти из программы 
 _getch(); 
 exit(0); 
} 
 
// Запаковакть 
void pack(char* filename) { 
 char strong_filename[1000], 
  strong_zipname[1000], 
  cur_dir[1000], 
  archive_name[1000], 
  file[1000]; 
 _getcwd(cur_dir, 1000); 
 
 // Если введено неабсолютное имя файла, то преобразуется 
 if (filename[1] != ':') { 
  strcpy(strong_filename, cur_dir); 
  strcat(strong_filename, "\\"); 
 } 
 else 
  strong_filename[0] = 0; 
 strcat(strong_filename, filename); 
 
 // Имя архива - тоже, но с распоковкой .zip 
 strcpy(strong_zipname, strong_filename); 
 char* dot = strchr(strong_zipname, '.'); 
 if (dot != NULL) 
  strcpy(dot, ".zip"); 
 else 
  strcat(strong_zipname, ".zip"); 
 
 // Параметры командой строки 
 sprintf(file, "\"%s\"", strong_filename); 
 sprintf(archive_name, "\"%s\"", strong_zipname); 
 
 // Зашифровать 
 crypt(strong_filename);
 
 // Запаковать 
 _spawnl(_P_WAIT, "C:\\Program Files\\7Zip\\7z.exe","7z.exe","a", archive_name, file, NULL);
 
 // Вернуть файл в исходное состояние 
 uncrypt(strong_filename); 
 
 printf("Done"); 
 _getch(); 
} 
 
// Распаковать 
void unpack(const char* zipname, char* dir) { 
 char strong_zipname[1000], 
  filename[1000], 
  cur_dir[1000], 
  param1[1000]; 
 _getcwd(cur_dir, 1000); 
 
 // Если введено неабсолютное имя файла, то преобразуется 
 if (zipname[1] != ':') { 
  strcpy(strong_zipname, cur_dir); 
  strcat(strong_zipname, "\\"); 
 } 
 else 
  strong_zipname[0] = 0; 
 strcat(strong_zipname, zipname); 
 sprintf(param1, "\"%s\"", strong_zipname); 
 
 // Распаковать в текущюю папку (флаг 'x'), а потом поместить в нужное место 
 _spawnl(_P_WAIT, "C:\\Program Files\\7-Zip\\7z.exe","7z.exe","x",param1,NULL);
 
 //Переместитьь в нужный каталог 
 char old_file_name[1000]; 
 char new_file_name[1000];
 
 if (dir[strlen(dir) - 1] != '\\') 
  strcat(dir, "\\");

 strcpy(old_file_name, cur_dir); 
 strcat(old_file_name, strrchr(strong_zipname, '\\') + 1); 
 strcpy(strrchr(old_file_name, '.'),".txt"); 
 strcpy(new_file_name, dir); 
 strcat(new_file_name, strrchr(strong_zipname, '\\') + 1); 
 strcpy(strrchr(new_file_name, '.'), ".txt"); 
 if (rename(old_file_name, new_file_name)) 
     error_message((char*) "Cann't put file to destination folder"); 
 
 // Расшифровать 
 uncrypt(new_file_name); 
 printf("Done"); 
 _getch(); 
}

// Зашифровать 
void crypt(char* filename) { 
    FILE* in = fopen(filename, "r"); 
    FILE* out = fopen(filename, "r+"); 
    if (in == NULL || out == NULL) 
        error_message("File doesn't exist"); 
    char cur; 
    while (fread((void*)&cur, sizeof(char), 1, in) != 0) { 
        if (cur == 0) 
            cur = 255; 
        else 
            cur--; 
        fwrite((void*)&cur, sizeof(char), 1, out); 
    } 
    fclose(in); 
    fclose(out); 
}

// Расшифровать 
void uncrypt(char* filename) { 
    FILE* in = fopen(filename, "r"); 
    FILE* out = fopen(filename, "r+"); 
    if (in == NULL || out == NULL) 
        error_message("File doesn't exist"); 
    char cur; 
    while (fread((void*)&cur, sizeof(char), 1, in) != 0) { 
        if (cur == 255) 
            cur = 0; 
        else 
            cur++; 
        fwrite((void*)&cur, sizeof(char), 1, out); 
    } 
    fclose(in); 
    fclose(out); 
}