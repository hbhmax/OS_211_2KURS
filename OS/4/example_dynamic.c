#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

typedef float (*sin_integral_t)(float, float, float);
typedef float (*e_t)(int);

char *paths[] = {"./src/libown1.so", "./src/libown2.so"};
char *lib_names[] = {"Library 1 (Rectangles method, Formula)", 
                     "Library 2 (Trapezoids method, Series sum)"};

sin_integral_t sin_integral_func;
e_t e_func;
void *lib = NULL;
int current_lib = 0;

void print_menu() {
    printf("Current library: %s\n", lib_names[current_lib]);
    printf("1 - Calculate integral of sin(x) from A to B\n");
    printf("2 - Calculate Euler's number e\n");
    printf("0 - Switch library implementation\n");
    printf("-1 - Exit\n");
    printf("Choose function: ");
}

int open_and_init(int ind) {
    if (lib) dlclose(lib);
    lib = dlopen(paths[ind], RTLD_LAZY);
    if (!lib) {
        printf("Error: Cannot load library %s\n", paths[ind]);
        return -1;
    }
    
    sin_integral_func = (sin_integral_t)dlsym(lib, "sin_integral");
    e_func = (e_t)dlsym(lib, "e");
    
    if (!sin_integral_func || !e_func) {
        printf("Error: Cannot load functions from library\n");
        return -1;
    }
    return 0;
}

int main() {
    int choice;
    
    printf("Available libraries:\n");
    printf("• Library 1: Rectangle method for integral, (1+1/x)^x for e\n");
    printf("• Library 2: Trapezoid method for integral, series sum for e\n\n");
    
    if (open_and_init(current_lib) != 0) {
        printf("Fatal error: Cannot load initial library\n");
        return 1;
    }

    print_menu();
    scanf("%d", &choice);
    
    while (choice != -1) {
        if (choice == 0) {
            // Switch library
            current_lib = 1 - current_lib;
            if (open_and_init(current_lib) != 0) {
                printf("Error switching library\n");
                return 1;
            }
            printf("✓ Switched to: %s\n", lib_names[current_lib]);
            
        } else if (choice == 1) {
            float a, b, e_val;
            printf("\n--- Integral of sin(x) ---\n");
            printf("Enter A (start): ");
            scanf("%f", &a);
            printf("Enter B (end): ");
            scanf("%f", &b);
            printf("Enter E (step): ");
            scanf("%f", &e_val);
            
            float result = sin_integral_func(a, b, e_val);
            printf("Result: ∫sin(x)dx from %.2f to %.2f = %f\n", a, b, result);
            
        } else if (choice == 2) {
            int x;
            printf("\n--- Euler's number e ---\n");
            printf("Enter precision (integer > 0): ");
            scanf("%d", &x);
            
            if (x <= 0) {
                printf("Error: Precision must be positive integer\n");
            } else {
                float result = e_func(x);
                printf("Result: e ≈ %f\n", result);
            }
            
        } else {
            printf("Invalid choice! Please try again.\n");
        }
        
        print_menu();
        scanf("%d", &choice);
    }

    if (lib) dlclose(lib);
    return 0;
}