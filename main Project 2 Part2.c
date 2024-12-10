#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include "BENSCHILLIBOWL.h" // Include your header file

// Define constants here since we cannot modify the header file
#define NUM_CUSTOMERS 90
#define NUM_COOKS 10
#define ORDERS_PER_CUSTOMER 3 // Define number of orders per customer

// Global variable for the restaurant.
BENSCHILLIBOWL *bcb;

/* Thread function that represents a customer. A customer should: */
/*  - allocate space (memory) for an order. */
/*  - select a menu item. */
/*  - populate the order with their menu item and their customer ID. */
/*  - add their order to the restaurant.*/
void* BENSCHILLIBOWLCustomer(void* tid) {
    int customer_id = (int)(long)tid;

    for (int i = 0; i < ORDERS_PER_CUSTOMER; i++) {
        sleep(rand() % 5); // Sleep for a random amount of time between 0-4 seconds

        Order order;

        // Allocate memory for menu_item
        order.menu_item = malloc(50 * sizeof(char)); // Allocate space for up to 50 characters
        if (order.menu_item == NULL) {
            fprintf(stderr, "Error: Failed to allocate memory for menu_item\n");
            pthread_exit(NULL);
        }

        snprintf(order.menu_item, 50, "%s", PickRandomMenuItem()); // Random menu item
        order.customer_id = customer_id;

        printf("Customer #%d: Placing order: %s\n", customer_id, order.menu_item);
        AddOrder(bcb, &order); // Add the order to the restaurant

        // Free allocated memory for menu_item after use
        free(order.menu_item);
    }

    return NULL;
}

/* Thread function that represents a cook in the restaurant. A cook should:*/
/*  - get an order from the restaurant. */
/*  - if the order is valid, it should fulfill the order, and then */
/*     free the space taken by the order. */
void* BENSCHILLIBOWLCook(void* tid) {
    int cook_id = (int)(long) tid;

    while (1) {
        sleep(rand() % 5); // Sleep for a random amount of time between 0-4 seconds

        Order *order = GetOrder(bcb); // Get an order from the restaurant

        if (order == NULL) {
            printf("Cook #%d: No more orders to fulfill. Exiting...\n", cook_id);
            break; // Exit if no orders are available
        }

        printf("Cook #%d: Fulfilling order from Customer #%d: %s\n", cook_id, order->customer_id, order->menu_item);

        // Simulate fulfilling the order
        sleep(1); // Simulate time taken to fulfill the order

        printf("Cook #%d: Finished fulfilling order from Customer #%d\n", cook_id, order->customer_id);
        
        free(order); // Free memory allocated for fulfilled orders
    }

    return NULL;
}

/* Runs when the program begins executing. This program should:*/
/*  - open the restaurant (instantiate a restaurant [i.e. allocate memory])*/
/*  - create customers and cooks (Create Threads)*/
/*  - wait for all customers and cooks to be done (join)*/
/*  - close the restaurant.*/
int main() {
    srand(time(NULL)); // Seed random number generator

    // Open the restaurant with defined maximum size.
    bcb = OpenRestaurant(BENSCHILLIBOWL_SIZE, NUM_CUSTOMERS * ORDERS_PER_CUSTOMER);

    pthread_t customers[NUM_CUSTOMERS];
    pthread_t cooks[NUM_COOKS];

    // Create customer threads.
    for (long i = 0; i < NUM_CUSTOMERS; i++) {
        pthread_create(&customers[i], NULL, BENSCHILLIBOWLCustomer, (void*)i);
    }

   // Create cook threads.
   for (long i = 0; i < NUM_COOKS; i++) {
       pthread_create(&cooks[i], NULL, BENSCHILLIBOWLCook, (void*)i);
   }

   // Wait for all customers to finish.
   for (int i = 0; i < NUM_CUSTOMERS; i++) {
       pthread_join(customers[i], NULL);
   }

   // Optionally signal cooks to stop if they are waiting indefinitely.
   
   // Wait for all cooks to finish.
   for (int i = 0; i < NUM_COOKS; i++) {
       pthread_join(cooks[i], NULL);
   }

   // Close the restaurant.
   CloseRestaurant(bcb);

   return 0;
}