package edu.psu.cmpsc221.model;

import edu.psu.cmpsc221.exceptions.InventoryFullException;
import edu.psu.cmpsc221.exceptions.ItemNotInInventoryException;

import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;

public abstract class InventorySystem extends ModelObject {
    protected InventorySystem() {
        list = new ArrayList<>();
    } /* end InventorySystem */

    void add(Item item) throws InventoryFullException {
        if (canAddItem(item)) {
            list.add(item);
        } else {
            throw new InventoryFullException(getInventoryFullMessage(), item);
        } /* end if */
    } /* end add */

    protected abstract boolean canAddItem(Item item);

    void dropAll() {
        while (!list.isEmpty()) {
            try {
                drop(list.get(0));
            } catch (ItemNotInInventoryException e) {
                // No-op.  This can't occur
            } /* end try */
        } /* end while */
    } /* end dropAll */

    void drop(Item item) throws ItemNotInInventoryException {
        remove(item);
        getModel().addToCurrentRoomInventory(item);
    } /* end drop */

    protected abstract String getInventoryFullMessage();

    String getInventoryString() {
        String inventoryString =
           (list.isEmpty() ?
            "You aren't carrying anything." :
            "You are carrying: " + list.stream().map(Item::getName).collect(Collectors.joining(", ")));

        return inventoryString;
    } /* end getInventoryString */

    String getLookItems() {
        String lookItems =
           (list.isEmpty() ?
            "" :
            list.stream().map(Item::getLookDescription).collect(Collectors.joining(System.lineSeparator())) + System.lineSeparator());

        return lookItems;
    } /* end getLookItems */

    protected void remove(Item item) throws ItemNotInInventoryException {
        if (!list.remove(item)) throw new ItemNotInInventoryException(item);
    } /* end remove */

    Item removeItemNamed(String itemName) throws ItemNotInInventoryException {
        // Can use lambdas, but we'll do it manually
        for (Item item : list) {
            if (item.getName().equals(itemName)) {
                list.remove(item);
                return item;
            } /* end if */
        } /* end for */

        throw new ItemNotInInventoryException(itemName);
    } /* end removeItemNamed */

    List<Item> list;
} /* end InventorySystem */
