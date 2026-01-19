typedef struct {
    int array[100];
    int top; 
} Stack;

void initialize(Stack *stack) { stack->top = -1;}

bool isEmpty(Stack *stack) { return stack->top == -1;}

void push(Stack *stack, int n) { stack->array[++stack->top] = n;}

int pop(Stack *stack) { return stack->array[stack->top--];}

void kernel simple_add(global const int* A, global const int* B, global const int* C, global int* D) {
    int gid = get_global_id( 0 );
    D[gid]=A[gid]+B[gid]+C[gid];                 
}

void kernel fibonacci(global const int* N, global int* O) {
    int gid = get_global_id( 0 );
    int sum = 0;

    Stack stack;
    initialize(&stack);
    push(&stack, N[gid]);
    while (!isEmpty(&stack)) {
        int n = pop(&stack);
        if (n == 0 || n == 1)
            sum += n;
        else {
            push(&stack, n - 1);
            push(&stack, n - 2);
        }
    }

    O[gid] = sum;
}

