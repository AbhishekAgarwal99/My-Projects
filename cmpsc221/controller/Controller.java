package edu.psu.cmpsc221.controller;

import edu.psu.cmpsc221.AdventureObject;
import edu.psu.cmpsc221.model.Direction;
import edu.psu.cmpsc221.model.Model;
import edu.psu.cmpsc221.view.TextView;
import edu.psu.cmpsc221.view.View;

public class Controller extends AdventureObject {
    public Controller() {
        initializeKeepPlaying();
        initializeParser();
        initializeView();
    } /* end Controller */

    public void drop(String itemName) {
        getModel().drop(itemName);
    } /* end drop */

    public void get(String itemName) {
        getModel().get(itemName);
    } /* end get */

    public String getCurrentRoomLookDescription() {
        return getModel().getCurrentRoomLookDescription();
    } /* end getCurrentRoomLookDescription */

    public String getCurrentRoomLookExits() {
        return getModel().getCurrentRoomLookExits();
    } /* end getCurrentRoomLookExits */

    public String getCurrentRoomLookItems() {
        return getModel().getCurrentRoomLookItems();
    } /* end getCurrentRoomLookItems */

    public static Controller getInstance() {
        return instance;
    } /* end getInstance */

    public String getInventoryString() {
        return getModel().getInventoryString();
    } /* end getInventoryString*/

    private boolean getKeepPlaying() {
        return keepPlaying;
    } /* end getKeepPlaying */

    private Model getModel() {
        return Model.getInstance();
    } /* end getModel */

    private Parser getParser() {
        return parser;
    } /* end getParser */

    private View getView() {
        return view;
    } /* end View */

    public void go(Direction direction) {
        getModel().go(direction);
    } /* end go */

    private void initializeKeepPlaying() {
        setKeepPlaying(true);
    } /* end initializeKeepPlaying */

    private void initializeParser() {
        parser = new Parser();
    } /* end initializeParser */

    private void initializeView() {
        view = new TextView();
    } /* end initializeView */

    public void inventory() {
        getView().inventory();
    } /* end inventory */

    private void parseCommand(String command) {
        getParser().parseCommand(command);
    } /* end parseCommand */

    public void processCantGoDirection(String directionName) {
        getView().processCantGoDirection(directionName);
    } /* end processCantGoDirection */

    public void processDropItemNotInInventory(String itemName) {
        getView().processDropItemNotInInventory(itemName);
    } /* end processDropItemNotInInventory */

    public void processDropSuccessful(String itemName) {
        getView().processDropSuccessful(itemName);
    } /* end processDropSuccessful */

    public void processGetItemNotInInventory(String itemName) {
        getView().processGetItemNotInInventory(itemName);
    } /* end processGetItemNotInInventory */

    public void processGetSuccessful(String itemName) {
        getView().processGetSuccessful(itemName);
    } /* end processGetSuccessful */

    public void processGoDirectionSuccessful() {
        getView().look();
    } /* end processGoDirectionSuccessful */

    public void processInventoryFullException(String message) {
        getView().processInventoryFullException(message);
    } /* end processInventoryFullException */

    public void processLook() {
        getView().look();
    } /* end processLook */

    private void processNextUserCommand() {
        String command = getView().getUserCommand();
        parseCommand(command);
    } /* end processNextUserCommand */

    public void processQuitCommand() {
        getView().processQuitCommand();
        setKeepPlaying(false);
    } /* end processQuitCommand */

    public void processUnknownCommand(String command) {
        getView().processUnknownCommand(command);
    } /* end processUnknownCommand */

    public void run() {
        getView().look();

        while (getKeepPlaying()) {
            processNextUserCommand();
        } /* end while */
    } /* end run */

    private void setKeepPlaying(boolean value) {
        keepPlaying = value;
    } /* end setKeepPlaying*/

    private static Controller instance = new Controller();
    private boolean keepPlaying;
    private Parser parser;
    private View view;
} /* end Controller */
