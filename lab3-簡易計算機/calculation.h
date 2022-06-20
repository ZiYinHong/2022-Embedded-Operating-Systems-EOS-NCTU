#pragma once
#include <stack>
#include <string>
#include <stdio.h> 


// return priority
int priority(char op) { 
    switch(op) { 
        case '+': case '-': return 1;
        case '*': case '/': return 2;
        default:            return 0;
    } 
} 

// return arithmetic result
int cal(char op, int num1, int num2) { 
    switch(op) { 
        case '+': return num1 + num2; 
        case '-': return num1 - num2; 
        case '*': return num1 * num2; 
        case '/':
            if(num1 < num2 && num1 != 0){
                printf("\nwe don't provide fraction result, please let num1 >= num2.\n");
                return -1;
            }
            if(num1 % num2 != 0){
                printf("\nwe don't provide fraction result, please let division with no remainder.\n");
                return -1;
            }  
            if(num2 == 0){
                printf("\ndivision by 0, please let num2 != 0.\n");
                return -1;
            } 
            return num1 / num2; 
        default: return -1;
    } 
}

int read_num(int& i, const string& input){
    int num = 0;
    while(isdigit(input[i])){
        num = num*10+(input[i] - '0');
        i++;
    }
    return num;
}

/*push operand and operators in stacks*/
void pushing(stack<int>& operands, stack<char>& operators, int& i, const string& input){  //都是先push運算元再push數字的
    operators.push(input[i++]);         //put the leading operator in operator stack
    operands.push(read_num(i, input));  //put the following number in operands stack
}

/*do the overall calculation and return final result*/
int calculation(const string& input){
    int i = 0;
    stack<int> operands;   //儲存表示式中的數值
    stack<char> operators; //儲存表示式中的運算元

    operands.push(read_num(i, input));  //put first number in operands stack
    while(i < input.length()){
        pushing(operands, operators, i, input);
        if( i >= input.length() || ( priority(input[i]) <= priority(operators.top()) )){
            do{
                int num2 = operands.top();
                operands.pop();
                int num1 = operands.top();
                operands.pop();
                char op = operators.top();
                operators.pop();

                int result = cal(op, num1, num2);   
                operands.push(result);
            }while(!operators.empty());
        }
    };
    return operands.top();  //result = operands.top()
}
