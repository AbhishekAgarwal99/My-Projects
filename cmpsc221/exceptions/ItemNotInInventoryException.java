package edu.psu.cmpsc221.exceptions;

import edu.psu.cmpsc221.model.Item;

public class ItemNotInInventoryException extends Exception {
    public ItemNotInInventoryException(String itemName) {
        super(itemName);
    } /* end ItemNotInInventoryException */

    public ItemNotInInventoryException(Item item) {
        super(item.getName());
    } /* end ItemNotInInventoryException */
} /* end ItemNotInInventoryException */
