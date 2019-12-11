package edu.psu.cmpsc221.model;

public class InfiniteInventorySystem extends InventorySystem {
    protected boolean canAddItem(Item item) {
        return true;
    } /* end canAddItem */

    protected String getInventoryFullMessage() {
        // This can't occur in this mode so just return an empty string
        return "";
    } /* end getInventoryFullMessage */
} /* end InfiniteInventorySystem */
