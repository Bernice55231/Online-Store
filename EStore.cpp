#include <cassert>

#include "EStore.h"

using namespace std;


Item::
Item() : valid(false)
{
}

Item::
~Item()
{
}


EStore::
EStore(bool enableFineMode)
    : fineMode(enableFineMode)
{
    // TODO: Your code here.
    smutex_init(&item_mutex);
    for(int i = 0; i < INVENTORY_SIZE; i++) {
        smutex_init(&mutex[i]);
    }
    scond_init(&available);

    store_discount = 0;
    shipping_cost = 0.3;
    
    for(int i = 0; i < INVENTORY_SIZE; i++) {
        Item* item = new Item();
        item->discount = 0;
        item->price = 0;
        item->quantity = 0;
        item->valid = false;
        inventory[i] = *item;

    }
    
}

EStore::
~EStore()
{
    // TODO: Your code here.
    smutex_destroy(&item_mutex);
     for(int i = 0; i < INVENTORY_SIZE; i++) {
        smutex_destroy(&mutex[i]);
    }
    scond_destroy(&available);

}

/*
 * ------------------------------------------------------------------
 * buyItem --
 *
 *      Attempt to buy the item from the store.
 *
 *      An item can be bought if:
 *          - The store carries it.
 *          - The item is in stock.
 *          - The cost of the item plus the cost of shipping is no
 *            more than the budget.
 *
 *      If the store *does not* carry this item, simply return and
 *      do nothing. Do not attempt to buy the item.
 *
 *      If the store *does* carry the item, but it is not in stock
 *      or its cost is over budget, block until both conditions are
 *      met (at which point the item should be bought) or the store
 *      removes the item from sale (at which point this method
 *      returns). 
 *
 *      The overall cost of a purchase for a single item is defined
 *      as the current cost of the item times 1 - the store
 *      discount, plus the flat overall store shipping fee.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void EStore::
buyItem(int item_id, double budget)
{
    assert(!fineModeEnabled());

    // TODO: Your code here.
    smutex_lock(&mutex[item_id]);
    Item item = inventory[item_id];
    
    if(item.valid) {
        int actual_cost = item.price * (1 - item.discount) * (1 - store_discount) + shipping_cost;
        while (item.valid && ((item.quantity == 0) | (actual_cost > budget))) {
            scond_wait(&available, &item_mutex);
            // update the cost
            actual_cost = item.price * (1 - item.discount) * (1 - store_discount) + shipping_cost;
        }
        if(item.valid) {
            --inventory[item_id].quantity;
            
        }

    }
    smutex_unlock(&mutex[item_id]);
    return;
    
}

/*
 * ------------------------------------------------------------------
 * buyManyItem --
 *
 *      Attempt to buy all of the specified items at once. If the
 *      order cannot be bought, give up and return without buying
 *      anything. Otherwise buy the entire order at once.
 *
 *      The entire order can be bought if:
 *          - The store carries all items.
 *          - All items are in stock.
 *          - The cost of the the entire order (cost of items plus
 *            shipping for each item) is no more than the budget.
 *
 *      If multiple customers are attempting to buy at the same
 *      time and their orders are mutually exclusive (i.e., the
 *      two customers are not trying to buy any of the same items),
 *      then their orders must be processed at the same time.
 *
 *      For the purposes of this lab, it is OK for the store
 *      discount and shipping cost to change while an order is being
 *      processed.
 *
 *      The cost of a purchase of many items is the sum of the
 *      costs of purchasing each item individually. The purchase
 *      cost of an individual item is covered above in the
 *      description of buyItem.
 *
 *      Challenge: For bonus points, implement a version of this
 *      method that will wait until the order can be fulfilled
 *      instead of giving up. The implementation should be efficient
 *      in that it should not wake up threads unecessarily. For
 *      instance, if an item decreases in price, only threads that
 *      are waiting to buy an order that includes that item should be
 *      signaled (though all such threads should be signaled).
 *
 *      Challenge: For bonus points, ensure that the shipping cost
 *      and store discount does not change while processing an
 *      order.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void EStore::
buyManyItems(vector<int>* item_ids, double budget)
{
    assert(fineModeEnabled());

    // TODO: Your code here.
    
    // bool check_status = true;
    int total_budget = 0;
    int sz = (int) item_ids->size();

    // check whether the customer can buy these items:
    for (int i = 0; i < sz; i++) {
        smutex_lock(&mutex[item_ids->at(i)]);
        while(!inventory[item_ids->at(i)].valid) {
           scond_wait(&available, &mutex[item_ids->at(i)])
        }
        while(inventory[item_ids->at(i)].quantity == 0) {
            scond_wait(&available, &mutex[item_id])
        }
        total_budget += inventory[item_ids->at(i)].price * (1 - inventory[item_ids->at(i)].discount);
        smutex_unlock(&mutex[item_ids->at(i)]);
    }

    while(total_budget < budget) {
        scond_wait(&available, &mutex[item_id]);
        for (int i = 0; i < sz; i++) {
            while(!inventory[item_ids->at(i)].valid) {
            scond_wait(&available, &mutex[item_ids->at(i)])
            }
            while(inventory[item_ids->at(i)].quantity == 0) {
                scond_wait(&available, &mutex[item_id])
            }
            total_budget += inventory[item_ids->at(i)].price * (1 - inventory[item_ids->at(i)].discount);
        }
    }
    if(!check_status) {
        for (int i = 0; i < sz; i++) {
            Item item = inventory[item_ids->at(i)];
            --inventory[item_id].quantity;
            budget -= item.price * (1 - item.discount);
        }
    }
    smutex_unlock(&mutex[i]);
    return;


}

/*
 * ------------------------------------------------------------------
 * addItem --
 *
 *      Add the item to the store with the specified quantity,
 *      price, and discount. If the store already carries an item
 *      with the specified id, do nothing.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void EStore::
addItem(int item_id, int quantity, double price, double discount)
{
    // TODO: Your code here.
    smutex_lock(&mutex[item_id]);
    if(!inventory[item_id].valid) {
        inventory[item_id].quantity = quantity;
        inventory[item_id].price = price;
        inventory[item_id].discount = discount;
        inventory[item_id].valid = true;
    }
    smutex_unlock(&mutex[item_id]);
    return;
}

/*
 * ------------------------------------------------------------------
 * removeItem --
 *
 *      Remove the item from the store. The store no longer carries
 *      this item. If the store is not carrying this item, do
 *      nothing.
 *
 *      Wake any waiters.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void EStore::
removeItem(int item_id)
{
    // TODO: Your code here.
    smutex_lock(&mutex[item_id]);
    if(inventory[item_id].valid) {
        inventory[item_id].valid = false;
        scond_broadcast(&available, &mutex[item_id]);
    }
    smutex_unlock(&mutex[item_id]);
    return;
}

/*
 * ------------------------------------------------------------------
 * addStock --
 *
 *      Increase the stock of the specified item by count. If the
 *      store does not carry the item, do nothing. Wake any waiters.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void EStore::
addStock(int item_id, int count)
{
    // TODO: Your code here.
    smutex_lock(&mutex[item_id]);
    if(inventory[item_id].valid) {
        inventory[item_id].quantity += count;
    }
    if(inventory[item_id].quantity > 0)
        scond_broadcast(&available, &mutex[item_id]);
    smutex_unlock(&mutex[item_id]);
    return;
}

/*
 * ------------------------------------------------------------------
 * priceItem --
 *
 *      Change the price on the item. If the store does not carry
 *      the item, do nothing.
 *
 *      If the item price decreased, wake any waiters.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void EStore::
priceItem(int item_id, double price)
{
    // TODO: Your code here.
    smutex_lock(&mutex[item_id]);
    if(inventory[item_id].valid) {
        inventory[item_id].price = price;
        scond_broadcast(&available, &mutex[item_id]);
    }
    smutex_unlock(&mutex[item_id]);
    return;

}

/*
 * ------------------------------------------------------------------
 * discountItem --
 *
 *      Change the discount on the item. If the store does not carry
 *      the item, do nothing.
 *
 *      If the item discount increased, wake any waiters.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void EStore::
discountItem(int item_id, double discount)
{
    // TODO: Your code here.
    smutex_lock(&mutex[item_id]);
    if(inventory[item_id].valid) {
        if(inventory[item_id].discount < discount)
            scond_broadcast(&available, &mutex[item_id]);
        inventory[item_id].discount = discount;
    }
    smutex_unlock(&mutex[item_id]);
    return;
}

/*
 * ------------------------------------------------------------------
 * setShippingCost --
 *
 *      Set the per-item shipping cost. If the shipping cost
 *      decreased, wake any waiters.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void EStore::
setShippingCost(double cost)
{
    // TODO: Your code here.
    smutex_lock(&mutex[item_id]);
    shipping_cost = cost;
    scond_broadcast(&available, &mutex[item_id]);
    smutex_unlock(&mutex[item_id]);
    return;
}

/*
 * ------------------------------------------------------------------
 * setStoreDiscount --
 *
 *      Set the store discount. If the discount increased, wake any
 *      waiters.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void EStore::
setStoreDiscount(double discount)
{
    // TODO: Your code here.
    smutex_lock(&mutex[item_id]);
    store_discount = discount;
    scond_broadcast(&available, &mutex[item_id]x);
    smutex_unlock(&mutex[item_id]);
    return;
}


