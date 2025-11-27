#include <stdio.h>
#include "include/lib.h"

void print_menu() {
    printf("1 - Calculate integral of sin(x) from A to B\n");
    printf("2 - Calculate Euler's number e\n");
    printf("0 - Exit\n");
    printf("Choose function: ");
}

int main() {
    int choice;
    
    printf("Available functions:\n");
    printf("1. Integral of sin(x) from A to B with step E\n");
    printf("2. Euler's number e approximation\n\n");
    
    print_menu();
    scanf("%d", &choice);
    
    while (choice != 0) {
        if (choice == 1) {
            float a, b, e_val;
            printf("\n--- Integral of sin(x) ---\n");
            printf("Enter A (start): ");
            scanf("%f", &a);
            printf("Enter B (end): ");
            scanf("%f", &b);
            printf("Enter E (step): ");
            scanf("%f", &e_val);
            
            float result = sin_integral(a, b, e_val);
            printf("Result: ∫sin(x)dx from %.2f to %.2f = %f\n", a, b, result);
            
        } else if (choice == 2) {
            int x;
            printf("\n--- Euler's number e ---\n");
            printf("Enter precision (integer > 0): ");
            scanf("%d", &x);
            
            if (x <= 0) {
                printf("Error: Precision must be positive integer\n");
            } else {
                float result = e(x);
                printf("Result: e ≈ %f\n", result);
            }
            
        } else {
            printf("Invalid choice! Please try again.\n");
        }
        
        print_menu();
        scanf("%d", &choice);
    }
    
    return 0;
}