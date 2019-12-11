package edu.psu.cmpsc221.model;

public class Model {
    private static Model instance;

    static {
        instance = new Model();
        // The model, itself, as well as both the map and the player must exist
        // before they can be properly initialized.
        instance.initialize();
    } /* end static */

    protected Model() {
        map = new Map();
        player = new Player();
    } /* end Model */

    void addToCurrentRoomInventory(Item item) {
        getPlayer().addToCurrentRoomInventory(item);
    } /* end addToCurrentRoomInventory */

    public void drop(String itemName) {
        getPlayer().drop(itemName);
    } /* end drop */

    public void get(String itemName) {
        getPlayer().get(itemName);
    } /* end get */

    public String getCurrentRoomLookDescription() {
        return getPlayer().getCurrentRoomLookDescription();
    } /* end getCurrentRoomLookDescription */

    public String getCurrentRoomLookExits() {
        return getPlayer().getCurrentRoomLookExits();
    } /* end getCurrentRoomLookExits */

    public String getCurrentRoomLookItems() {
        return getPlayer().getCurrentRoomLookItems();
    } /* end getCurrentRoomLookItems*/

    public static Model getInstance() {
        return instance;
    } /* end getInstance */

    public String getInventoryString() {
        return getPlayer().getInventoryString();
    } /* end getInventoryString */

    public Room getInitialPlayerRoom() {
        return getMap().getInitialPlayerRoom();
    } /* end getInitialPlayerRoom */

    Map getMap() {
        return map;
    } /* end getMap */

    Player getPlayer() {
        return player;
    } /* end getPlayer */

    public void go(Direction direction) {
        getPlayer().go(direction);
    } /* end go */

    private void initialize() {
        getPlayer().initialize();
    } /* end initialize */

    private Map map;
    private Player player;
} /* end Model */
