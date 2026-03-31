typedef struct {
    int array[100];
    int top; 
} Stack;

enum MATERIAL {
    LAMBERTIAN, REFLECTIVE, FUZZY, DIELECTRIC
};

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

void kernel ray_color(__read_only image2d_array_t pts, __read_only image2d_array_t dirs, __write_only image2d_array_t out) {
    int g0 = get_global_id(0);
    int g1 = get_global_id(1);
    int g2 = get_global_id(2);
    __constant sampler_t my_sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
    float4 color = (float4)(0.0f);
    
    int4 coord = (int4)(g0, g1, g2, 0);
    if (false) write_imagef(out, coord, color);
    float4 dir = read_imagef(dirs, my_sampler, coord);
    float4 unit_dir = normalize(dir);
    float a = 0.5f * (unit_dir.y + 1.0f);
    color = (1.0f - a) * (float4)(1.0f) + a * (float4)(0.5f, 0.8f, 0.8f, 1.0f);
    write_imagef(out, coord, color);
}

