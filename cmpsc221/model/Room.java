package edu.psu.cmpsc221.model;

import edu.psu.cmpsc221.exceptions.CantGoDirectionException;
import edu.psu.cmpsc221.exceptions.InventoryFullException;
import edu.psu.cmpsc221.exceptions.ItemNotInInventoryException;

import java.util.HashMap;
import java.util.Map;
import java.util.stream.Collectors;

/**
 * This class models rooms in our adventure game
 */
public class Room {

    Room(String lookDescription, String name) {
        this.exits = new HashMap<>();
        this.inventorySystem = new InfiniteInventorySystem();
        this.lookDescription = lookDescription;
        this.name = name;
    } /* end Room */

    void addCompassExit(CompassDirection compassDirection, Room toRoom) {
        addExit(compassDirection, toRoom);
        toRoom.addExit(compassDirection.getOppositeCompassDirection(), this);
    } /* end addCompassExit */

    void addExit(Direction direction, Room toRoom) {
        exits.put(direction, toRoom);
    } /* end addExit */

    void addToInventory(Item item) {
        try {
            inventorySystem.add(item);
        } catch (InventoryFullException e) {
            // No-op.  This won't occur.
        } /* end try */
    } /* end addToInventory*/

    private Room getExitForDirection(Direction direction) throws CantGoDirectionException {
        Room candidateRoom = exits.get(direction);
        if (null == candidateRoom) throw new CantGoDirectionException(direction);
        return candidateRoom;
    } /* end getExitForDirection */

    public String getLookDescription() {
        return lookDescription;
    } /* end getLookDescription */

    public String getLookExits() {
        String lookExits = "Obvious exits are to the ";
        String exitsAsString =
           exits.keySet().stream().map(Direction::getName).collect(Collectors.joining(", "));
        lookExits += exitsAsString; // lookExits = lookExits + exitsAsString;
        return lookExits;
    } /* end getLookExits */

    public String getLookItems() {
        return inventorySystem.getLookItems();
    } /* end getLookItems */

    public Room go(Direction direction) throws CantGoDirectionException {
        return getExitForDirection(direction);
    } /* end go */

    Item removeFromInventory(String itemName) throws ItemNotInInventoryException {
        return inventorySystem.removeItemNamed(itemName);
    } /* end removeFromInventory */

    private Map<Direction, Room> exits;
    private InventorySystem inventorySystem;
    private String lookDescription;
    private String name;
} /* end Room */
