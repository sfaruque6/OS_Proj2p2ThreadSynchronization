//worked with Stephanie Percy

#include "BENSCHILLIBOWL.h"

#include <assert.h>
#include <stdlib.h>
#include <time.h>

bool IsEmpty(BENSCHILLIBOWL* bcb);
bool IsFull(BENSCHILLIBOWL* bcb);
void AddOrderToBack(Order **orders, Order *order);

MenuItem BENSCHILLIBOWLMenu[] = { 
    "BensChilli", 
    "BensHalfSmoke", 
    "BensHotDog", 
    "BensChilliCheeseFries", 
    "BensShake",
    "BensHotCakes",
    "BensCake",
    "BensHamburger",
    "BensVeggieBurger",
    "BensOnionRings",
};
int BENSCHILLIBOWLMenuLength = 10;

/* Select a random item from the Menu and return it */
MenuItem PickRandomMenuItem() {
  int randnum = rand() % BENSCHILLIBOWLMenuLength;
  return BENSCHILLIBOWLMenu[randnum];
}

/* Allocate memory for the Restaurant, then create the mutex and condition variables needed to instantiate the Restaurant */

BENSCHILLIBOWL* OpenRestaurant(int max_size, int expected_num_orders) {
    BENSCHILLIBOWL *restaurant = (BENSCHILLIBOWL*) malloc(sizeof(BENSCHILLIBOWL));
    restaurant->orders = NULL;
    restaurant->current_size = 0;
    restaurant->max_size = max_size;
    restaurant->next_order_number = 1;
    restaurant->orders_handled = 0;
    restaurant->expected_num_orders = expected_num_orders;
    printf("Restaurant is open!\n");
    pthread_mutex_init(&(restaurant->mutex), NULL); 
    pthread_cond_init(&(restaurant->can_add_orders), NULL);
    pthread_cond_init(&(restaurant->can_get_orders), NULL);
    return NULL;
}


/* check that the number of orders received is equal to the number handled (ie.fullfilled). Remember to deallocate your resources */

void CloseRestaurant(BENSCHILLIBOWL* bcb) {
  printf("Orders handled: %d\n", bcb->orders_handled);
  printf("Orders expected: %d\n", bcb->expected_num_orders);
  //check condition
  if(bcb->expected_num_orders == bcb->orders_handled){
    pthread_mutex_destroy(&bcb->mutex);
    pthread_cond_destroy(&bcb->can_add_orders);
    pthread_cond_destroy(&bcb->can_get_orders);
  }
  free(bcb); //deletes restaurant
    printf("Restaurant is closed!\n");
}

/* add an order to the back of queue */
int AddOrder(BENSCHILLIBOWL* bcb, Order* order) {
  pthread_mutex_lock(&(bcb->mutex));
  while (IsFull(bcb)){
    pthread_cond_wait(&(bcb->can_add_orders), &(bcb->mutex));
  }
   bcb->current_size++;
   bcb->next_order_number++;
   AddOrderToBack(&(bcb->orders), order);
    
   pthread_cond_broadcast(&(bcb->can_get_orders));
   pthread_mutex_unlock(&(bcb->mutex));
   return bcb->next_order_number - 1;
}

/* remove an order from the queue */
Order *GetOrder(BENSCHILLIBOWL* bcb) {
    pthread_mutex_lock(&(bcb->mutex));
    while (IsEmpty(bcb)){
      if (bcb->orders_handled >= bcb->expected_num_orders) {
          pthread_mutex_unlock(&(bcb->mutex));
          return NULL;
      }
      pthread_cond_wait(&(bcb->can_get_orders), &bcb->mutex);
    }
  Order *get = bcb->orders;
  bcb->orders = bcb->orders->next;
  bcb->current_size--;
  bcb->orders_handled++;
  pthread_cond_signal(&(bcb->can_add_orders));
  if (bcb->orders_handled == bcb->expected_num_orders){
    pthread_cond_broadcast(&(bcb->can_add_orders));
  }
  pthread_mutex_unlock(&(bcb->mutex));
  return get;
}

// Optional helper functions (you can implement if you think they would be useful)
bool IsEmpty(BENSCHILLIBOWL* bcb) {
 return bcb->current_size == 0;
}

bool IsFull(BENSCHILLIBOWL* bcb) {
  return bcb->current_size == bcb->max_size;
}

/* this methods adds order to rear of queue */
void AddOrderToBack(Order **orders, Order *order) {
  if (!orders[0]){
    orders[0] = order;
    order->next = NULL;
  }
  Order *temp = orders[0];
  while (temp->next != NULL){
    temp = temp->next;
  }
  temp->next = order;
  order->next = NULL;
  return;
}
