#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_STRING_LENGTH 256
#define MAX_INT_VALUE 99999999

typedef enum {
    INT,
    TEXT
} DataType;

typedef struct {
    char name[20];
    DataType type;
    union {
        int intValue;
        char stringValue[MAX_STRING_LENGTH];
    } value;
} Variable;

Variable variables[100];
int variableCount = 0;

void initializeVariables() {
    for (int i = 0; i < 100; i++) {
        variables[i].name[0] = '\0';
    }
}

Variable *findVariable(char *name) {
    for (int i = 0; i < variableCount; i++) {
        if (strcmp(variables[i].name, name) == 0) 
        {
            return &variables[i];
        }
    }
    return NULL;
}
//yeni variable oluştur veya var olanın değerlerini sıfırla
void addVariable(char *name, DataType type) {
    Variable *variable = findVariable(name);
    if (variable == NULL) {
        variable = &variables[variableCount++];
        strcpy(variable->name, name);
    }
    variable->type = type;
    if (type == INT) {
        variable->value.intValue = 0;
    } else {
        variable->value.stringValue[0] = '\0';
    }
}
//değişkene istenen integer değeri atar
void setIntValue(Variable *variable, int value) {
    variable->value.intValue = value;
    if (variable->value.intValue > MAX_INT_VALUE) {
        printf("Error: Maximum integer size exceeded. Variable '%s' set to maximum value.\n", variable->name);
        variable->value.intValue = MAX_INT_VALUE;
    } 
    else if (variable->value.intValue < 0) {
        variable->value.intValue = 0;
    }
}
//değişkene istenen string değeri atar
void setStringValue(Variable *variable, char *value)
{
    strncpy(variable->value.stringValue, value, MAX_STRING_LENGTH);
    variable->value.stringValue[MAX_STRING_LENGTH] = '\0'; // Ensure null termination
}
    
