#ifndef LAB3_BENSCHILLIBOWL_H_
#define LAB3_BENSCHILLIBOWL_H_

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Let a menu item be a string.
typedef char* MenuItem;

// Constants for restaurant configuration
#define BENSCHILLIBOWL_SIZE 10  // Maximum orders the restaurant can hold
#define NUM_CUSTOMERS 5        // Number of customers
#define ORDERS_PER_CUSTOMER 3  // Number of orders each customer places

// Contents of an Order.
typedef struct OrderStruct {
    MenuItem menu_item;
    int customer_id;
    int order_number;
    struct OrderStruct *next;  // Pointer to the next order (linked list)
} Order;

// A restaurant contains:
//  - Pointer to the head of the orders queue (linked list)
//  - Its current size (number of orders currently in the queue)
//  - Its maximum size (maximum number of orders it can handle)
//  - The next order number to be assigned
//  - The number of orders fulfilled
//  - The expected number of orders the restaurant should fulfill
//  - Synchronization objects for thread safety
typedef struct Restaurant {
    Order* orders;              // Head of the orders queue (linked list)
    int current_size;           // Current number of orders in the queue
    int max_size;               // Maximum size of the queue
    int next_order_number;      // Next order number to assign
    int orders_handled;         // Total number of orders fulfilled
    int expected_num_orders;    // Total number of orders expected
    pthread_mutex_t mutex;      // Mutex for thread safety
    pthread_cond_t can_add_orders; // Condition variable: when orders can be added
    pthread_cond_t can_get_orders; // Condition variable: when orders can be retrieved
} BENSCHILLIBOWL;

/**
 * Picks a random menu item and returns it.
 */
MenuItem PickRandomMenuItem();

/**
 * Creates a restaurant with a maximum size and the expected number of orders.
 * Returns the restaurant.
 * 
 * This function should:
 *  - Allocate space for the restaurant
 *  - Initialize all its variables
 *  - Initialize its synchronization objects
 */
BENSCHILLIBOWL* OpenRestaurant(int max_size, int expected_num_orders);

/**
 * Closes the restaurant. This function should:
 *  - Ensure all orders have been fulfilled
 *  - Ensure the number of orders fulfilled matches the expected number of orders
 *  - Destroy all the synchronization objects
 *  - Free the space of the restaurant
 */
void CloseRestaurant(BENSCHILLIBOWL* bcb);

/**
 * Add an order to the restaurant. This function should:
 *  - Wait until the restaurant is not full
 *  - Add an order to the back of the orders queue
 *  - Populate the order number of the order
 *  - Return the order number
 */
int AddOrder(BENSCHILLIBOWL* bcb, Order* order);

/**
 * Gets an order from the restaurant. This function should:
 *  - Wait until the restaurant is not empty
 *  - Get an order from the front of the orders queue
 *  - Return the order
 * 
 * If there are no orders left, this function should notify the other cooks
 * that there are no orders left.
 */
Order* GetOrder(BENSCHILLIBOWL* bcb);

/**
 * Helper function to check if the restaurant's queue is full.
 */
bool IsFull(BENSCHILLIBOWL* bcb);

#endif  // LAB3_BENSCHILLIBOWL_H_
