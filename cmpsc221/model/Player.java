package edu.psu.cmpsc221.model;

import edu.psu.cmpsc221.exceptions.CantGoDirectionException;
import edu.psu.cmpsc221.exceptions.InventoryFullException;
import edu.psu.cmpsc221.exceptions.ItemNotInInventoryException;

public class Player extends MobileCharacter {
    public Player() {
        // We cannot initialize the current room yet.  We only have a proto-object!
        inventorySystem = new ZeroInventorySystem();
    } /* end Player */

    void addToCurrentRoomInventory(Item item) {
        getCurrentRoom().addToInventory(item);
    } /* end addToCurrentRoomInventory */

    private void addToInventory(Item item) throws InventoryFullException {
        inventorySystem.add(item);
    } /* end addToInventory */

    public void drop(String itemName) {
        try {
            Item item = removeFromInventory(itemName);
            addToCurrentRoomInventory(item);
            getController().processDropSuccessful(itemName);
        } catch (ItemNotInInventoryException e) {
            getController().processDropItemNotInInventory(e.getMessage());
        } /* end catch */
    } /* end drop */

    public void get(String itemName) {
        try {
            Item item = removeFromCurrentRoomInventory(itemName);
            addToInventory(item);
            getController().processGetSuccessful(itemName);
        } catch (ItemNotInInventoryException e) {
            getController().processGetItemNotInInventory(itemName);
        } catch (InventoryFullException e) {
            addToCurrentRoomInventory(e.getItem());
            getController().processInventoryFullException(e.getMessage());
        } /* end try */
    } /* end get */

    String getInventoryString() {
        return inventorySystem.getInventoryString();
    } /* end getInventoryString */

    public void go(Direction direction) {
        try {
            super.go(direction);
            getController().processGoDirectionSuccessful();
        } catch (CantGoDirectionException e) {
            getController().processCantGoDirection(e.getMessage());
        } /* end try */
    } /* end go */

    void initialize() {
        initializeCurrentRoom();
    } /* end initialize */

    protected void initializeCurrentRoom() {
        setCurrentRoom(getModel().getInitialPlayerRoom());
    } /* end initializeCurrentRoom */

    private Item removeFromCurrentRoomInventory(String itemName) throws ItemNotInInventoryException {
        return getCurrentRoom().removeFromInventory(itemName);
    } /* end removeFromCurrentRoomInventory */

    private Item removeFromInventory(String itemName) throws ItemNotInInventoryException {
        return inventorySystem.removeItemNamed(itemName);
    } /* end removeFromInventory */

    private InventorySystem inventorySystem;
} /* end Player */
