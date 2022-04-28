#include<stdio.h>
#include<stdio.h>
#include<string.h>

char op[2], arg1[3], arg2[3], result[2], c;
char arg_stack[50][3];
int arg_stack_index = 0;

char operators[5] = {'/', '*', '+', '-', '='};
char temp_vars[7][3] = {"t0", "t1", "t2", "t3", "t4", "t5", "t6"};
int vars = 0;

int operator(char ch) {
    for (int i = 0; i < 5; i++) {
        if (ch == operators[i]) {
            return 1;
        }
    }
    return 0;
}

void push(char* arg) {
    strcpy(arg_stack[arg_stack_index], arg);
    arg_stack_index++;
}

char* pop() {   
    int temp = arg_stack_index - 1;
    arg_stack_index--;
    return arg_stack[temp];
}



int precedence(char op) {
    for (int i = 0; i < 5; i++) {
        if (op == operators[i]) {
            return i;
        }
    }
    return 0;
}

void stack_print() {
    for (int i = 0; i < arg_stack_index; i++) {
        printf("%s ", arg_stack[i]);
    }
    printf("\n");
}

int should_pop(char op) {
    char last_op;
    int prec_in, prec_last;
    for (int i = 0; i < arg_stack_index; i++) {
        if (operator(arg_stack[i][0])) {
            last_op = arg_stack[i][0];
        }
    }
    prec_last = precedence(last_op);
    prec_in = precedence(op);
    return prec_in >= prec_last;
}

void push_char(char ch) {
    char temp[2] = "\0";
    temp[0] = ch;
    push(temp);
}

int main() {
    FILE *input, *output, *intermediate;
    input = fopen("./icg_input.txt", "r");
    intermediate = fopen("./intermediate_b.txt", "w");
    int k = 0;

    printf("Parsing started\n");
    while (!feof(input)) {
        char in_character = fgetc(input);
        if (in_character == -1) {
            break;
        }
        if (!operator(in_character)) {
            push_char(in_character);
        } else {
            // This is a operator

            if (in_character == '=') {
                push_char(in_character);
                continue;
            }

            // If current character is operator (minus) and previous character is operator
            if (in_character == '-' 
            && operator(arg_stack[arg_stack_index - 1][0]) 
            && arg_stack[arg_stack_index - 1][0] != '=') {
                // Get next argument (symbol) and write Intermediate code 
                char temp_arg = fgetc(input);
                fprintf(intermediate, "%s = %c%c\n", temp_vars[vars], in_character, temp_arg);
                push(temp_vars[vars]);
                vars++;
                // in_character = temp_arg;
            } else if (should_pop(in_character)) {
                // Precedence of current operator 
                int prec_current = precedence(in_character);    
                char arg1[3], arg2[3], op;

                // While stack is not empty
                while (arg_stack_index != -1 && arg_stack[arg_stack_index - 1][0] != '=') {
                    if (arg_stack[arg_stack_index - 2][0] == '=') {
                        break;
                    }
                    if (!operator(arg_stack[arg_stack_index - 1][0])) {
                        strcpy(arg1, pop());
                    }
                    if (operator(arg_stack[arg_stack_index - 1][0])) {
                        int op_prec = precedence(arg_stack[arg_stack_index][0]);
                        if (prec_current >= op_prec) {
                            op = arg_stack[arg_stack_index - 1][0];
                            pop();
                        } else {
                            break;
                        }
                    }
                    if (!operator(arg_stack[arg_stack_index - 1][0])) {
                        strcpy(arg2, pop());
                    }
                    fprintf(intermediate, "%s = %s %c %s\n", temp_vars[vars], arg1, op, arg2);
                    push(temp_vars[vars]);
                    vars++;
                }
                push_char(in_character);
            } else {
                char temp[2] = "\0";
                temp[0] = in_character;
                push(temp);
            }
        }
    }

    // Process remaining elements in stack
    char arg1[4], arg2[4], op; 
    while (arg_stack_index != -1) {
        if (arg_stack[arg_stack_index - 2][0] == '=') {
            strcpy(arg1, pop());
            op = pop()[0];
            strcpy(arg2, pop());
            fprintf(intermediate, "%s = %s\n", arg2, arg1);
            break;
        }
        if (!operator(arg_stack[arg_stack_index - 1][0])) {
            strcpy(arg1, pop());
        }
        if (operator(arg_stack[arg_stack_index - 1][0])) {
            op = arg_stack[arg_stack_index - 1][0];
            pop();
        }
        if (!operator(arg_stack[arg_stack_index - 1][0])) {
            strcpy(arg2, pop());
        }
        fprintf(intermediate, "%s = %s %c %s\n", temp_vars[vars], arg1, op, arg2);
        push(temp_vars[vars]);
        vars++;
    }

    fclose(input);
    fclose(intermediate);
   
    printf("Building quadruple table...\n");
    output = fopen("./icg_output.txt", "w");
    intermediate = fopen("./intermediate_b.txt", "r");

    
    char oper[3], res[4], c, buffer[20];
    strcpy(arg1, "");
    strcpy(arg2, "");
    printf("%8s %8s %8s %8s\n", "Operator", "Arg1", "Arg2", "Result");
    while(fgets(buffer, 20, intermediate)) {
        sscanf(buffer ,"%s%*s%s%s%s", res, arg1, oper, arg2);
        if (strcmp(oper, "") == 0 || strcmp(arg2, "") == 0) {
            strcpy(oper, "=");
            strcpy(arg2, "?");
        }
        fprintf(output,"%8s %8s %8s %8s\n", oper, arg1, arg2, res);  
        strcpy(oper, "");
        strcpy(arg1, "");
        strcpy(arg2, "");
        strcpy(res, "");
    }
    fclose(output);
    fclose(intermediate);

    output = fopen("./icg_output.txt", "r");
    while (fgets(buffer, 20, output)) {
        printf("%s", buffer);
    }
    fclose(output);
    return 0;
}