int main()
{
    char line[300];
    initializeVariables();

    FILE *textFile;  
    textFile = fopen("code.lex", "r"); //okunan dosya
    if (textFile == NULL)
    {
        printf("Dosya acma hatasi!\n");
        return 1;
    }
    while ((fgets(line, sizeof(line), textFile) != NULL)) //dosya satır satır okunur
    {
        char *token = strtok(line, "()\n");
        if(strcmp(token,"Keyword")==0)
        {
            token = strtok(NULL, "()\n");
            if(strcmp(token,"int")==0)
            {
                DataType type = INT;
                fgets(line, sizeof(line), textFile);
                token = strtok(line, "()\n");
                token = strtok(NULL, "()\n");
                addVariable(token, type); //istenen isimde int tipinde bir değişken oluşturulur

                fgets(line, sizeof(line), textFile);
                token = strtok(line, "()\n");

                while (strcmp(token,"Comma")==0) //virgül geldikçe yeni değişkenler oluşturulur(int a,b,c.)
                {
                    fgets(line, sizeof(line), textFile);
                    token = strtok(line, "()\n");
                    token = strtok(NULL, "()\n");
                    addVariable(token, type);
                    fgets(line, sizeof(line), textFile);
                    token = strtok(line, "()\n");
                }
                if(strcmp(token,"Keyword")==0) //is
                {
                    fgets(line, sizeof(line), textFile);
                    token = strtok(line, "()\n");
                    if(strcmp(token,"Integer")==0) //girilen int değeri değişkene atanır
                    {
                        token = strtok(NULL, "()\n");
                        int num = atoi(token); //string int e dönüştürülür
                        setIntValue(&variables[variableCount - 1], num);
                        fgets(line, sizeof(line), textFile);
                        token = strtok(line, "()\n");

                        while(strcmp(token,"Comma")==0) //int a is 5,b is 2 durumları
                        {
                            fgets(line, sizeof(line), textFile);
                            token = strtok(line, "()\n");
                            token = strtok(NULL, "()\n");
                            addVariable(token, type);
                            fgets(line, sizeof(line), textFile); //is
                            token = strtok(line, "()\n");
                            fgets(line, sizeof(line), textFile);
                            token = strtok(line, "()\n");
                            if(strcmp(token,"Integer")==0)
                            {
                                token = strtok(NULL, "()\n");
                                int num = atoi(token);
                                setIntValue(&variables[variableCount - 1], num);
                            }
                            else if(strcmp(token,"Identifier")==0) //Identifierın int değeri başta oluşturulan değişkene atılır
                            {
                                token = strtok(NULL, "()\n");
                                Variable* thisVariable = findVariable(token);
                                setIntValue(&variables[variableCount - 1], thisVariable->value.intValue);
                            }

                            fgets(line, sizeof(line), textFile);
                            token = strtok(line, "()\n");
                        }

                        if(strcmp(token,"Operator")==0){ //en son oluşturulan değişkene yapılan işlemler
                            token = strtok(NULL, "()\n");
                            if(strcmp(token,"+")==0){
                                fgets(line, sizeof(line), textFile);
                                token = strtok(line, "()\n"); 
                                if(strcmp(token,"Integer")==0) 
                                {
                                    token = strtok(NULL, "()\n");
                                    int num = atoi(token);
                                    setIntValue(&variables[variableCount - 1],variables[variableCount - 1].value.intValue+num);
                                }
                                else if(strcmp(token,"Identifier")==0)
                                {
                                    token = strtok(NULL, "()\n");
                                    Variable* thisVariable = findVariable(token);
                                    setIntValue(&variables[variableCount - 1],variables[variableCount - 1].value.intValue+thisVariable->value.intValue);
                                }
                            }
                            else if(strcmp(token,"-")==0){
                                fgets(line, sizeof(line), textFile);
                                token = strtok(line, "()\n");
                                if(strcmp(token,"Integer")==0)
                                {
                                    token = strtok(NULL, "()\n");
                                    int num = atoi(token);
                                    setIntValue(&variables[variableCount - 1],(variables[variableCount - 1].value.intValue)-num);
                                }
                                else if(strcmp(token,"Identifier")==0)
                                {
                                    token = strtok(NULL, "()\n");
                                    Variable* thisVariable = findVariable(token);
                                    setIntValue(&variables[variableCount - 1],(variables[variableCount - 1].value.intValue)-(thisVariable->value.intValue));
                                }
                            }
                            else if(strcmp(token,"*")==0){
                                fgets(line, sizeof(line), textFile);
                                token = strtok(line, "()\n");
                                if(strcmp(token,"Integer")==0)
                                {
                                    token = strtok(NULL, "()\n");
                                    int num = atoi(token);
                                    setIntValue(&variables[variableCount - 1],(variables[variableCount - 1].value.intValue)*num);
                                }
                                else if(strcmp(token,"Identifier")==0)
                                {
                                    token = strtok(NULL, "()\n");
                                    Variable* thisVariable = findVariable(token);
                                    setIntValue(&variables[variableCount - 1],(variables[variableCount - 1].value.intValue)*(thisVariable->value.intValue));
                                }
                            }
                            else if(strcmp(token,"/")==0){
                                fgets(line, sizeof(line), textFile);
                                token = strtok(line, "()\n");
                                if(strcmp(token,"Integer")==0)
                                {
                                    token = strtok(NULL, "()\n");
                                    int num = atoi(token);
                                    setIntValue(&variables[variableCount - 1],(variables[variableCount - 1].value.intValue)/num);
                                }
                                else if(strcmp(token,"Identifier")==0)
                                {
                                    token = strtok(NULL, "()\n");
                                    Variable* thisVariable = findVariable(token);
                                    setIntValue(&variables[variableCount - 1],(variables[variableCount - 1].value.intValue)/(thisVariable->value.intValue));
                                }
                            }
                        }
                    }
                    else if(strcmp(token,"Identifier")==0) //oluşturulan değişkene identifierın değeri atama(int a is b)
                    {
                        token = strtok(NULL, "()\n");
                        Variable* thisVariable = findVariable(token); //ismi girilen identifierı bulur
                        setIntValue(&variables[variableCount - 1], thisVariable->value.intValue);
                        fgets(line, sizeof(line), textFile);
                        token = strtok(line, "()\n");

                        while(strcmp(token,"Comma")==0) //int a is b, c is d,e is 2 gibi virgüllü durumlar
                        {
                            fgets(line, sizeof(line), textFile);
                            token = strtok(line, "()\n");

                            token = strtok(NULL, "()\n");
                            addVariable(token, type);

                            fgets(line, sizeof(line), textFile); //is
                            token = strtok(line, "()\n");

                            fgets(line, sizeof(line), textFile);
                            token = strtok(line, "()\n");

                            if(strcmp(token,"Integer")==0)
                            {
                                token = strtok(NULL, "()\n");
                                int num = atoi(token);
                                setIntValue(&variables[variableCount - 1], num);
                            }
                            else if(strcmp(token,"Identifier")==0)
                            {
                                token = strtok(NULL, "()\n");
                                Variable* thisVariable = findVariable(token);
                                setIntValue(&variables[variableCount - 1], thisVariable->value.intValue);
                            }
                            fgets(line, sizeof(line), textFile);
                            token = strtok(line, "()\n");
                        }

                        if(strcmp(token,"Operator")==0){
                            token = strtok(NULL, "()\n");
                            if(strcmp(token,"+")==0){
                                fgets(line, sizeof(line), textFile);
                                token = strtok(line, "()\n"); 
                                if(strcmp(token,"Integer")==0)
                                {
                                    token = strtok(NULL, "()\n");
                                    int num = atoi(token);
                                    setIntValue(&variables[variableCount - 1],(variables[variableCount - 1].value.intValue)+(num));
                                }
                                else if(strcmp(token,"Identifier")==0)
                                {
                                    token = strtok(NULL, "()\n");
                                    Variable* thisVariable = findVariable(token);
                                    setIntValue(&variables[variableCount - 1],variables[variableCount - 1].value.intValue+thisVariable->value.intValue);
                                }
                            }
                            else if(strcmp(token,"-")==0){
                                fgets(line, sizeof(line), textFile);
                                token = strtok(line, "()\n");
                                if(strcmp(token,"Integer")==0)
                                {
                                    token = strtok(NULL, "()\n");
                                    int num = atoi(token);
                                    setIntValue(&variables[variableCount - 1],(variables[variableCount - 1].value.intValue)-(num));
                                }
                                else if(strcmp(token,"Identifier")==0)
                                {
                                    token = strtok(NULL, "()\n");
                                    Variable* thisVariable = findVariable(token);
                                    setIntValue(&variables[variableCount - 1],(variables[variableCount - 1].value.intValue)-(thisVariable->value.intValue));
                                }
                            }
                            else if(strcmp(token,"*")==0){
                                fgets(line, sizeof(line), textFile);
                                token = strtok(line, "()\n");
                                if(strcmp(token,"Integer")==0)
                                {
                                    token = strtok(NULL, "()\n");
                                    int num = atoi(token);
                                    setIntValue(&variables[variableCount - 1],(variables[variableCount - 1].value.intValue)*num);
                                }
                                else if(strcmp(token,"Identifier")==0)
                                {
                                    token = strtok(NULL, "()\n");
                                    Variable* thisVariable = findVariable(token);
                                    setIntValue(&variables[variableCount - 1],(variables[variableCount - 1].value.intValue)*(thisVariable->value.intValue));

                                }
                            }
                            else if(strcmp(token,"/")==0)
                            {
                                fgets(line, sizeof(line), textFile);
                                token = strtok(line, "()\n");
                                //int mi ident mi gelcek diye kontrol
                                if(strcmp(token,"Integer")==0)
                                {
                                    token = strtok(NULL, "()\n");
                                    int num = atoi(token);
                                    setIntValue(&variables[variableCount - 1],(variables[variableCount - 1].value.intValue)/num);
                                }
                                else if(strcmp(token,"Identifier")==0)
                                {
                                    token = strtok(NULL, "()\n");
                                    Variable* thisVariable = findVariable(token);
                                    setIntValue(&variables[variableCount - 1],(variables[variableCount - 1].value.intValue)/(thisVariable->value.intValue));

                                }
                            }
                        }
                    }
                               
                }                
            }
            else if(strcmp(token,"write")==0)
            {
                char metin[2048] =""; //yazdırılacak metin
                fgets(line, sizeof(line), textFile);
                token = strtok(line, "()\n");
                if(strcmp(token,"String")==0)
                {
                    token = strtok(NULL, "()\n");
                    int len = strlen(token);
                    char sub_str[len - 2];
                    strncpy(sub_str, token + 1, len - 2); // "" kaldırılır
                    sub_str[len - 2] = '\0'; 
                    strcat(metin,sub_str);
                }
                else if(strcmp(token,"Identifier")==0)
                {
                    token = strtok(NULL, "()\n");
                    Variable* thisVariable = findVariable(token);
                    if(thisVariable->type== TEXT)
                    {
                        strcat(metin,thisVariable->value.stringValue);
                    }
                    else if(thisVariable->type == INT)
                    {
                        char num[8];
                        sprintf(num, "%d", thisVariable->value.intValue); //sayıyı karakter dizisine dönüştürür
                        strcat(metin,num);
                    }
                }
                else if(strcmp(token,"Integer")==0){
                    token = strtok(NULL, "()\n");
                    strcat(metin,token);
                }
                fgets(line, sizeof(line), textFile);
                token = strtok(line, "()\n");
            
                while(strcmp(token,"Comma")==0) //virgülden sonra gelenler metine aktarılır
                {
                    fgets(line, sizeof(line), textFile);
                    token = strtok(line, "()\n");
                    if(strcmp(token,"String")==0)
                    {
                        token = strtok(NULL, "()\n");
                        int len = strlen(token);
                        // "" kaldırılır
                        char sub_str[len - 2];
                        strncpy(sub_str, token + 1, len - 2);
                        sub_str[len - 2] = '\0'; 
                        strcat(metin,sub_str);

                    }
                    else if(strcmp(token,"Identifier")==0)
                    {
                        token = strtok(NULL, "()\n");
                        Variable* thisVariable = findVariable(token);
                        if(thisVariable->type== TEXT)
                        {
                            strcat(metin,thisVariable->value.stringValue);
                        }
                        else if(thisVariable->type == INT)
                        {
                            char num[8];
                            // sprintf ile tamsayıyı karakter dizisine dönüştürme
                            sprintf(num, "%d", thisVariable->value.intValue);
                            strcat(metin,num);
                        }
                    }
                    else if(strcmp(token,"Integer")==0)
                    {
                        token = strtok(NULL, "()\n");
                        strcat(metin,token);
                    }
                    fgets(line, sizeof(line), textFile);
                    token = strtok(line, "()\n");    
                }
                printf("%s\n",metin);
            }
            else if(strcmp(token,"newLine")==0)
            {
                printf("\n");
            }
            else if(strcmp(token,"read")==0)
            {
                fgets(line, sizeof(line), textFile);
                token = strtok(line, "()\n");
                if(strcmp(token,"String")==0)
                {
                    token = strtok(NULL, "()\n");
                    printf("%s",token); //read den sonra yazan yazı ekrana yazdırılır
                    fgets(line, sizeof(line), textFile);
                    token = strtok(line, "()\n"); //Comma(,)
                    fgets(line, sizeof(line), textFile);
                    token = strtok(line, "()\n"); //Identifier
                    token = strtok(NULL, "()\n"); //variable
                    Variable* thisVariable = findVariable(token);
        
                    if(thisVariable->type==INT){
                        int num =0;
                        scanf("%d", &num);
                        setIntValue(thisVariable,num);
                    }
                    else if(thisVariable->type==TEXT){
                        char text[] = "";
                        scanf("%s", &text);
                        setStringValue(thisVariable,text);
                    }
                }
                else if(strcmp(token,"Identifier")==0) //readden sonra metin olmadan direkt input alma
                {
                    token = strtok(NULL, "()\n");
                    Variable* thisVariable = findVariable(token);
                    if(thisVariable->type==INT){
                        int num =0;
                        scanf("%d", &num);
                        setIntValue(thisVariable,num);                    }
                    else if(thisVariable->type==TEXT){
                        char text[] = "";
                        scanf("%s", &text);
                        setStringValue(thisVariable,text);
                    }
                }
            }
            else if(strcmp(token,"text")==0)
            {
                DataType type = TEXT;
                fgets(line, sizeof(line), textFile);
                token = strtok(line, "()\n");
                
                token = strtok(NULL, "()\n");
                addVariable(token, type);

                fgets(line, sizeof(line), textFile);
                token = strtok(line, "()\n");

                while (strcmp(token,"Comma")==0)
                {
                    fgets(line, sizeof(line), textFile);
                    token = strtok(line, "()\n");
                    
                    token = strtok(NULL, "()\n");
                    addVariable(token, type);

                    fgets(line, sizeof(line), textFile);
                    token = strtok(line, "()\n");
                }

                if(strcmp(token,"Keyword")==0) //is
                {
                    fgets(line, sizeof(line), textFile);
                    token = strtok(line, "()\n");

                    if(strcmp(token,"String")==0)
                    {
                        token = strtok(NULL, "()\n");
                        int len = strlen(token);
                        char sub_str[len - 2];
                        strncpy(sub_str, token + 1, len - 2);
                        sub_str[len - 2] = '\0';

                        setStringValue(&variables[variableCount - 1], sub_str);

                        fgets(line, sizeof(line), textFile);
                        token = strtok(line, "()\n");


                        while(strcmp(token,"Comma")==0)
                        {
                            fgets(line, sizeof(line), textFile);
                            token = strtok(line, "()\n");

                            token = strtok(NULL, "()\n");
                            addVariable(token, type);

                            fgets(line, sizeof(line), textFile); //is
                            token = strtok(line, "()\n");
                            

                            fgets(line, sizeof(line), textFile);
                            token = strtok(line, "()\n");
                            if(strcmp(token,"String")==0)
                            {
                                token = strtok(NULL, "()\n");

                                int len = strlen(token);
                                char sub_str[len - 2];
                                strncpy(sub_str, token + 1, len - 2);
                                sub_str[len - 2] = '\0';

                                setStringValue(&variables[variableCount - 1], sub_str);

                            }
                            else if(strcmp(token,"Identifier")==0)
                            {
                                token = strtok(NULL, "()\n");

                                Variable* thisVariable = findVariable(token);
                                setStringValue(&variables[variableCount - 1], thisVariable->value.stringValue);
                            }

                            fgets(line, sizeof(line), textFile);
                            token = strtok(line, "()\n");
                        }

                        if(strcmp(token,"Operator")==0){
                            token = strtok(NULL, "()\n");

                            if(strcmp(token,"+")==0){
                                fgets(line, sizeof(line), textFile);
                                token = strtok(line, "()\n"); 
                                //int mi ident mi gelcek diye kontrol
                                if(strcmp(token,"String")==0)
                                {
                                    token = strtok(NULL, "()\n");

                                    int len = strlen(token);
                                    char sub_str[len - 2];
                                    strncpy(sub_str, token + 1, len - 2);
                                    sub_str[len - 2] = '\0';

                                    strcat(variables[variableCount - 1].value.stringValue, sub_str);
                                    setStringValue(&variables[variableCount - 1],variables[variableCount - 1].value.stringValue);
                                    
                                }
                                else if(strcmp(token,"Identifier")==0)
                                {
                                    token = strtok(NULL, "()\n");
                                    
                                    Variable* thisVariable = findVariable(token);
                                    strcat(variables[variableCount - 1].value.stringValue,thisVariable->value.stringValue);
                                    setStringValue(&variables[variableCount - 1],variables[variableCount - 1].value.stringValue);
                                    
                                }
                            }
                            else if(strcmp(token,"-")==0){
                                fgets(line, sizeof(line), textFile);
                                token = strtok(line, "()\n");
                                //int mi ident mi gelcek diye kontrol
                                if(strcmp(token,"String")==0)
                                {
                                    token = strtok(NULL, "()\n");

                                    int len = strlen(token);
                                    char sub_str[len - 2];
                                    strncpy(sub_str, token + 1, len - 2);
                                    sub_str[len - 2] = '\0';


                                    char *ptr = strstr(variables[variableCount - 1].value.stringValue, sub_str);
    
                                    if (ptr != NULL) {
                                        memmove(ptr, ptr + strlen(sub_str), strlen(ptr + strlen(sub_str)) + 1);
                                    }

                                    setStringValue(&variables[variableCount - 1],variables[variableCount - 1].value.stringValue);
                                }
                                else if(strcmp(token,"Identifier")==0)
                                {
                                    token = strtok(NULL, "()\n");

                                    Variable* thisVariable = findVariable(token);

                                    char *ptr = strstr(variables[variableCount - 1].value.stringValue, thisVariable->value.stringValue);
    
                                    if (ptr != NULL) {
                                        memmove(ptr, ptr + strlen(thisVariable->value.stringValue), strlen(ptr + strlen(thisVariable->value.stringValue)) + 1);
                                    }

                                    setStringValue(&variables[variableCount - 1],variables[variableCount - 1].value.stringValue);

                                }
                            }
                        }                   
                    }
                    else if(strcmp(token,"Identifier")==0)
                    {
                        token = strtok(NULL, "()\n");
                        
                        Variable* thisVariable = findVariable(token);
                        setStringValue(&variables[variableCount - 1], thisVariable->value.stringValue);
                        fgets(line, sizeof(line), textFile);
                        token = strtok(line, "()\n");

                        while(strcmp(token,"Comma")==0)
                        {
                            fgets(line, sizeof(line), textFile);
                            token = strtok(line, "()\n");

                            token = strtok(NULL, "()\n");
                            addVariable(token, type);

                            fgets(line, sizeof(line), textFile); //is
                            token = strtok(line, "()\n");

                            fgets(line, sizeof(line), textFile);
                            token = strtok(line, "()\n");

                            if(strcmp(token,"String")==0)
                            {
                                token = strtok(NULL, "()\n");

                                int len = strlen(token);
                                char sub_str[len - 2];
                                strncpy(sub_str, token + 1, len - 2);
                                sub_str[len - 2] = '\0';

                                setStringValue(&variables[variableCount - 1], sub_str);
                            }
                            else if(strcmp(token,"Identifier")==0)
                            {
                                token = strtok(NULL, "()\n");

                                Variable* thisVariable = findVariable(token);
                                setStringValue(&variables[variableCount - 1], thisVariable->value.stringValue);
                            }

                            fgets(line, sizeof(line), textFile);
                            token = strtok(line, "()\n");
                        }

                        if(strcmp(token,"Operator")==0){
                            token = strtok(NULL, "()\n");

                            if(strcmp(token,"+")==0){
                                fgets(line, sizeof(line), textFile);
                                token = strtok(line, "()\n"); 
                    
                                //int mi ident mi gelcek diye kontrol
                                if(strcmp(token,"String")==0)
                                {
                                    token = strtok(NULL, "()\n");
                                    
                                    int len = strlen(token);
                                    char sub_str[len - 2];
                                    strncpy(sub_str, token + 1, len - 2);
                                    sub_str[len - 2] = '\0';

                                    strcat(variables[variableCount - 1].value.stringValue, sub_str);
                                    setStringValue(&variables[variableCount - 1],variables[variableCount - 1].value.stringValue);

                                }
                                else if(strcmp(token,"Identifier")==0)
                                {
                                    token = strtok(NULL, "()\n");
                                    
                                    Variable* thisVariable = findVariable(token);
                                    strcat(variables[variableCount - 1].value.stringValue,thisVariable->value.stringValue);
                                    setStringValue(&variables[variableCount - 1],variables[variableCount - 1].value.stringValue);

                                }
                            }
                            else if(strcmp(token,"-")==0){
                                fgets(line, sizeof(line), textFile);
                                token = strtok(line, "()\n");
                            
                                //int mi ident mi gelcek diye kontrol
                                if(strcmp(token,"String")==0)
                                {
                                    token = strtok(NULL, "()\n");

                                    int len = strlen(token);
                                    char sub_str[len - 2];
                                    strncpy(sub_str, token + 1, len - 2);
                                    sub_str[len - 2] = '\0';                                    

                                    char *ptr = strstr(variables[variableCount - 1].value.stringValue, sub_str);
    
                                    if (ptr != NULL) {
                                        memmove(ptr, ptr + strlen(sub_str), strlen(ptr + strlen(sub_str)) + 1);
                                    }

                                    setStringValue(&variables[variableCount - 1],variables[variableCount - 1].value.stringValue);
                                }
                                else if(strcmp(token,"Identifier")==0)
                                {
                                    token = strtok(NULL, "()\n");
                                    Variable* thisVariable = findVariable(token);

                                    char *ptr = strstr(variables[variableCount - 1].value.stringValue, thisVariable->value.stringValue);
    
                                    if (ptr != NULL) {
                                        memmove(ptr, ptr + strlen(thisVariable->value.stringValue), strlen(ptr + strlen(thisVariable->value.stringValue)) + 1);
                                    }

                                    setStringValue(&variables[variableCount - 1],variables[variableCount - 1].value.stringValue);

                                }
                            }
                        }
                    }
                               
                }    
            }
            else if(strcmp(token,"loop")==0)
            {
                int i=0; // Loop'un kaç defa döneceği i değişkeninde tutulur
                fgets(line, sizeof(line), textFile);
                token = strtok(line, "()\n");

                // i değişkeni burada atanır
                if(strcmp(token,"Integer")==0)
                {
                    token = strtok(NULL, "()\n");
                    i = atoi(token);
                }
                else if(strcmp(token,"Identifier")==0)
                {
                    token = strtok(NULL, "()\n");
                    Variable* thisVariable = findVariable(token);
                    i=thisVariable->value.intValue;
                }

                fgets(line, sizeof(line), textFile);
                token = strtok(line, "()\n"); //Keyword(times)
                fgets(line, sizeof(line), textFile);
                token = strtok(line, "()\n"); 
                if(strcmp(token,"Keyword")==0) //Keyword(write)
                {
                    // Loop curly brackets içermiyosa bu if bloğu çalışır
                    fgets(line, sizeof(line), textFile);
                    token = strtok(line, "()\n");
                    char text[] = ""; // text'te i kadar yazılacak olan ifade tutulur

                    if(strcmp(token,"String")==0)
                    {
                        token = strtok(NULL, "()\n");

                        int len = strlen(token);
                        char sub_str[len - 2];
                        strncpy(sub_str, token + 1, len - 2);
                        sub_str[len - 2] = '\0';

                        strcpy(text, sub_str);
                        
                    }
                    else if(strcmp(token,"Integer")==0)
                    {
                        token = strtok(NULL, "()\n");
                        strcpy(text, token);
                    }
                    else if(strcmp(token,"Identifier")==0)
                    {
                        token = strtok(NULL, "()\n");

                        Variable* thisVariable = findVariable(token);

                        strcpy(text, thisVariable -> value.stringValue);

                    }

                    for(int j = 0; j < i; j++){
                        printf("%s", text);
                    }

                }
                // Curly bracket içeren loop'lar bu blokta çalışır
                else if(strcmp(token,"LeftCurlyBracket")==0)
                {
                    long position;
                    position = ftell(textFile); // süslü parantezin başı işaretlenir ve dosyada her seferinde bu noktadan tekrar döngü başlatılır
                    //i sayısının değeri kadar süslü pazartezin içi dönülür
                    for (int j = 0; j < i; j++)
                    {
                        fgets(line, sizeof(line), textFile);
                        token = strtok(line, "()\n");
                        // Süslü parantez kapanana kadar içindeki ifadeler çalıştırılır
                        while(strcmp(token, "RightCurlyBracket") != 0)
                        {
                            if(strcmp(token,"Keyword")==0)
                            {
                                token = strtok(NULL, "()\n");

                                if(strcmp(token,"write")==0)
                                {
                                    fgets(line, sizeof(line), textFile);
                                    token = strtok(line, "()\n");

                                    if(strcmp(token,"Identifier")==0){
                                        token = strtok(NULL, "()\n");
                                        Variable* thisVariable = findVariable(token);
                                        printf("%d", thisVariable -> value.intValue);
                                    }
                                    else if(strcmp(token,"String")==0){
                                        token = strtok(NULL, "()\n");

                                        int len = strlen(token);
                                        char sub_str[len - 2];
                                        strncpy(sub_str, token + 1, len - 2);
                                        sub_str[len - 2] = '\0';

                                        printf("%s", sub_str);
                                    }
                                    else if(strcmp(token,"Integer")==0){
                                        token = strtok(NULL, "()\n");
                                        printf("%s", token);
                                    }
                                }
                                else if(strcmp(token,"newLine")==0)
                                {
                                    printf("\n");
                                }
                                // Loop içinde tekrar loop ile karşılaşılırsa bu bloğa girilir
                                else if(strcmp(token, "loop") == 0)
                                {
                                    int i=0;
                                    fgets(line, sizeof(line), textFile);
                                    token = strtok(line, "()\n");
                                    if(strcmp(token,"Integer")==0)
                                    {
                                        token = strtok(NULL, "()\n");
                                        i = atoi(token);
                                    }
                                    else if(strcmp(token,"Identifier")==0)
                                    {
                                        token = strtok(NULL, "()\n");
                                        Variable* thisVariable = findVariable(token);
                                        i=thisVariable->value.intValue;
                                    }
                                    fgets(line, sizeof(line), textFile);
                                    token = strtok(line, "()\n"); //times
                                    fgets(line, sizeof(line), textFile);
                                    token = strtok(line, "()\n"); 
                                    if(strcmp(token,"Keyword")==0)
                                    {
                                        fgets(line, sizeof(line), textFile);
                                        token = strtok(line, "()\n");
                                        char text[] = "";

                                        if(strcmp(token,"String")==0)
                                        {
                                            token = strtok(NULL, "()\n");

                                            int len = strlen(token);
                                            char sub_str[len - 2];
                                            strncpy(sub_str, token + 1, len - 2);
                                            sub_str[len - 2] = '\0';

                                            strcpy(text, sub_str);
                                            
                                        }
                                        else if(strcmp(token,"Integer")==0)
                                        {
                                            token = strtok(NULL, "()\n");
                                            strcpy(text, token);
                                        }
                                        else if(strcmp(token,"Identifier")==0)
                                        {
                                            token = strtok(NULL, "()\n");
                                            Variable* thisVariable = findVariable(token);
                                            strcpy(text, thisVariable -> value.stringValue);
                                        }
                                        for(int j = 0; j < i; j++){
                                            printf("%s", text);
                                        }
                                    }
                                }
                            }
                            else if(strcmp(token,"Identifier")==0)
                            {
                                token = strtok(NULL, "()\n");
                                Variable* thisVariable = findVariable(token);

                                fgets(line, sizeof(line), textFile);
                                fgets(line, sizeof(line), textFile);
                                token = strtok(line, "()\n");
                                token = strtok(NULL, "()\n");
                                Variable* otherVariable = findVariable(token);

                                fgets(line, sizeof(line), textFile);
                                token = strtok(line, "()\n");
                                token = strtok(NULL, "()\n");

                                if(strcmp(token, "+") == 0)
                                {
                                    fgets(line, sizeof(line), textFile);
                                    token = strtok(line, "()\n");

                                    token = strtok(NULL, "()\n");
                                    int num = atoi(token);
                                
                                    setIntValue(thisVariable, otherVariable ->value.intValue + num);
                                }
                                else if(strcmp(token, "-") == 0)
                                {
                                    fgets(line, sizeof(line), textFile);
                                    token = strtok(line, "()\n");

                                    token = strtok(NULL, "()\n");
                                    
                                    int num = atoi(token);
                                    setIntValue(thisVariable, otherVariable ->value.intValue - num);
                                }
                            }
                            fgets(line, sizeof(line), textFile);
                            token = strtok(line, "()\n");
                        }
                        if(j < i-1)
                        {
                             fseek(textFile, position, SEEK_SET); // Bu noktaya gelindiğinde döngü sayısı kadar dosyada işaretlenen satıra geri dönülür
                        }
                    }
                }
            }
        }
        else if(strcmp(token,"Identifier")==0)
        {
            token = strtok(NULL, "()\n");
            Variable* thisVariable = findVariable(token); //ismi girilen identifier bulunur

            if(thisVariable->type == INT){
                fgets(line, sizeof(line), textFile);
                token = strtok(line, "()\n"); //is
                fgets(line, sizeof(line), textFile);
                token = strtok(line, "()\n"); 
                if(strcmp(token,"Integer")==0){ //girilen int değeri atanır
                    token = strtok(NULL, "()\n");
                    int num = atoi(token);
                    setIntValue(thisVariable,num);
                }
                else if(strcmp(token,"Identifier")==0){ //girilen identifierin int değeri atanır
                    token = strtok(NULL, "()\n");
                    Variable* otherVariable = findVariable(token);
                    setIntValue(thisVariable,otherVariable->value.intValue);
                }
                fgets(line, sizeof(line), textFile);
                token = strtok(line, "()\n"); 
                if(strcmp(token,"Operator")==0) //aritmetik işlemler yapılır ve değişkene atanır
                {
                    token = strtok(NULL, "()\n");
                    if(strcmp(token,"+")==0)
                    {
                        fgets(line, sizeof(line), textFile);
                        token = strtok(line, "()\n"); 
                        if(strcmp(token,"Integer")==0)
                        {
                            token = strtok(NULL, "()\n");
                            int num = atoi(token);
                            setIntValue(thisVariable,thisVariable->value.intValue+num);
                        }
                        else if(strcmp(token,"Identifier")==0)
                        {
                            token = strtok(NULL, "()\n");
                            Variable* var2 = findVariable(token);
                            setIntValue(thisVariable,thisVariable->value.intValue+var2->value.intValue);
                        }
                    }
                    else if(strcmp(token,"-")==0)
                    {
                        fgets(line, sizeof(line), textFile);
                        token = strtok(line, "()\n"); 
                        if(strcmp(token,"Integer")==0)
                        {
                            token = strtok(NULL, "()\n");
                            int num = atoi(token);
                            setIntValue(thisVariable,thisVariable->value.intValue-num);
                        }
                        else if(strcmp(token,"Identifier")==0)
                        {
                            token = strtok(NULL, "()\n");
                            Variable* var2 = findVariable(token);
                            setIntValue(thisVariable,(thisVariable->value.intValue)-(var2->value.intValue));
                        }
                    }
                    else if(strcmp(token,"*")==0)
                    {
                        fgets(line, sizeof(line), textFile);
                        token = strtok(line, "()\n"); 
                        if(strcmp(token,"Integer")==0)
                        {
                            token = strtok(NULL, "()\n");
                            int num = atoi(token);
                            setIntValue(thisVariable,(thisVariable->value.intValue)*(num));
                        }
                        else if(strcmp(token,"Identifier")==0)
                        {
                            token = strtok(NULL, "()\n");
                            Variable* var2 = findVariable(token);
                            setIntValue(thisVariable,(thisVariable->value.intValue)*(var2->value.intValue));
                        }
                    }
                    else if(strcmp(token,"/")==0)
                    {
                        fgets(line, sizeof(line), textFile);
                        token = strtok(line, "()\n"); 
                        if(strcmp(token,"Integer")==0)
                        {
                            token = strtok(NULL, "()\n");
                            int num = atoi(token);
                            setIntValue(thisVariable,(thisVariable->value.intValue)/(num));
                        }
                        else if(strcmp(token,"Identifier")==0)
                        {
                            token = strtok(NULL, "()\n");
                            Variable* var2 = findVariable(token);
                            setIntValue(thisVariable,(thisVariable->value.intValue)/(var2->value.intValue));
                        }
                    }
                }
            }
            else if(thisVariable->type == TEXT)
            {
                fgets(line, sizeof(line), textFile);
                token = strtok(line, "()\n"); //is
                fgets(line, sizeof(line), textFile);
                token = strtok(line, "()\n"); 
                if(strcmp(token,"String")==0){ //girilen string değişkene atanır
                    token = strtok(NULL, "()\n");
                    int len = strlen(token);
                    char sub_str[len - 2];
                    strncpy(sub_str, token + 1, len - 2); //"" kaldırılır
                    sub_str[len - 2] = '\0';
                    setStringValue(thisVariable, sub_str);
                }
                else if(strcmp(token,"Identifier")==0){
                    token = strtok(NULL, "()\n");
                    Variable* otherVariable = findVariable(token);
                    setStringValue(thisVariable,otherVariable->value.stringValue);
                }
                fgets(line, sizeof(line), textFile);
                token = strtok(line, "()\n"); 
                if(strcmp(token,"Operator")==0) //text için + ve - işlemleri 
                {
                    token = strtok(NULL, "()\n");
                    if(strcmp(token,"+")==0)
                    {
                        fgets(line, sizeof(line), textFile);
                        token = strtok(line, "()\n"); 
                        if(strcmp(token,"String")==0)
                        {
                            token = strtok(NULL, "()\n");
                            int len = strlen(token);
                            char sub_str[len - 2];
                            strncpy(sub_str, token + 1, len - 2);
                            sub_str[len - 2] = '\0';

                            strcat(thisVariable->value.stringValue, sub_str);
                            setStringValue(thisVariable,thisVariable->value.stringValue);
                        }
                        else if(strcmp(token,"Identifier")==0)
                        {
                            token = strtok(NULL, "()\n");
                            Variable* otherVariable = findVariable(token);
                            strcat(thisVariable->value.stringValue,otherVariable->value.stringValue);
                            setStringValue(thisVariable,thisVariable->value.stringValue);

                        }
                    }
                    else if(strcmp(token,"-")==0)
                    {
                        fgets(line, sizeof(line), textFile);
                        token = strtok(line, "()\n");
                        if(strcmp(token,"String")==0)
                        {
                            token = strtok(NULL, "()\n");
                            int len = strlen(token);
                            char sub_str[len - 2];
                            strncpy(sub_str, token + 1, len - 2);
                            sub_str[len - 2] = '\0';                                    
                            char *ptr = strstr(thisVariable->value.stringValue, sub_str);

                            if (ptr != NULL) 
                            {
                                memmove(ptr, ptr + strlen(sub_str), strlen(ptr + strlen(sub_str)) + 1); //çıkması istenen metin çıkartılır
                            }
                            setStringValue(thisVariable,thisVariable->value.stringValue);
                        }
                        else if(strcmp(token,"Identifier")==0)
                        {
                            Variable* otherVariable = findVariable(token);
                            char *ptr = strstr(thisVariable->value.stringValue, otherVariable->value.stringValue);

                            if (ptr != NULL)
                            {
                                memmove(ptr, ptr + strlen(otherVariable->value.stringValue), strlen(ptr + strlen(otherVariable->value.stringValue)) + 1);
                            }
                            setStringValue(thisVariable,thisVariable->value.stringValue);
                        }
                    }
                }
            }
        } 
    }
    return 0;
}
