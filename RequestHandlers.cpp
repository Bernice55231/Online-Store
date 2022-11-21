#include <iostream>
#include <cstdlib>
#include <cassert>
#include <set>

#include "Request.h"
#include "RequestHandlers.h"
#include "EStore.h"
/*
 * ------------------------------------------------------------------
 * add_item_handler --
 *
 *      Handle an AddItemReq.
 *
 *      Delete the request object when done.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void 
add_item_handler(void *args)
{
    // TODO: Your code here.
    AddItemReq* req = (AddItemReq *) args;
    printf("Handling AddItemReq: ");
    printf("item_id - %d, ", req->item_id);
    printf("quantity - %d, ", req->quantity);
    printf("price - %f, ", req->price);
    printf("discount - %f\n", req->discount);
    req->store->addItem(req->item_id, req->quantity, req->price, req->discount);
    delete req;

}

/*
 * ------------------------------------------------------------------
 * remove_item_handler --
 *
 *      Handle a RemoveItemReq.
 *
 *      Delete the request object when done.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void 
remove_item_handler(void *args)
{
    // TODO: Your code here.
    RemoveItemReq* req = (RemoveItemReq *) args;
    printf("Handling RemoveItemReq: ");
    printf("item_id - %d\n", req->item_id);
    req->store->removeItem(req->item_id);
    delete req;
}

/*
 * ------------------------------------------------------------------
 * add_stock_handler --
 *
 *      Handle an AddStockReq.
 *
 *      Delete the request object when done.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void 
add_stock_handler(void *args)
{
    // TODO: Your code here.
    AddStockReq* req = (AddStockReq *) args;
    printf("Handling AddStockReq: ");
    printf("item_id - %d, ", req->item_id);
    printf("additional stock - %d\n", req->additional_stock);
    req->store->addStock(req->item_id, req->additional_stock);
    delete req;
}

/*
 * ------------------------------------------------------------------
 * change_item_price_handler --
 *
 *      Handle a ChangeItemPriceReq.
 *
 *      Delete the request object when done.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void 
change_item_price_handler(void *args)
{
    // TODO: Your code here.
    ChangeItemPriceReq* req = (ChangeItemPriceReq *) args;
    printf("Handling ChangeItemPriceReq: ");
    printf("item_id - %d, ", req->item_id);
    printf("price - %f\n", req->new_price);
    req->store->priceItem(req->item_id, req->new_price);
    delete req;
}

/*
 * ------------------------------------------------------------------
 * change_item_discount_handler --
 *
 *      Handle a ChangeItemDiscountReq.
 *
 *      Delete the request object when done.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void 
change_item_discount_handler(void *args)
{
    // TODO: Your code here.
    ChangeItemDiscountReq* req = (ChangeItemDiscountReq *) args;
    printf("Handling ChangeItemDiscountReq: ");
    printf("item_id - %d, ", req->item_id);
    printf("discount - %f\n", req->new_discount);
    req->store->discountItem(req->item_id, req->new_discount);
    delete req;
}

/*
 * ------------------------------------------------------------------
 * set_shipping_cost_handler --
 *
 *      Handle a SetShippingCostReq.
 *
 *      Delete the request object when done.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void 
set_shipping_cost_handler(void *args)
{
    // TODO: Your code here.
    SetShippingCostReq* req = (SetShippingCostReq *) args;
    printf("Handling SetShippingCostReq: ");
    printf("shipping price - %f\n", req->new_cost);
    req->store->setShippingCost(req->new_cost);
    delete req;
}

/*
 * ------------------------------------------------------------------
 * set_store_discount_handler --
 *
 *      Handle a SetStoreDiscountReq.
 *
 *      Delete the request object when done.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void
set_store_discount_handler(void *args)
{
    // TODO: Your code here.
    SetStoreDiscountReq* req = (SetStoreDiscountReq *) args;
    printf("Handling SetStoreDiscountReq: ");
    printf("store discount - %f\n", req->new_discount);
    req->store->setStoreDiscount(req->new_discount);
    delete req;
}

/*
 * ------------------------------------------------------------------
 * buy_item_handler --
 *
 *      Handle a BuyItemReq.
 *
 *      Delete the request object when done.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void
buy_item_handler(void *args)
{
    // TODO: Your code here.
    BuyItemReq* req = (BuyItemReq *) args;
    printf("Handling BuyItemReq: ");
    printf("item_id - %d, ", req->item_id);
    printf("customer budget - %f\n", req->budget);
    req->store->buyItem(req->item_id, req->budget);
    delete req;
}

/*
 * ------------------------------------------------------------------
 * buy_many_items_handler --
 *
 *      Handle a BuyManyItemsReq.
 *
 *      Delete the request object when done.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void
buy_many_items_handler(void *args)
{
    // TODO: Your code here.
    BuyManyItemsReq* req = (BuyManyItemsReq *) args;
    printf("Handling BuyManyItemsReq: ");
    printf("item_id -  ");
    int sz = (int) req->item_ids.size();
    for(int i = 0; i < sz; i++)
        printf("%d ", req->item_ids[i]);
    printf("customer budget - %f\n", req->budget);
    req->store->buyManyItems(&(req->item_ids), req->budget);
    delete req;
}

/*
 * ------------------------------------------------------------------
 * stop_handler --
 *
 *      The thread should exit.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void 
stop_handler(void* args)
{
    // TODO: Your code here.
    printf("Handling StopHandlerReq: Quitting.\n");
    sthread_exit();
}

